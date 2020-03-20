//
// Copyright (c) 2020 LAAS, INRIA
//

#ifndef __pinocchio_algorithm_contact_dynamics_derivatives_hxx__
#define __pinocchio_algorithm_contact_dynamics_derivatives_hxx__

#include "pinocchio/algorithm/check.hpp"
#include "pinocchio/algorithm/rnea-derivatives.hpp"
#include "pinocchio/algorithm/kinematics-derivatives.hpp"
#include "pinocchio/algorithm/contact-cholesky.hpp"

namespace pinocchio
{

  template<typename Scalar, int Options, template<typename,int> class JointCollectionTpl, typename ConfigVectorType, typename TangentVectorType1, typename TangentVectorType2, class Allocator, class AllocatorData, typename MatrixType1, typename MatrixType2, typename MatrixType3, typename MatrixType4, typename MatrixType5, typename MatrixType6>
  inline void computeContactDynamicsDerivatives(const ModelTpl<Scalar,Options,JointCollectionTpl> & model,
                                                DataTpl<Scalar,Options,JointCollectionTpl> & data,
                                                const Eigen::MatrixBase<ConfigVectorType> & q,
                                                const Eigen::MatrixBase<TangentVectorType1> & v,
                                                const Eigen::MatrixBase<TangentVectorType2> & tau,
                                                const std::vector<RigidContactModelTpl<Scalar,Options>,Allocator> & contact_models,
                                                const std::vector<RigidContactDataTpl<Scalar,Options>, AllocatorData> & contact_datas,
                                                const Scalar mu,
                                                const Eigen::MatrixBase<MatrixType1> & ddq_partial_dq,
                                                const Eigen::MatrixBase<MatrixType2> & ddq_partial_dv,
                                                const Eigen::MatrixBase<MatrixType3> & ddq_partial_dtau,
                                                const Eigen::MatrixBase<MatrixType4> & lambda_partial_dq,
                                                const Eigen::MatrixBase<MatrixType5> & lambda_partial_dv,
                                                const Eigen::MatrixBase<MatrixType6> & lambda_partial_dtau)
  {
    const Eigen::DenseIndex& nc = data.contact_chol.constraintDim();
    
    PINOCCHIO_CHECK_INPUT_ARGUMENT(q.size() == model.nq, "The joint configuration vector is not of right size");
    PINOCCHIO_CHECK_INPUT_ARGUMENT(v.size() == model.nv, "The joint velocity vector is not of right size");
    PINOCCHIO_CHECK_INPUT_ARGUMENT(tau.size() == model.nv, "The joint torque vector is not of right size");
    PINOCCHIO_CHECK_INPUT_ARGUMENT(ddq_partial_dq.cols() == model.nv);
    PINOCCHIO_CHECK_INPUT_ARGUMENT(ddq_partial_dq.rows() == model.nv);
    PINOCCHIO_CHECK_INPUT_ARGUMENT(ddq_partial_dv.cols() == model.nv);
    PINOCCHIO_CHECK_INPUT_ARGUMENT(ddq_partial_dv.rows() == model.nv);
    PINOCCHIO_CHECK_INPUT_ARGUMENT(ddq_partial_dtau.cols() == model.nv);
    PINOCCHIO_CHECK_INPUT_ARGUMENT(ddq_partial_dtau.rows() == model.nv);
    PINOCCHIO_CHECK_INPUT_ARGUMENT(lambda_partial_dq.cols() == model.nv);
    PINOCCHIO_CHECK_INPUT_ARGUMENT(lambda_partial_dq.rows() == nc);
    PINOCCHIO_CHECK_INPUT_ARGUMENT(lambda_partial_dv.cols() == model.nv);
    PINOCCHIO_CHECK_INPUT_ARGUMENT(lambda_partial_dv.rows() == nc);
    PINOCCHIO_CHECK_INPUT_ARGUMENT(lambda_partial_dtau.cols() == model.nv);
    PINOCCHIO_CHECK_INPUT_ARGUMENT(lambda_partial_dtau.rows() == nc);
    assert(model.check(data) && "data is not consistent with model.");
    assert(mu >= (Scalar)0 && "mu must be positive.");
    
    typedef ModelTpl<Scalar,Options,JointCollectionTpl> Model;
    typedef DataTpl<Scalar,Options,JointCollectionTpl> Data;
    
    typedef RigidContactModelTpl<Scalar,Options> RigidContactModel;
    typedef RigidContactDataTpl<Scalar,Options> RigidContactData;
    typedef std::vector<RigidContactModel,Allocator> RigidContactModelVector;
    typedef std::vector<RigidContactData,AllocatorData> RigidContactDataVector;
    typedef typename ModelTpl<Scalar,Options,JointCollectionTpl>::JointIndex JointIndex;

    computeRNEADerivatives(model, data, q, v, data.ddq, data.contact_forces);

    data.contact_chol.getInverseMassMatrix(data.Minv);

    Eigen::DenseIndex current_row_sol_id = 0;
    typename RigidContactDataVector::const_iterator it_d = contact_datas.begin();
    for(typename RigidContactModelVector::const_iterator it = contact_models.begin();
        (it != contact_models.end() && it_d != contact_datas.end()); ++it, ++it_d)
    {
      const RigidContactModel & contact_model = *it;
      const RigidContactData & contact_data = *it_d;

      const typename Model::FrameIndex & frame_id = contact_model.frame_id;
      const typename Model::Frame & frame = model.frames[frame_id];
      const typename Model::JointIndex & joint_id = frame.parent;
      
      //TODO: This is only for size 6. replace with contact_model::NC
      switch(contact_model.type)
      {
      case CONTACT_6D:
      {
        typedef typename SizeDepType<6>::template RowsReturn<typename Data::MatrixXs>::Type RowsBlock;
        //TODO: We don't need all these quantities. Remove those not needed.
        getJointAccelerationDerivatives(model, data,
                                        joint_id,
                                        LOCAL,
                                        contact_data.v_partial_dq,
                                        contact_data.a_partial_dq,
                                        contact_data.a_partial_dv,
                                        contact_data.a_partial_da);

        //TODO: replace with contact_model::nc
        RowsBlock contact_dac_dq = SizeDepType<6>::middleRows(data.dac_dq,
                                                              current_row_sol_id);
        RowsBlock contact_dac_dvq = SizeDepType<6>::middleRows(data.dac_dvq,
                                                               current_row_sol_id);
        RowsBlock contact_dac_daq = SizeDepType<6>::middleRows(data.dac_daq,
                                                                  current_row_sol_id);

        contact_dac_daq = contact_data.a_partial_da;

        contact_dac_dq = contact_data.a_partial_dq;
        contact_dac_dvq = contact_data.a_partial_dv;

        int colRef = nv(model.joints[joint_id])+idx_v(model.joints[joint_id])-1;
        for(Eigen::DenseIndex j=colRef;j>=0;j=data.parents_fromRow[(size_t)j]) {
          contact_dac_dq.template topRows<3>().col(j) +=
            data.v[joint_id].angular().cross(contact_data.v_partial_dq.template topRows<3>().col(j))
            - data.v[joint_id].linear().cross(contact_data.v_partial_dq.template bottomRows<3>().col(j));
          contact_dac_dvq.template topRows<3>().col(j) +=
            data.v[joint_id].angular().cross(contact_data.a_partial_da.template topRows<3>().col(j))
            - data.v[joint_id].linear().cross(contact_data.a_partial_da.template bottomRows<3>().col(j));
        }

        //TODO: remplacer par contact_model::NC
        current_row_sol_id += 6;
        break;
      }
      case CONTACT_3D:
      {
        typedef typename SizeDepType<3>::template RowsReturn<typename Data::MatrixXs>::Type RowsBlock;
        //TODO: Specialize for the 3d case.
        //TODO: We don't need all these quantities. Remove those not needed.
        getJointAccelerationDerivatives(model, data,
                                        joint_id,
                                        LOCAL,
                                        contact_data.v_partial_dq,
                                        contact_data.a_partial_dq,
                                        contact_data.a_partial_dv,
                                        contact_data.a_partial_da);

        //TODO: replace with contact_model::nc
        RowsBlock contact_dac_dq = SizeDepType<3>::middleRows(data.dac_dq,
                                                              current_row_sol_id);
        RowsBlock contact_dac_dvq = SizeDepType<3>::middleRows(data.dac_dvq,
                                                               current_row_sol_id);
        RowsBlock contact_dac_daq = SizeDepType<3>::middleRows(data.dac_daq,
                                                                  current_row_sol_id);

        contact_dac_daq.noalias() = contact_data.a_partial_da.template topRows<3>();

        contact_dac_dq = contact_data.a_partial_dq.template topRows<3>();
        contact_dac_dvq = contact_data.a_partial_dv.template topRows<3>();
        
        int colRef = nv(model.joints[joint_id])+idx_v(model.joints[joint_id])-1;
        for(Eigen::DenseIndex j=colRef;j>=0;j=data.parents_fromRow[(size_t)j]) {
          contact_dac_dq.template topRows<3>().col(j) +=
            data.v[joint_id].angular().cross(contact_data.v_partial_dq.template topRows<3>().col(j))
            - data.v[joint_id].linear().cross(contact_data.v_partial_dq.template bottomRows<3>().col(j));
          contact_dac_dvq.template topRows<3>().col(j) +=
            data.v[joint_id].angular().cross(contact_data.a_partial_da.template topRows<3>().col(j))
            - data.v[joint_id].linear().cross(contact_data.a_partial_da.template bottomRows<3>().col(j));
        }
        current_row_sol_id += 3;
        break;
      }
      default:
        assert(false && "must never happen");
        break;
      }
    }
    data.contact_chol.getOperationalSpaceInertiaMatrix(data.osim);
    
    //Temporary: dlambda_dvq stores J*Minv
    //TODO: Sparse
    data.dlambda_dvq.noalias() = data.dac_daq * data.Minv;
    PINOCCHIO_EIGEN_CONST_CAST(MatrixType6,lambda_partial_dtau).noalias() = -data.osim * data.dlambda_dvq; //OUTPUT

    MatrixType3 & ddq_partial_dtau_ = PINOCCHIO_EIGEN_CONST_CAST(MatrixType3,ddq_partial_dtau);
    ddq_partial_dtau_.noalias() = data.dlambda_dvq.transpose() * data.dlambda_dtau;
    ddq_partial_dtau_ += data.Minv; //OUTPUT
    
    data.dac_dq.noalias() -=  data.dlambda_dvq * data.dtau_dq;
    data.dac_dvq.noalias() -= data.dlambda_dvq * data.dtau_dv;

    PINOCCHIO_EIGEN_CONST_CAST(MatrixType4,lambda_partial_dq).noalias() = -data.osim * data.dac_dq; //OUTPUT
    PINOCCHIO_EIGEN_CONST_CAST(MatrixType5,lambda_partial_dv).noalias() = -data.osim * data.dac_dvq; //OUTPUT

    //TODO: SPARSE
    data.dtau_dq.noalias() -= data.dac_daq.transpose() * data.dlambda_dq;
    data.dtau_dv.noalias() -= data.dac_daq.transpose() * data.dlambda_dvq;

    PINOCCHIO_EIGEN_CONST_CAST(MatrixType1,ddq_partial_dq).noalias() = -data.Minv*data.dtau_dq; //OUTPUT
    PINOCCHIO_EIGEN_CONST_CAST(MatrixType1,ddq_partial_dv).noalias() = -data.Minv*data.dtau_dv; //OUTPUT
  }
  
  
} // namespace pinocchio

#endif // ifndef __pinocchio_algorithm_contact_dynamics_hxx__
