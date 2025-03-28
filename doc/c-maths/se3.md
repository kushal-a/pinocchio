# CheatSheet: SE(3) operations

<div class="center">

$$
\newcommand{\BIN}{\begin{bmatrix}}
\newcommand{\BOUT}{\end{bmatrix}}
\newcommand{\calR}{\mathcal{R}}
\newcommand{\calE}{\mathcal{E}}
\newcommand{\repr}{\cong}
\newcommand{\dpartial}[2]{\frac{\partial{#1}}{\partial{#2}}}
\newcommand{\ddpartial}[2]{\frac{\partial^2{#1}}{\partial{#2}^2}}

\newcommand{\aRb}{\ {}^{A}R_B}
\newcommand{\aMb}{\ {}^{A}M_B}
\newcommand{\amb}{\ {}^{A}m_B}
\newcommand{\apb}{{\ {}^{A}{AB}{}}}
\newcommand{\aXb}{\ {}^{A}X_B}

\newcommand{\bRa}{\ {}^{B}R_A}
\newcommand{\bMa}{\ {}^{B}M_A}
\newcommand{\bma}{\ {}^{B}m_A}
\newcommand{\bpa}{\ {}^{B}{BA}{}}
\newcommand{\bXa}{\ {}^{B}X_A}

\newcommand{\ap}{\ {}^{A}p}
\newcommand{\bp}{\ {}^{B}p}

\newcommand{\afs}{\ {}^{A}\phi}
\newcommand{\bfs}{\ {}^{B}\phi}
\newcommand{\af}{\ {}^{A}f}
\renewcommand{\bf}{\ {}^{B}f}
\newcommand{\an}{\ {}^{A}\tau}
\newcommand{\bn}{\ {}^{B}\tau}

\newcommand{\avs}{\ {}^{A}\nu}
\newcommand{\bvs}{\ {}^{B}\nu}
\newcommand{\w}{\omega}
\newcommand{\av}{\ {}^{A}v}
\newcommand{\bv}{\ {}^{B}v}
\newcommand{\aw}{\ {}^{A}\w}
\newcommand{\bw}{\ {}^{B}\w}

\newcommand{\aI}{\ {}^{A}I}
\newcommand{\bI}{\ {}^{B}I}
\newcommand{\cI}{\ {}^{C}I}
\newcommand{\aY}{\ {}^{A}Y}
\newcommand{\bY}{\ {}^{B}Y}
\newcommand{\cY}{\ {}^{c}Y}
\newcommand{\aXc}{\ {}^{A}X_C}
\newcommand{\aMc}{\ {}^{A}M_C}
\newcommand{\aRc}{\ {}^{A}R_C}
\newcommand{\apc}{\ {}^{A}{AC}{}}
\newcommand{\bXc}{\ {}^{B}X_C}
\newcommand{\bRc}{\ {}^{B}R_C}
\newcommand{\bMc}{\ {}^{B}M_C}
\newcommand{\bpc}{\ {}^{B}{BC}{}}
$$

## Rigid transformation

$m : p \in \calE(3) \rightarrow m(p) \in E(3)$

Transformation from B to A:

$\amb : \bp \in \calR^3 \repr \calE(3) \ \rightarrow\ \ap = \amb(\bp) = \aMb\ \bp$

$ \ap = \aRb \bp +  \apb$

$\aMb = \BIN \aRb & \apb \\ 0 & 1 \BOUT $

Transformation from A to B:

$\bp = \aRb^T \ap + \bpa, \quad\textrm{with }\bpa = - \aRb^T \apb$

$\bMa = \BIN \aRb^T & - \aRb^T \apb \\ 0 & 1 \BOUT $

For Featherstone, $E = \bRa =\aRb^T$ and $r = \apb$. Then:

$\bMa = \BIN \bRa & -\bRa \apb \\ 0 & 1 \BOUT = \BIN E & -E r \\ 0 & 1 \BOUT $

$\aMb = \BIN \bRa^T & \apb \\ 0 & 1 \BOUT = \BIN E^T & r \\ 0 & 1 \BOUT $

## Composition

$ \aMb \bMc = \BIN \aRb \bRc & \apb +  \aRb \bpc \\ 0 & 1 \BOUT $

$ \aMb^{-1} \aMc = \BIN \aRb^T \aRc & \aRb^T (\apc - \apb) \\ 0 & 1 \BOUT $



## Motion Application

$\avs = \BIN \av \\ \aw \BOUT$

$\bvs = \bXa\avs$

$ \aXb =  \BIN \aRb & \apb_\times \aRb \\ 0 & \aRb \BOUT $

$ \aXb^{-1} = \bXa =  \BIN \aRb^T & -\aRb^T \apb_\times \\ 0 & \aRb^T \BOUT $

For Featherstone, $E = \bRa =\aRb^T$ and $r = \apb$. Then:

$ \bXa = \BIN \bRa & - \bRa \apb_\times \\ 0 & \bRa \BOUT = \BIN E & -E r_\times \\ 0 & E \BOUT$

$ \aXb = \BIN \bRa^T & \apb_\times \bRa^T \\ 0 & \bRa^T \BOUT = \BIN E^T & r_\times E^T \\ 0 & E^T \BOUT$

## Force Application

$\afs = \BIN \af \\ \an \BOUT$

$\bfs = \bXa^* \afs$

For any $\phi,\nu$, $\phi\dot\nu = \afs^T \avs = \bfs^T \bvs$ and then:

$\aXb^* = \aXb^{-T} = \BIN \aRb & 0 \\ \apb_\times \aRb & \aRb \BOUT$

(because $\apb_\times^T = - \apb_\times$).

$\aXb^{-*} = \bXa^* = \BIN \aRb^T & 0 \\ -\aRb^T \apb_\times  & \aRb^T \BOUT$

For Featherstone, $E = \bRa =\aRb^T$ and $r = \apb$. Then:

$\bXa^* = \BIN \bRa & 0 \\ -\bRa \apb_\times & \bRa \BOUT = \BIN E & 0 \\ - E r_\times & E \BOUT $

$\aXb^* = \BIN \bRa^T & 0 \\  \apb_\times \bRa^T & \bRa^T \BOUT = \BIN E^T & 0 \\ r_\times E^T & E^T \BOUT $

## Inertia
### Inertia application

$\aY: \avs \rightarrow \afs = \aY \avs$

Coordinate transform:

$\bY = \bXa^{*} \aY \bXa^{-1}$

since:

$\bfs = \bXa^* \bfs = \bXa^* \aI \aXb \bvs$

Cannonical form. The inertia about the center of mass $c$ is:

$\cY = \BIN m & 0 \\ 0 & \cI \BOUT$

Expressed in any non-centered coordinate system $A$:
$\aY = \aXc^* \cI \aXc^{-1} = \BIN m & m\ ^AAC_\times^T \\  m\ ^AAC_\times & \aI + m \apc_\times \apc\times^T \BOUT
$

Changing the coordinates system from $B$ to $A$:

$\aY = \aXb^* \bXc^* \cI \bXc^{-1} \aXb^{-1} $
$ = \BIN m & m [\apb + \aRb \bpc]_\times^T \\  m [\apb + \aRb \bpc]_\times & \aRb \bI \aRb^T - m [\apb + \aRb
\bpc]_\times^2 \BOUT$

Representing the spatial inertia in $B$ by the triplet $(m,\bpc,\bI)$, the expression in $A$ is:

$ \amb: \bY = (m,\bpc,\bI) \rightarrow \aY = (m,\apb+\aRb \bpc,\aRb \bI \aRb^T)$

Similarly, the inverse action is:

$ \amb^{-1}: \aY \rightarrow \bY = (m,\aRb^T(^AAC-\apb),\aRb^T\aI \aRb) $

Motion-to-force map:

$ Y \nu = \BIN m & mc_\times^T \\ mc_\times & I+mc_\times c_\times^T \BOUT \BIN v \\ \omega \BOUT
 = \BIN m v - mc \times \omega \\ mc \times v + I \omega - mc \times ( c\times \omega) \BOUT$

Nota: the square of the cross product is:
$\BIN x\\y\\z\BOUT_ \times^2 = \BIN 0&-z&y \\ z&0&-x \\ -y&x&0 \BOUT^2 = \BIN -y^2-z^2&xy&xz \\ xy&-x^2-z^2&yz \\
xz&yz&-x^2-y^2 \BOUT$
There is no computational interest in using it.

### Inertia addition

$ Y_p = \BIN m_p &  m_p  p_\times^T \\ m_p p_\times &  I_p + m_p  p_\times p_\times^T \BOUT$

$ Y_q = \BIN m_q &  m_q  q_\times^T \\ m_q q_\times &  I_q + m_q  q_\times q_\times^T \BOUT$




## Cross products

Motion-motion product:

$\nu_1 \times \nu_2 = \BIN v_1\\\omega_1\BOUT \times \BIN v_2\\\omega_2\BOUT = \BIN  v_1 \times \omega_2 + \omega_1 \times v_2 \\ \omega_1 \times \omega_2 \BOUT $

Motion-force product:

$\nu \times \phi =  \BIN v\\\omega\BOUT \times \BIN f\\ \tau \BOUT = \BIN  \omega \times f \\ \omega \times \tau + v \times f \BOUT $

A special form of the motion-force product is often used:

$$\begin{align*}\nu \times (Y \nu) &= \nu \times \BIN mv - mc\times \omega \\ mc\times v + I \omega - mc\times(c\times \omega) \BOUT \\&= \BIN m \omega\times v - \omega\times(mc\times \omega) \\ \omega \times ( mc \times v) + \omega \times (I\omega) -\omega \times(c \times( mc\times \omega)) -v\times(mc \times \omega)\BOUT\end{align*}$$

Setting $\beta=mc \times \omega$, this product can be written:

$\nu \times (Y \nu) = \BIN \omega \times (m v - \beta) \\ \omega \times( c \times (mv-\beta)+I\omega) - v \times \beta \BOUT$

This last form cost five $\times$, four $+$ and one $3\times3$ matrix-vector multiplication.

## Joint

We denote by $1$ the coordinate system attached to the parent (predecessor) body at the joint input, and by $2$
the coordinate system attached to the (child) successor body at the joint output. We neglect the possible time
variation of the joint model (ie the bias velocity $\sigma = \nu(q,0)$ is null).

The joint geometry is expressed by the rigid transformation from the input to the ouput, parametrized by the joint
coordinate system $q \in \mathcal{Q}$:

$ ^2m_1 \repr \ ^2M_1(q)$

The joint velocity (i.e. the velocity of the child wrt. the parent in the child coordinate system) is:

$^2\nu_{12} = \nu_J(q,v_q) = \ ^2S(q) v_q $

where $^2S$ is the joint Jacobian (or constraint matrix) that define the motion subspace allowed by the joint, and
$v_q$ is the joint coordinate velocity (i.e. an element of the Lie algebra associated with the joint coordinate
manifold), which would be $v_q=\dot q$ when $\dot q$ exists.

The joint acceleration is:

$^2\alpha_{12} = S \dot v_q + c_J + \ ^2\nu_{1} \times \ ^2\nu_{12}$

where $c_J = \sum_{i=1}^{n_q} \dpartial{S}{q_i} \dot q_i$ (null in the usual cases) and $^2\nu_{1}$ is the
velocity of the parent body with respect to an absolute (Galilean) coordinate system

NB: The abosulte velocity
$\nu_{1}$ is also the relative velocity wrt. the Galilean coordinate system $\Omega$. The exhaustive notation
should be $\nu_{\Omega1}$ but $\nu_1$ is prefered for simplicity.

The joint calculations take as input the joint position $q$ and velocity $v_q$ and should output $^2M_1$,
$^2\nu_{12}$ and $^2c$ (this last vector being often a trivial $0_6$ vector). In addition, the joint model
should store the position of the joint input in the central coordinate system of the previous joint $^0m_1$ which is a constant value.

The joint integrator computes the exponential map associated with the joint manifold. The function inputs are the
initial position $q_0$, the velocity $v_q$  and the length of the integration interval $t$. It computes $q_t$ as:

$ q_t = q_0 + \int_0^t v_q dt$

For the simple vectorial case where $v_q=\dot q$, we have $q_t=q_0 + t v_q$. Written in the more general case of a Lie group, we have $q_t = q_0 exp(t v_q)$ where $exp$ denotes the exponential map (i.e. integration of a constant vector field from the Lie algebra into the Lie group). This integration only considers first order explicit Euler. More general integrators (e.g. Runge-Kutta in Lie groups) remains to be written. Adequate references are welcome.

## RNEA

### Initialization
$^0\nu_0 = 0 ; \ ^0\alpha_0 = -g$

In the following, the coordinate system $i$ is attached to the output of the joint (child body), while $lambda(i)$ is the central coordinate system attached to the parent joint. The coordinated system associated with the joint input is denoted by $i_0$. The constant rigid transformation from $\lambda(i)$ to the joint input is then $^{\lambda(i)}M_{i_0}$.


### Forward loop
For each joint $i$, update the joint calculation $\mathbf j_i$.calc($q,v_q$). This compute $\mathbf{j}.M = \ ^{\lambda(i)}M_{i_0}(q)$, $\mathbf{j}.\nu = \ ^i\nu_{{\lambda(i)}i}(q,v_q)$, $\mathbf{j}.S = \ ^iS(q)$  and $\mathbf{j}.c = \sum_{k=1}^{n_q} \dpartial{^iS}{q_k} \dot q_k$. Attached to the joint is also its placement in body $\lambda(i)$ denoted by $\mathbf{j}.M_0 =\ ^{\lambda(i)}M_{i_0}$. Then:

$^{\lambda(i)}M_i = \mathbf{j}.M_0 \ \mathbf{j}.M $

$^0M_i = \ ^0M_{\lambda(i)} \ ^{\lambda(i)}M_i$

$^i\nu_{i}= \ ^{\lambda(i)}X_i^{-1} \ ^{\lambda(i)}\nu_{{\lambda(i)}} + \mathbf{j}.\nu$

$^i\alpha_{i}= \ ^{\lambda(i)}X_i^{-1} \  ^{\lambda(i)}\alpha_{{\lambda(i)}} + \mathbf{j}.S \dot v_q + \mathbf{j}.c + \ ^i\nu_{i} \times  \mathbf{j}.\nu$

$^i\phi_i= \ ^iY_i \ ^i\alpha_i + \ ^i\nu_i \times \ ^iY_i \ ^i\nu_i - \ ^0X_i^{-*}\ ^0\phi_i^{ext}$

### Backward loop
For each joint $i$ from leaf to root, do:

$\tau_i = \mathbf{j}.S^T \ ^i\phi_i$

$^{\lambda(i)}\phi_{\lambda(i)} \ +\!\!= \ ^{\lambda(i)}X_i^{*} \ ^i\phi_i$

### Nota
It is more efficient to apply $X^{-1}$ than $X$. Similarly, it is more efficient to apply $X^{-*}$ than $X^*$. Therefore, it is better to store the transformations $^{\lambda(i)}m_i$ and $^0m_i$ than $^im_{\lambda(i)}$ and $^im_0$.

</div>
