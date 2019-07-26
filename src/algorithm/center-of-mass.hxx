//
// Copyright (c) 2015-2018 CNRS
//

#ifndef __pinocchio_center_of_mass_hxx__
#define __pinocchio_center_of_mass_hxx__

#include "pinocchio/algorithm/check.hpp"
#include "pinocchio/multibody/visitor.hpp"
#include "pinocchio/algorithm/kinematics.hpp"
#include "pinocchio/algorithm/jacobian.hpp"

/// @cond DEV

namespace pinocchio
{
  template<typename Scalar, int Options, template<typename,int> class JointCollectionTpl>
  inline Scalar computeTotalMass(const ModelTpl<Scalar,Options,JointCollectionTpl> & model)
  {
    Scalar m = Scalar(0);
    for(JointIndex i=1; i<(JointIndex)(model.njoints); ++i)
    {
      m += model.inertias[i].mass();
    }
    return m;
  }

  template<typename Scalar, int Options, template<typename,int> class JointCollectionTpl>
  inline Scalar computeTotalMass(const ModelTpl<Scalar,Options,JointCollectionTpl> & model,
                                 DataTpl<Scalar,Options,JointCollectionTpl> & data)
  {
    data.mass[0] = computeTotalMass(model);
    return data.mass[0];
  }

  template<typename Scalar, int Options, template<typename,int> class JointCollectionTpl>
  inline void computeSubtreeMasses(const ModelTpl<Scalar,Options,JointCollectionTpl> & model,
                                   DataTpl<Scalar,Options,JointCollectionTpl> & data)
  {
    data.mass[0] = Scalar(0);

    // Forward Step
    for(JointIndex i=1; i<(JointIndex)(model.njoints); ++i)
    {
      data.mass[i] = model.inertias[i].mass();
    }

    // Backward Step
    for(JointIndex i=(JointIndex)(model.njoints-1); i>0; --i)
    {
      const JointIndex & parent = model.parents[i];
      data.mass[parent] += data.mass[i];
    }
  }

  template<typename Scalar, int Options, template<typename,int> class JointCollectionTpl, typename ConfigVectorType>
  inline const typename DataTpl<Scalar,Options,JointCollectionTpl>::Vector3 &
  centerOfMass(const ModelTpl<Scalar,Options,JointCollectionTpl> & model,
               DataTpl<Scalar,Options,JointCollectionTpl> & data,
               const Eigen::MatrixBase<ConfigVectorType> & q,
               const bool computeSubtreeComs)
  {
    forwardKinematics(model,data,q.derived());

    const int LEVEL = 0;
    centerOfMass(model,data,LEVEL,computeSubtreeComs);
    return data.com[0];
  }

  template<typename Scalar, int Options, template<typename,int> class JointCollectionTpl, typename ConfigVectorType, typename TangentVectorType>
  inline const typename DataTpl<Scalar,Options,JointCollectionTpl>::Vector3 &
  centerOfMass(const ModelTpl<Scalar,Options,JointCollectionTpl> & model,
               DataTpl<Scalar,Options,JointCollectionTpl> & data,
               const Eigen::MatrixBase<ConfigVectorType> & q,
               const Eigen::MatrixBase<TangentVectorType> & v,
               const bool computeSubtreeComs)
  {
    forwardKinematics(model,data,q.derived(),v.derived());

    const int LEVEL = 1;
    centerOfMass(model,data,LEVEL,computeSubtreeComs);
    return data.com[0];
  }

  template<typename Scalar, int Options, template<typename,int> class JointCollectionTpl, typename ConfigVectorType, typename TangentVectorType1, typename TangentVectorType2>
  inline const typename DataTpl<Scalar,Options,JointCollectionTpl>::Vector3 &
  centerOfMass(const ModelTpl<Scalar,Options,JointCollectionTpl> & model,
               DataTpl<Scalar,Options,JointCollectionTpl> & data,
               const Eigen::MatrixBase<ConfigVectorType> & q,
               const Eigen::MatrixBase<TangentVectorType1> & v,
               const Eigen::MatrixBase<TangentVectorType2> & a,
               const bool computeSubtreeComs)
  {
    forwardKinematics(model,data,q,v,a);

    const int LEVEL = 2;
    centerOfMass(model,data,LEVEL,computeSubtreeComs);
    return data.com[0];
  }

  template<typename Scalar, int Options, template<typename,int> class JointCollectionTpl>
  inline void centerOfMass(const ModelTpl<Scalar,Options,JointCollectionTpl> & model,
                           DataTpl<Scalar,Options,JointCollectionTpl> & data,
                           const int LEVEL,
                           const bool computeSubtreeComs)
  {
    assert(model.check(data) && "data is not consistent with model.");
    assert(LEVEL >= 0);

    typedef ModelTpl<Scalar,Options,JointCollectionTpl> Model;
    typedef DataTpl<Scalar,Options,JointCollectionTpl> Data;

    typedef typename Model::JointIndex JointIndex;

    typedef typename Data::SE3 SE3;
    typedef typename Data::Motion Motion;
    typedef typename Data::Inertia Inertia;

    const bool do_position = (LEVEL>=0);
    const bool do_velocity = (LEVEL>=1);
    const bool do_acceleration = (LEVEL>=2);

    data.mass[0] = 0;
    if(do_position)
      data.com[0].setZero();
    if(do_velocity)
      data.vcom[0].setZero();
    if(do_acceleration)
      data.acom[0].setZero();

    // Forward Step
    for(JointIndex i=1; i<(JointIndex)(model.njoints); ++i)
    {
      const typename Inertia::Scalar  & mass = model.inertias[i].mass();
      const typename SE3::Vector3 & lever = model.inertias[i].lever();

      const Motion & v = data.v[i];
      const Motion & a = data.a[i];

      data.mass[i] = mass;

      if(do_position)
        data.com[i].noalias()  = mass * lever;

      if(do_velocity)
        data.vcom[i].noalias() = mass * (v.angular().cross(lever) + v.linear());

      if(do_acceleration)
        data.acom[i].noalias() = mass * (a.angular().cross(lever) + a.linear())
                     + v.angular().cross(data.vcom[i]); // take into accound the coriolis part of the acceleration
    }

    // Backward Step
    for(JointIndex i=(JointIndex)(model.njoints-1); i>0; --i)
    {
      const JointIndex & parent = model.parents[i];
      const SE3 & liMi = data.liMi[i];

      data.mass[parent] += data.mass[i];

      if(do_position)
        data.com[parent] += (liMi.rotation()*data.com[i]
                         + data.mass[i] * liMi.translation());

      if(do_velocity)
        data.vcom[parent] += liMi.rotation()*data.vcom[i];

      if(do_acceleration)
        data.acom[parent] += liMi.rotation()*data.acom[i];

      if(computeSubtreeComs)
      {
        if(do_position)
          data.com[i] /= data.mass[i];
        if(do_velocity)
          data.vcom[i] /= data.mass[i];
        if(do_acceleration)
          data.acom[i] /= data.mass[i];
      }
    }

    if(do_position)
      data.com[0] /= data.mass[0];
    if(do_velocity)
      data.vcom[0] /= data.mass[0];
    if(do_acceleration)
      data.acom[0] /= data.mass[0];
  }

  template<typename Scalar, int Options, template<typename,int> class JointCollectionTpl>
  inline const typename DataTpl<Scalar,Options,JointCollectionTpl>::Vector3 &
  getComFromCrba(const ModelTpl<Scalar,Options,JointCollectionTpl> & model,
                 DataTpl<Scalar,Options,JointCollectionTpl> & data)
  {
    PINOCCHIO_UNUSED_VARIABLE(model);

    assert(model.check(data) && "data is not consistent with model.");
    return data.com[0] = data.liMi[1].act(data.Ycrb[1].lever());
  }

  /* --- JACOBIAN ---------------------------------------------------------- */
  /* --- JACOBIAN ---------------------------------------------------------- */
  /* --- JACOBIAN ---------------------------------------------------------- */

  template<typename Scalar, int Options, template<typename,int> class JointCollectionTpl, typename Matrix3x>
  struct JacobianCenterOfMassBackwardStep
  : public fusion::JointVisitorBase< JacobianCenterOfMassBackwardStep<Scalar,Options,JointCollectionTpl,Matrix3x> >
  {
    typedef ModelTpl<Scalar,Options,JointCollectionTpl> Model;
    typedef DataTpl<Scalar,Options,JointCollectionTpl> Data;

    typedef boost::fusion::vector<const Model &,
                                  Data &,
                                  const Eigen::MatrixBase<Matrix3x> &,
                                  const bool &
                                  > ArgsType;

    template<typename JointModel>
    static void algo(const JointModelBase<JointModel> & jmodel,
                     JointDataBase<typename JointModel::JointDataDerived> & jdata,
                     const Model & model,
                     Data & data,
                     const Eigen::MatrixBase<Matrix3x> & Jcom,
                     const bool & computeSubtreeComs)
    {
      const JointIndex & i      = (JointIndex) jmodel.id();
      const JointIndex & parent = model.parents[i];

      data.com[parent]  += data.com[i];
      data.mass[parent] += data.mass[i];

      typedef typename Data::Matrix6x Matrix6x;
      typedef typename SizeDepType<JointModel::NV>::template ColsReturn<Matrix6x>::Type ColBlock;
      
      Matrix3x & Jcom_ = PINOCCHIO_EIGEN_CONST_CAST(Matrix3x,Jcom);

      ColBlock Jcols = jmodel.jointCols(data.J);
      Jcols = data.oMi[i].act(jdata.S());
      
      for(Eigen::DenseIndex col_id = 0; col_id < jmodel.nv(); ++col_id)
      {
        jmodel.jointCols(Jcom_).col(col_id)
        = data.mass[i] * Jcols.col(col_id).template segment<3>(Motion::LINEAR)
        - data.com[i].cross(Jcols.col(col_id).template segment<3>(Motion::ANGULAR));
      }

      if(computeSubtreeComs)
        data.com[i] /= data.mass[i];
    }

  };

  template<typename Scalar, int Options, template<typename,int> class JointCollectionTpl, typename ConfigVectorType>
  PINOCCHIO_DEPRECATED
  inline const typename DataTpl<Scalar,Options,JointCollectionTpl>::Matrix3x &
  jacobianCenterOfMass(const ModelTpl<Scalar,Options,JointCollectionTpl> & model,
                       DataTpl<Scalar,Options,JointCollectionTpl> & data,
                       const Eigen::MatrixBase<ConfigVectorType> & q,
                       const bool computeSubtreeComs,
                       const bool updateKinematics)
  {
    if(updateKinematics)
      return jacobianCenterOfMass(model,data,q,computeSubtreeComs);
    else
      return jacobianCenterOfMass(model,data,computeSubtreeComs);
  }

  template<typename Scalar, int Options, template<typename,int> class JointCollectionTpl, typename ConfigVectorType>
  inline const typename DataTpl<Scalar,Options,JointCollectionTpl>::Matrix3x &
  jacobianCenterOfMass(const ModelTpl<Scalar,Options,JointCollectionTpl> & model,
                       DataTpl<Scalar,Options,JointCollectionTpl> & data,
                       const Eigen::MatrixBase<ConfigVectorType> & q,
                       const bool computeSubtreeComs)
  {
    forwardKinematics(model, data, q);
    return jacobianCenterOfMass(model, data, computeSubtreeComs);
  }

  template<typename Scalar, int Options, template<typename,int> class JointCollectionTpl>
  inline const typename DataTpl<Scalar,Options,JointCollectionTpl>::Matrix3x &
  jacobianCenterOfMass(const ModelTpl<Scalar,Options,JointCollectionTpl> & model,
                       DataTpl<Scalar,Options,JointCollectionTpl> & data,
                       const bool computeSubtreeComs)
  {
    assert(model.check(data) && "data is not consistent with model.");

    typedef ModelTpl<Scalar,Options,JointCollectionTpl> Model;
    typedef DataTpl<Scalar,Options,JointCollectionTpl> Data;

    typedef typename Model::JointIndex JointIndex;
    
    typedef typename Data::Matrix3x Matrix3x;
    typedef typename Data::SE3 SE3;
    typedef typename Data::Inertia Inertia;

    data.com[0].setZero();
    data.mass[0] = Scalar(0);

    for(JointIndex i=1; i<(JointIndex)(model.njoints); ++i)
    {
      const typename Inertia::Scalar & mass = model.inertias[i].mass();
      const typename SE3::Vector3 & lever = model.inertias[i].lever();

      data.mass[i] = mass;
      data.com[i].noalias() = mass*data.oMi[i].act(lever);
    }

    // Backward step
    typedef JacobianCenterOfMassBackwardStep<Scalar,Options,JointCollectionTpl,Matrix3x> Pass2;
    for(JointIndex i= (JointIndex) (model.njoints-1); i>0; --i)
    {
      Pass2::run(model.joints[i],data.joints[i],
                 typename Pass2::ArgsType(model,data,data.Jcom,computeSubtreeComs));
    }

    data.com[0] /= data.mass[0];
    data.Jcom /=  data.mass[0];

    return data.Jcom;
  }

  template<typename Scalar, int Options, template<typename,int> class JointCollectionTpl, typename ConfigVectorType, typename Matrix3xLike>
  inline void
  jacobianSubtreeCenterOfMass(const ModelTpl<Scalar,Options,JointCollectionTpl> & model,
                              DataTpl<Scalar,Options,JointCollectionTpl> & data,
                              const Eigen::MatrixBase<ConfigVectorType> & q,
                              const JointIndex & rootSubtreeId,
                              const Eigen::MatrixBase<Matrix3xLike> & JacobianOut)
  {
    forwardKinematics(model, data, q);
    jacobianSubtreeCenterOfMass(model, data, rootSubtreeId, JacobianOut);
  }

  template<typename Scalar, int Options, template<typename,int> class JointCollectionTpl, typename Matrix3xLike>
  inline void
  jacobianSubtreeCenterOfMass(const ModelTpl<Scalar,Options,JointCollectionTpl> & model,
                              DataTpl<Scalar,Options,JointCollectionTpl> & data,
                              const JointIndex & rootSubtreeId,
                              const Eigen::MatrixBase<Matrix3xLike> & JacobianOut)
  {
    typedef typename DataTpl<Scalar,Options,JointCollectionTpl>::Matrix6x Matrix6x;
    assert(model.check(data) && "data is not consistent with model.");
    assert(((int)rootSubtreeId < model.njoints) && "Invalid joint id.");

    Eigen::MatrixBase<Matrix3xLike> & Jacobian = const_cast<Eigen::MatrixBase<Matrix3xLike>& >(JacobianOut);

    if (rootSubtreeId == 0)
    {
      // Joint 0 is universe: this means that the user is asking for the entier model CoM jacobian.
      Jacobian = jacobianCenterOfMass(model, data);
    }
    else
    {
      // Iterates over all the joints of the subtree.
      for(size_t i = 0; i < model.subtrees[rootSubtreeId].size(); i++)
      {
        const JointIndex id = model.subtrees[rootSubtreeId][i];
        // Get joint jacobian in world.
        Matrix6x jointJacobian = Matrix6x::Zero(6, model.nv);
        getJointJacobian(model, data, id, LOCAL, jointJacobian);

        // Add jacobian of selected body com.
        Jacobian += model.inertias[id].mass() * data.oMi[id].rotation() * (
          jointJacobian.template topRows<3>() - skew(model.inertias[id].lever()) * jointJacobian.template bottomRows<3>());
      }
      Jacobian /= data.mass[rootSubtreeId];
    }
  }

  template<typename Scalar, int Options, template<typename,int> class JointCollectionTpl>
  inline const typename DataTpl<Scalar,Options,JointCollectionTpl>::Matrix3x &
  getJacobianComFromCrba(const ModelTpl<Scalar,Options,JointCollectionTpl> & model,
                         DataTpl<Scalar,Options,JointCollectionTpl> & data)
  {
    PINOCCHIO_UNUSED_VARIABLE(model);
    assert(model.check(data) && "data is not consistent with model.");

    typedef DataTpl<Scalar,Options,JointCollectionTpl> Data;
    typedef typename Data::SE3 SE3;

    const SE3 & oM1 = data.liMi[1];

    // Extract the total mass of the system.
    data.mass[0] = data.M(0,0);

    // As the 6 first rows of M*a are a wrench, we just need to multiply by the
    // relative rotation between the first joint and the world
    const typename SE3::Matrix3 oR1_over_m (oM1.rotation() / data.M(0,0));

    // I don't know why, but the colwise multiplication is much more faster
    // than the direct Eigen multiplication
    for(long k=0; k<model.nv;++k)
      data.Jcom.col(k).noalias() = oR1_over_m * data.M.template topRows<3>().col(k);

    return data.Jcom;
  }

} // namespace pinocchio

/// @endcond

#endif // ifndef __pinocchio_center_of_mass_hxx__
