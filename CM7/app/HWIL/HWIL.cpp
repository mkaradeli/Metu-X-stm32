/*

//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: HWIL.cpp
//
// Code generated for Simulink model 'HWIL'.
//
// Model version                  : 1.32
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Sun Aug 30 01:45:16 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#include "HWIL.h"
#include "rtwtypes.h"
#include "limits"
#include "cmath"

// private model entry point functions
extern void HWIL_derivatives();
//extern "C"
//{
//  real_T rtNaN { -std::numeric_limits<real_T>::quiet_NaN() };
//
//  real_T rtInf { std::numeric_limits<real_T>::infinity() };
//
//  real_T rtMinusInf { -std::numeric_limits<real_T>::infinity() };
//
//  real32_T rtNaNF { -std::numeric_limits<real32_T>::quiet_NaN() };
//
//  real32_T rtInfF { std::numeric_limits<real32_T>::infinity() };
//
//  real32_T rtMinusInfF { -std::numeric_limits<real32_T>::infinity() };
//}

extern "C"
{
  // Return rtInf needed by the generated code.
  static real_T rtGetInf(void)
  {
    return rtInf;
  }

  // Get rtInfF needed by the generated code.
  static real32_T rtGetInfF(void)
  {
    return rtInfF;
  }

  // Return rtMinusInf needed by the generated code.
  static real_T rtGetMinusInf(void)
  {
    return rtMinusInf;
  }

  // Return rtMinusInfF needed by the generated code.
  static real32_T rtGetMinusInfF(void)
  {
    return rtMinusInfF;
  }
}

extern "C"
{
  // Test if value is infinite
  static boolean_T rtIsInf(real_T value)
  {
    return std::isinf(value);
  }

  // Test if single-precision value is infinite
  static boolean_T rtIsInfF(real32_T value)
  {
    return std::isinf(value);
  }

  // Test if value is not a number
  static boolean_T rtIsNaN(real_T value)
  {
    return std::isnan(value);
  }

  // Test if single-precision value is not a number
  static boolean_T rtIsNaNF(real32_T value)
  {
    return std::isnan(value);
  }
}

//
// This function updates continuous states using the ODE3 fixed-step
// solver algorithm
//
void HWIL::rt_ertODEUpdateContinuousStates(RTWSolverInfo *si )
{
  // Solver Matrices
  static const real_T rt_ODE3_A[3]{
    1.0/2.0, 3.0/4.0, 1.0
  };

  static const real_T rt_ODE3_B[3][3]{
    { 1.0/2.0, 0.0, 0.0 },

    { 0.0, 3.0/4.0, 0.0 },

    { 2.0/9.0, 1.0/3.0, 4.0/9.0 }
  };

  time_T t { rtsiGetT(si) };

  time_T tnew { rtsiGetSolverStopTime(si) };

  time_T h { rtsiGetStepSize(si) };

  real_T *x { rtsiGetContStates(si) };

  ODE3_IntgData *id { static_cast<ODE3_IntgData *>(rtsiGetSolverData(si)) };

  real_T *y { id->y };

  real_T *f0 { id->f[0] };

  real_T *f1 { id->f[1] };

  real_T *f2 { id->f[2] };

  real_T hB[3];
  int_T i;
  int_T nXc { 2 };

  rtsiSetSimTimeStep(si,MINOR_TIME_STEP);

  // Save the state values at time t in y, we'll use x as ynew.
  (void) std::memcpy(y, x,
                     static_cast<uint_T>(nXc)*sizeof(real_T));

  // Assumes that rtsiSetT and ModelOutputs are up-to-date
  // f0 = f(t,y)
  rtsiSetdX(si, f0);
  HWIL_derivatives();

  // f(:,2) = feval(odefile, t + hA(1), y + f*hB(:,1), args(:)(*));
  hB[0] = h * rt_ODE3_B[0][0];
  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[0]);
  rtsiSetdX(si, f1);
  this->step();
  HWIL_derivatives();

  // f(:,3) = feval(odefile, t + hA(2), y + f*hB(:,2), args(:)(*));
  for (i = 0; i <= 1; i++) {
    hB[i] = h * rt_ODE3_B[1][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[1]);
  rtsiSetdX(si, f2);
  this->step();
  HWIL_derivatives();

  // tnew = t + hA(3);
  // ynew = y + f*hB(:,3);
  for (i = 0; i <= 2; i++) {
    hB[i] = h * rt_ODE3_B[2][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1] + f2[i]*hB[2]);
  }

  rtsiSetT(si, tnew);
  rtsiSetSimTimeStep(si,MAJOR_TIME_STEP);
}

// Model step function
void HWIL::step()
{
  real_T rtb_CastToDouble1;
  real32_T rtb_Bias;
  real32_T tmp;
  real32_T tmp_0;
  real32_T tmp_1;
  real32_T tmp_2;
  if ((&rtM)->isMajorTimeStep()) {
    // set solver stop time
    rtsiSetSolverStopTime(&(&rtM)->solverInfo,(((&rtM)->Timing.clockTick0+1)*
      (&rtM)->Timing.stepSize0));
  }                                    // end MajorTimeStep

  // Update absolute time of base rate at minor time step
  if ((&rtM)->isMinorTimeStep()) {
    (&rtM)->Timing.t[0] = rtsiGetT(&(&rtM)->solverInfo);
  }

  // Saturate: '<S1>/Saturation5' incorporates:
  //   Inport: '<Root>/P_nozzle_manifold'

  if (rtU.P_nozzle_manifold <= 0.0F) {
    tmp = 0.0F;
  } else {
    tmp = rtU.P_nozzle_manifold;
  }

  // Bias: '<S1>/Bias' incorporates:
  //   Gain: '<S1>/Gain'
  //   Gain: '<S1>/Gain2'
  //   Saturate: '<S1>/Saturation5'

  rtb_Bias = 0.069F * tmp * 0.0095F + 34.0F;

  // Outport: '<Root>/mass'
  rtY.mass = rtb_Bias;

  // Integrator: '<S1>/Integrator1' incorporates:
  //   Inport: '<Root>/X0'

  // Limited  Integrator
  if (rtDW.Integrator1_DWORK1) {
    rtX.Integrator1_CSTATE = rtU.X0;
  }

  if ((!(rtX.Integrator1_CSTATE >= (rtInf))) && (rtX.Integrator1_CSTATE <= 0.0))
  {
    rtX.Integrator1_CSTATE = 0.0;
  }

  // Switch: '<S1>/Switch' incorporates:
  //   Constant: '<S1>/Constant'
  //   Inport: '<Root>/enable_spring'

  if (rtU.enable_spring) {
    // Gain: '<S1>/Gain4' incorporates:
    //   Integrator: '<S1>/Integrator1'
    //   Sum: '<S1>/Sum2'

    rtb_CastToDouble1 = (rtX.Integrator1_CSTATE - 4.613664285714286) * -140.0;

    // Saturate: '<S1>/Saturation'
    if (rtb_CastToDouble1 > 623.35000000000014) {
      rtb_CastToDouble1 = 623.35000000000014;
    } else if (rtb_CastToDouble1 < 0.0) {
      rtb_CastToDouble1 = 0.0;
    }

    // End of Saturate: '<S1>/Saturation'
  } else {
    rtb_CastToDouble1 = 0.0;
  }

  // End of Switch: '<S1>/Switch'

  // Saturate: '<S1>/Saturation4' incorporates:
  //   Inport: '<Root>/F_nozzle_1'

  if (rtU.F_nozzle_1 <= 0.0F) {
    tmp = 0.0F;
  } else {
    tmp = rtU.F_nozzle_1;
  }

  // Saturate: '<S1>/Saturation3' incorporates:
  //   Inport: '<Root>/F_nozzle_2'

  if (rtU.F_nozzle_2 <= 0.0F) {
    tmp_0 = 0.0F;
  } else {
    tmp_0 = rtU.F_nozzle_2;
  }

  // Saturate: '<S1>/Saturation2' incorporates:
  //   Inport: '<Root>/F_nozzle_3'

  if (rtU.F_nozzle_3 <= 0.0F) {
    tmp_1 = 0.0F;
  } else {
    tmp_1 = rtU.F_nozzle_3;
  }

  // Saturate: '<S1>/Saturation1' incorporates:
  //   Inport: '<Root>/F_nozzle_4'

  if (rtU.F_nozzle_4 <= 0.0F) {
    tmp_2 = 0.0F;
  } else {
    tmp_2 = rtU.F_nozzle_4;
  }

  // Product: '<S1>/Divide' incorporates:
  //   DataTypeConversion: '<S1>/Cast To Double1'
  //   Gain: '<S1>/Gain1'
  //   Saturate: '<S1>/Saturation1'
  //   Saturate: '<S1>/Saturation2'
  //   Saturate: '<S1>/Saturation3'
  //   Saturate: '<S1>/Saturation4'
  //   Sum: '<S1>/Sum'
  //   Sum: '<S1>/Sum1'

  rtDW.Divide = (((((tmp + tmp_0) + tmp_1) + tmp_2) + rtb_CastToDouble1) +
                 -9.81F * rtb_Bias) / rtb_Bias;

  // Outport: '<Root>/acceleration' incorporates:
  //   Constant: '<S1>/Constant4'

  rtY.acceleration[0] = 0.0;
  rtY.acceleration[1] = 0.0;
  rtY.acceleration[2] = rtDW.Divide;

  // Outport: '<Root>/position' incorporates:
  //   Integrator: '<S1>/Integrator1'

  rtY.position = rtX.Integrator1_CSTATE;

  // Outport: '<Root>/lidar_height' incorporates:
  //   Integrator: '<S1>/Integrator1'

  rtY.lidar_height = rtX.Integrator1_CSTATE;

  // Integrator: '<S1>/Integrator' incorporates:
  //   Inport: '<Root>/V0'

  if (rtDW.Integrator_DWORK1) {
    rtX.Integrator_CSTATE = rtU.V0;
  }

  // Integrator: '<S1>/Integrator'
  rtDW.Integrator = rtX.Integrator_CSTATE;

  // Outport: '<Root>/velocity'
  rtY.velocity = rtDW.Integrator;
  if ((&rtM)->isMajorTimeStep()) {
    // Update for Integrator: '<S1>/Integrator1'
    rtDW.Integrator1_DWORK1 = false;

    // Update for Integrator: '<S1>/Integrator'
    rtDW.Integrator_DWORK1 = false;
  }                                    // end MajorTimeStep

  if ((&rtM)->isMajorTimeStep()) {
    rt_ertODEUpdateContinuousStates(&(&rtM)->solverInfo);

    // Update absolute time for base rate
    // The "clockTick0" counts the number of times the code of this task has
    //  been executed. The absolute time is the multiplication of "clockTick0"
    //  and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
    //  overflow during the application lifespan selected.

    ++(&rtM)->Timing.clockTick0;
    (&rtM)->Timing.t[0] = rtsiGetSolverStopTime(&(&rtM)->solverInfo);

    {
      // Update absolute timer for sample time: [0.001s, 0.0s]
      // The "clockTick1" counts the number of times the code of this task has
      //  been executed. The resolution of this integer timer is 0.001, which is the step size
      //  of the task. Size of "clockTick1" ensures timer will not overflow during the
      //  application lifespan selected.

      (&rtM)->Timing.clockTick1++;
    }
  }                                    // end MajorTimeStep
}

// Derivatives for root system: '<Root>'
void HWIL::HWIL_derivatives()
{
  HWIL::XDot *_rtXdot;
  boolean_T lsat;
  boolean_T usat;
  _rtXdot = ((XDot *) (&rtM)->derivs);

  // Derivatives for Integrator: '<S1>/Integrator1'
  lsat = (rtX.Integrator1_CSTATE <= 0.0);
  usat = (rtX.Integrator1_CSTATE >= (rtInf));
  if (((!lsat) && (!usat)) || (lsat && (rtDW.Integrator > 0.0)) || (usat &&
       (rtDW.Integrator < 0.0))) {
    _rtXdot->Integrator1_CSTATE = rtDW.Integrator;
  } else {
    // in saturation
    _rtXdot->Integrator1_CSTATE = 0.0;
  }

  // End of Derivatives for Integrator: '<S1>/Integrator1'

  // Derivatives for Integrator: '<S1>/Integrator'
  _rtXdot->Integrator_CSTATE = rtDW.Divide;
}

// Model initialize function
void HWIL::initialize()
{
  // Registration code
  {
    // Setup solver object
    rtsiSetSimTimeStepPtr(&(&rtM)->solverInfo, &(&rtM)->Timing.simTimeStep);
    rtsiSetTPtr(&(&rtM)->solverInfo, (&rtM)->getTPtrPtr());
    rtsiSetStepSizePtr(&(&rtM)->solverInfo, &(&rtM)->Timing.stepSize0);
    rtsiSetdXPtr(&(&rtM)->solverInfo, &(&rtM)->derivs);
    rtsiSetContStatesPtr(&(&rtM)->solverInfo, (real_T **) &(&rtM)->contStates);
    rtsiSetNumContStatesPtr(&(&rtM)->solverInfo, &(&rtM)->Sizes.numContStates);
    rtsiSetNumPeriodicContStatesPtr(&(&rtM)->solverInfo, &(&rtM)
      ->Sizes.numPeriodicContStates);
    rtsiSetPeriodicContStateIndicesPtr(&(&rtM)->solverInfo, &(&rtM)
      ->periodicContStateIndices);
    rtsiSetPeriodicContStateRangesPtr(&(&rtM)->solverInfo, &(&rtM)
      ->periodicContStateRanges);
    rtsiSetContStateDisabledPtr(&(&rtM)->solverInfo, (boolean_T**) &(&rtM)
      ->contStateDisabled);
    rtsiSetErrorStatusPtr(&(&rtM)->solverInfo, (&rtM)->getErrorStatusPtr());
    rtsiSetRTModelPtr(&(&rtM)->solverInfo, (&rtM));
  }

  rtsiSetSimTimeStep(&(&rtM)->solverInfo, MAJOR_TIME_STEP);
  rtsiSetIsMinorTimeStepWithModeChange(&(&rtM)->solverInfo, false);
  rtsiSetIsContModeFrozen(&(&rtM)->solverInfo, false);
  (&rtM)->intgData.y = (&rtM)->odeY;
  (&rtM)->intgData.f[0] = (&rtM)->odeF[0];
  (&rtM)->intgData.f[1] = (&rtM)->odeF[1];
  (&rtM)->intgData.f[2] = (&rtM)->odeF[2];
  (&rtM)->contStates = ((X *) &rtX);
  (&rtM)->contStateDisabled = ((XDis *) &rtXDis);
  (&rtM)->Timing.tStart = (0.0);
  rtsiSetSolverData(&(&rtM)->solverInfo, static_cast<void *>(&(&rtM)->intgData));
  rtsiSetSolverName(&(&rtM)->solverInfo,"ode3");
  (&rtM)->setTPtr(&(&rtM)->Timing.tArray[0]);
  (&rtM)->Timing.stepSize0 = 0.001;
  (&rtM)->setFirstInitCond(1);

  // InitializeConditions for Integrator: '<S1>/Integrator1' incorporates:
  //   Integrator: '<S1>/Integrator'

  if ((&rtM)->isFirstInitCond()) {
    rtX.Integrator1_CSTATE = 0.0;
    rtX.Integrator_CSTATE = 0.0;
  }

  rtDW.Integrator1_DWORK1 = true;

  // End of InitializeConditions for Integrator: '<S1>/Integrator1'

  // InitializeConditions for Integrator: '<S1>/Integrator'
  rtDW.Integrator_DWORK1 = true;

  // ConstCode for Outport: '<Root>/quaternion_sim' incorporates:
  //   Constant: '<S1>/Constant2'

  rtY.quaternion_sim[0] = 0.0F;
  rtY.quaternion_sim[1] = 0.0F;
  rtY.quaternion_sim[2] = 0.0F;
  rtY.quaternion_sim[3] = 1.0F;

  // set "at time zero" to false
  if ((&rtM)->isFirstInitCond()) {
    (&rtM)->setFirstInitCond(0);
  }
}

time_T** HWIL::RT_MODEL::getTPtrPtr()
{
  return &(Timing.t);
}

time_T* HWIL::RT_MODEL::getTPtr() const
{
  return (Timing.t);
}

void HWIL::RT_MODEL::setTPtr(time_T* aTPtr)
{
  (Timing.t = aTPtr);
}

boolean_T HWIL::RT_MODEL::getStopRequested() const
{
  return (Timing.stopRequestedFlag);
}

void HWIL::RT_MODEL::setStopRequested(boolean_T aStopRequested)
{
  (Timing.stopRequestedFlag = aStopRequested);
}

boolean_T HWIL::RT_MODEL::isMinorTimeStep() const
{
  return ((Timing.simTimeStep) == MINOR_TIME_STEP);
}

void HWIL::RT_MODEL::setFirstInitCond(boolean_T aFirstInitCond)
{
  (Timing.firstInitCondFlag = aFirstInitCond);
}

boolean_T HWIL::RT_MODEL::isFirstInitCond() const
{
  return (Timing.firstInitCondFlag);
}

boolean_T HWIL::RT_MODEL::isMajorTimeStep() const
{
  return ((Timing.simTimeStep) == MAJOR_TIME_STEP);
}

boolean_T* HWIL::RT_MODEL::getStopRequestedPtr()
{
  return (&(Timing.stopRequestedFlag));
}

const char_T** HWIL::RT_MODEL::getErrorStatusPtr()
{
  return &errorStatus;
}

time_T HWIL::RT_MODEL::getTStart() const
{
  return (Timing.tStart);
}

const char_T* HWIL::RT_MODEL::getErrorStatus() const
{
  return (errorStatus);
}

void HWIL::RT_MODEL::setErrorStatus(const char_T* const aErrorStatus)
{
  (errorStatus = aErrorStatus);
}

// Constructor
HWIL::HWIL() :
  rtU(),
  rtY(),
  rtDW(),
  rtX(),
  rtXDis(),
  rtM()
{
  // Currently there is no constructor body generated.
}

// Destructor
// Currently there is no destructor body generated.
HWIL::~HWIL() = default;

// Real-Time Model get method
HWIL::RT_MODEL * HWIL::getRTM()
{
  return (&rtM);
}

//
// File trailer for generated code.
//
// [EOF]
//
*/

//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: HWIL.cpp
//
// Code generated for Simulink model 'HWIL'.
//
// Model version                  : 1.32
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Sun Aug 30 01:45:16 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#include "HWIL.h"
#include "rtwtypes.h"
#include "limits"
#include "cmath"


extern "C"
{
  // Return rtInf needed by the generated code.
  static real_T rtGetInf(void)
  {
    return rtInf;
  }

  // Get rtInfF needed by the generated code.
  static real32_T rtGetInfF(void)
  {
    return rtInfF;
  }

  // Return rtMinusInf needed by the generated code.
  static real_T rtGetMinusInf(void)
  {
    return rtMinusInf;
  }

  // Return rtMinusInfF needed by the generated code.
  static real32_T rtGetMinusInfF(void)
  {
    return rtMinusInfF;
  }
}

extern "C"
{
  // Test if value is infinite
  static boolean_T rtIsInf(real_T value)
  {
    return std::isinf(value);
  }

  // Test if single-precision value is infinite
  static boolean_T rtIsInfF(real32_T value)
  {
    return std::isinf(value);
  }

  // Test if value is not a number
  static boolean_T rtIsNaN(real_T value)
  {
    return std::isnan(value);
  }

  // Test if single-precision value is not a number
  static boolean_T rtIsNaNF(real32_T value)
  {
    return std::isnan(value);
  }
}

#ifndef HIL_SECTION
#define HIL_SECTION

// HIL Model parameters all compile time parameters [constexpr]
namespace HIL {
namespace CFG {

  constexpr double kPsiToPascals = 6894.757293168361;

  constexpr double kDt       = 1.0e-3;
  constexpr int    kSubsteps = 1;
  constexpr double kDryMass = 35.1;
  constexpr double kIxx =  2.99187775,    kIyy =  2.982988085,    kIzz =  2.425655653;
  constexpr double kIxy = -0.00346980728, kIxz =  0.00133612293,  kIyz = -0.02622986357;
  constexpr double kCgX = 0.000, kCgY = -0.000, kCgZ = -0.25830;
  constexpr double kGravity = 9.80665;      // along world -z
  constexpr double kGamma = 1.3995239400841304;    // gas model gamma at 300 K, 310 bar
  constexpr double kGasR  = 296.80305203938116;    // J/(kg K)
  constexpr double kT0    = 300.0;                 // FILL stagnation temperature [K]
  constexpr double kPa    = 97325.0;               // ambient [Pa] // ankara is used
  constexpr bool kAdiabaticT0 = true;
  constexpr double kTankVolume   = 9.0e-3;         // [m^3]
  constexpr double kTankPressure = 310.0e5;        // fill [Pa]
  constexpr double kInitialGasMass = 3.1333909594588785;   // [kg] -- 9 L of N2 at 310 bar, 300 K, real gas
  constexpr double kTankX = kCgX, kTankY = kCgY, kTankZ = kCgZ;
  constexpr double kNozzleSupplyLimit = 1800.0 * 6894.757293168361;   // 124.1056 bar

  struct NozzleSpec {
      double X, Y, Z;                  // body position [m]
      double DX, DY, DZ;               // thrust direction, body axes (normalised on use)
      double DEntry, DThroat, DExit;   // diameters [m]
      double HalfAngleDeg;             // cone half angle -- sets lambda = (1 + cos a) / 2
      double Efficiency;               // scalar on the whole thrust
  };

  constexpr NozzleSpec kNozzles[] = {
      {  0.330,  0.000, -0.2834282,  0.0, 0.0, 1.0,  6.5e-3, 4.7e-3, 12.6e-3, 15.0, 1.0 }, // N1
      {  0.000,  -0.330, -0.2834282,  0.0, 0.0, 1.0,  6.5e-3, 4.7e-3, 12.6e-3, 15.0, 1.0 }, // N2
      {  0.000, 0.330, -0.2834282,  0.0, 0.0, 1.0,  6.5e-3, 4.7e-3, 12.6e-3, 15.0, 1.0 }, // N3
      { -0.330,  0.000, -0.2834282,  0.0, 0.0, 1.0,  6.5e-3, 4.7e-3, 12.6e-3, 15.0, 1.0 }, // N4
  };

  constexpr int kNozzleCount = static_cast<int>(sizeof(kNozzles) / sizeof(kNozzles[0]));
  constexpr int kPressureCount = kNozzleCount + 1;

  constexpr double kInitPosX = 0.0, kInitPosY = 0.0, kInitPosZ = 9.0;
  constexpr double kInitVelX = 0.0, kInitVelY = 0.0, kInitVelZ = 0.0;
  constexpr double kInitRoll = 0.0, kInitPitch = 0.0, kInitYaw = 0.0;   // [rad], ZYX
  constexpr double kInitP    = 0.0, kInitQ     = 0.0, kInitR   = 0.0;   // body rates [rad/s]
  constexpr int kStateDoubles = 25;

  constexpr double kPi = 3.14159265358979323846;

  struct Vec3 { double X, Y, Z; };

  inline Vec3 Add(const Vec3& A, const Vec3& B)  { Vec3 r = { A.X + B.X, A.Y + B.Y, A.Z + B.Z }; return r; }
  inline Vec3 Sub(const Vec3& A, const Vec3& B)  { Vec3 r = { A.X - B.X, A.Y - B.Y, A.Z - B.Z }; return r; }
  inline Vec3 Scale(const Vec3& A, double S)     { Vec3 r = { A.X * S, A.Y * S, A.Z * S }; return r; }
  inline Vec3 Cross(const Vec3& A, const Vec3& B) {
      Vec3 r = { A.Y * B.Z - A.Z * B.Y, A.Z * B.X - A.X * B.Z, A.X * B.Y - A.Y * B.X };
      return r;
  }
  inline double Norm(const Vec3& A) { return std::sqrt(A.X * A.X + A.Y * A.Y + A.Z * A.Z); }

  inline Vec3 Unit(const Vec3& A, const Vec3& Fallback) {
      const double n = Norm(A);
      return (n > 1e-12) ? Scale(A, 1.0 / n) : Fallback;
  }

  inline double CircleArea(double Dia) { return 0.25 * kPi * Dia * Dia; }

  struct Nozzle {
      Vec3   Position;
      Vec3   Direction;       // unit, body-fixed
      double EntryArea;
      double ThroatArea;
      double ExitArea;
      double Divergence;      // lambda = (1 + cos a) / 2
      double Efficiency;
  };

  struct Derived {
      Nozzle Nozzles[kNozzleCount];
      double Iinv[9];         // inverse inertia, row-major
      Vec3   Cg;
      bool   Ok;              // false only if the configured inertia tensor is singular
  };

  struct State {
      Vec3   Position;
      Vec3   Velocity;
      double Q[4];              // w x y z, body -> world
      Vec3   Omega;             // body rates p q r
      double GasMass;
  };

  struct Deriv {
    Vec3 DPos, DVel;
    double DQ[4];
    Vec3 DOmega;
    double DGasMass;
  };

  // get inverse of the inertia
  bool BuildInertia(double Out[9]) {
      const double a =  kIxx, b = -kIxy, c = -kIxz;
      const double d = -kIxy, e =  kIyy, f = -kIyz;
      const double g = -kIxz, h = -kIyz, i =  kIzz;
      const double det = a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
      if (std::fabs(det) < 1e-18) return false;
      const double s = 1.0 / det;
      Out[0] = (e * i - f * h) * s;  Out[1] = (c * h - b * i) * s;  Out[2] = (b * f - c * e) * s;
      Out[3] = (f * g - d * i) * s;  Out[4] = (a * i - c * g) * s;  Out[5] = (c * d - a * f) * s;
      Out[6] = (d * h - e * g) * s;  Out[7] = (b * g - a * h) * s;  Out[8] = (a * e - b * d) * s;
      return true;
  }

  void QuatFromEuler(double Roll, double Pitch, double Yaw, double Q[4]) {
      const double cr = std::cos(0.5 * Roll),  sr = std::sin(0.5 * Roll);
      const double cp = std::cos(0.5 * Pitch), sp = std::sin(0.5 * Pitch);
      const double cy = std::cos(0.5 * Yaw),   sy = std::sin(0.5 * Yaw);
      Q[0] = cr * cp * cy + sr * sp * sy;
      Q[1] = sr * cp * cy - cr * sp * sy;
      Q[2] = cr * sp * cy + sr * cp * sy;
      Q[3] = cr * cp * sy - sr * sp * cy;
  }

  void Normalize(State& S) {
      double n = std::sqrt(S.Q[0] * S.Q[0] + S.Q[1] * S.Q[1] + S.Q[2] * S.Q[2] + S.Q[3] * S.Q[3]);
      if (n < 1e-12) { S.Q[0] = 1.0; S.Q[1] = S.Q[2] = S.Q[3] = 0.0; return; }
      // Keep w >= 0 so the returned quaternion does not flip sign mid-flight, which reads as a 360 degree jump
      // to a controller differencing successive attitudes.
      if (S.Q[0] < 0.0) n = -n;
      for (int i = 0; i < 4; ++i) S.Q[i] /= n;
  }

  double MachFromAreaRatio(double Gamma, double AreaRatio) {
      if (AreaRatio <= 1.0) return 1.0;
      const double e   = 0.5 * (Gamma - 1.0);
      const double exp = (Gamma + 1.0) / (2.0 * (Gamma - 1.0));
      double M = 2.0;
      for (int i = 0; i < 40; ++i) {
          const double t  = (2.0 / (Gamma + 1.0)) * (1.0 + e * M * M);
          const double AR = std::pow(t, exp) / M;
          const double dr = AR * (M * M - 1.0) / (M * (1.0 + e * M * M));
          if (std::fabs(dr) < 1e-30) break;
          double Mn = M - (AR - AreaRatio) / dr;
          if (Mn < 1.0001) Mn = 1.0001;
          if (Mn > 60.0)   Mn = 60.0;
          const bool settled = std::fabs(Mn - M) <= 1e-13 * (1.0 + std::fabs(M));
          M = Mn;
          if (settled) break;
      }
      return M;
  }

  double StagTemperature(const State& S) {
      if (!kAdiabaticT0 || kInitialGasMass <= 0.0) return kT0;
      const double frac = (S.GasMass > 0.0) ? (S.GasMass / kInitialGasMass) : 0.0;
      if (frac <= 0.0) return kT0;
      const double T = kT0 * std::pow(frac, kGamma - 1.0);
      return (T > 1.0) ? T : 1.0;      // floored: a numerically empty tank must not divide by zero downstream
  }

  void NozzleFlow(const Nozzle& N, double P0, double T0, double& Thrust, double& Mdot) {
      Thrust = 0.0;
      Mdot   = 0.0;
      if (P0 <= kPa) return;                            // no flow, or flow the wrong way
      if (N.ExitArea <= 0.0) return;
      if (T0 <= 0.0) return;

      // The narrowest section is what meters. An entrance smaller than the throat therefore takes over as the
      // restriction rather than being decorative geometry, and the nozzle expands from whatever actually chokes.
      const double aStar = (N.EntryArea > 0.0 && N.EntryArea < N.ThroatArea) ? N.EntryArea : N.ThroatArea;
      if (aStar <= 0.0) return;

      const double g  = kGamma;
      const double e  = 0.5 * (g - 1.0);
      const double gp = g + 1.0;

      // Choked while the sonic static pressure still clears ambient.
      const double critical = std::pow(2.0 / gp, g / (g - 1.0));
      if (P0 * critical >= kPa) {
          const double k = std::pow(2.0 / gp, gp / (2.0 * (g - 1.0)));
          Mdot = aStar * P0 * std::sqrt(g / (kGasR * T0)) * k;

          const double ar = (N.ExitArea > aStar) ? (N.ExitArea / aStar) : 1.0;
          const double Me = MachFromAreaRatio(g, ar);
          const double tr = 1.0 + e * Me * Me;
          const double Te = T0 / tr;
          const double pe = P0 / std::pow(tr, g / (g - 1.0));
          const double Ve = Me * std::sqrt(g * kGasR * Te);

          Thrust = N.Efficiency * (N.Divergence * Mdot * Ve + (pe - kPa) * N.ExitArea);
          return;
      }

      // Subcritical: a weak subsonic jet expanded to ambient at the exit plane.
      const double pr = P0 / kPa;
      const double Te = T0 / std::pow(pr, (g - 1.0) / g);
      const double dT = T0 - Te;
      if (dT <= 0.0) return;
      const double Ve   = std::sqrt(2.0 * g * kGasR / (g - 1.0) * dT);
      const double rhoE = kPa / (kGasR * Te);
      Mdot   = rhoE * Ve * N.ExitArea;
      Thrust = N.Efficiency * N.Divergence * Mdot * Ve;
  }

  void RotationMatrix(const double Q[4], double R[9]) {
      const double w = Q[0], x = Q[1], y = Q[2], z = Q[3];
      R[0] = 1 - 2 * (y * y + z * z); R[1] = 2 * (x * y - w * z);     R[2] = 2 * (x * z + w * y);
      R[3] = 2 * (x * y + w * z);     R[4] = 1 - 2 * (x * x + z * z); R[5] = 2 * (y * z - w * x);
      R[6] = 2 * (x * z - w * y);     R[7] = 2 * (y * z + w * x);     R[8] = 1 - 2 * (x * x + y * y);
  }

  inline Vec3 Rotate(const double R[9], const Vec3& V) {
      Vec3 o = { R[0] * V.X + R[1] * V.Y + R[2] * V.Z,
                R[3] * V.X + R[4] * V.Y + R[5] * V.Z,
                R[6] * V.X + R[7] * V.Y + R[8] * V.Z };
      return o;
  }

  void EulerFromQuat(const double Q[4], double& Roll, double& Pitch, double& Yaw) {
      const double w = Q[0], x = Q[1], y = Q[2], z = Q[3];
      Roll = std::atan2(2.0 * (w * x + y * z), 1.0 - 2.0 * (x * x + y * y));
      double s = 2.0 * (w * y - z * x);
      if (s >  1.0) s =  1.0;
      if (s < -1.0) s = -1.0;
      Pitch = std::asin(s);        // +-90 deg is gimbal lock for the ANGLES; the quaternion stays valid there
      Yaw = std::atan2(2.0 * (w * z + x * y), 1.0 - 2.0 * (y * y + z * z));
  }

  Derived BuildDerived() {
      Derived D;
      for (int i = 0; i < kNozzleCount; ++i) {
          const NozzleSpec& s = kNozzles[i];
          Nozzle& n = D.Nozzles[i];
          n.Position.X = s.X; n.Position.Y = s.Y; n.Position.Z = s.Z;
          Vec3 dir = { s.DX, s.DY, s.DZ };
          Vec3 up  = { 0.0, 0.0, 1.0 };
          n.Direction  = Unit(dir, up);
          n.EntryArea  = CircleArea(s.DEntry);
          n.ThroatArea = CircleArea(s.DThroat);
          n.ExitArea   = CircleArea(s.DExit);
          n.Divergence = 0.5 * (1.0 + std::cos(s.HalfAngleDeg * kPi / 180.0));
          n.Efficiency = s.Efficiency;
      }
      D.Cg.X = kCgX; D.Cg.Y = kCgY; D.Cg.Z = kCgZ;
      D.Ok = BuildInertia(D.Iinv);
      return D;
  }

}}

const HIL::CFG::Derived& Cfg() {
    static const HIL::CFG::Derived d = HIL::CFG::BuildDerived();
    return d;
}

namespace HIL {
namespace CFG {

  // Total thrust force and moment in BODY axes. Moments are taken about the CG, which is where the inertia
  // tensor is defined -- taking them about the body origin instead would couple translation into rotation
  // wrongly, and every nozzle here is deliberately off-centre.
  void BodyLoads(const double* Thrusts, Vec3& Force, Vec3& Moment) {
      const Derived& C = Cfg();
      Force.X = Force.Y = Force.Z = 0.0;
      Moment.X = Moment.Y = Moment.Z = 0.0;
      for (int i = 0; i < kNozzleCount; ++i) {
          const Nozzle& n = C.Nozzles[i];
          const Vec3 f = Scale(n.Direction, Thrusts[i]);
          Force  = Add(Force, f);
          Moment = Add(Moment, Cross(Sub(n.Position, C.Cg), f));
      }
  }

  Deriv Derivative(const State& S, const Vec3& ForceB, const Vec3& MomentB, double MdotTotal) {
      const Derived& C = Cfg();
      Deriv d;
      double R[9];
      RotationMatrix(S.Q, R);

      d.DPos = S.Velocity;

      // Newton, at the CURRENT total mass: the vehicle accelerates harder as the tank empties, which is the
      // whole reason the propellant is tracked rather than assumed constant.
      //
      // WEIGHT ACTS ON THE DRY MASS *AND* ON THE GAS STILL IN THE TANK. At 310 bar the charge is a small
      // fraction of the vehicle but a large fraction of the NET vertical acceleration near hover.
      const double mass = kDryMass + ((S.GasMass > 0.0) ? S.GasMass : 0.0);
      const Vec3   fW   = Rotate(R, ForceB);      // thrust, body -> world
      const double W    = mass * kGravity;        // weight, along world -z
      d.DVel.X =  fW.X / mass;
      d.DVel.Y =  fW.Y / mass;
      d.DVel.Z = (fW.Z - W) / mass;

      // NO GRAVITY MOMENT, which is not an omission: weight acts at the CG and the moments are taken about the
      // CG, so its arm is zero by construction.

      // Quaternion kinematics, qdot = 0.5 * q * (0, omega).
      const double w = S.Q[0], x = S.Q[1], y = S.Q[2], z = S.Q[3];
      const double p = S.Omega.X, q = S.Omega.Y, r = S.Omega.Z;
      d.DQ[0] = 0.5 * (-x * p - y * q - z * r);
      d.DQ[1] = 0.5 * ( w * p + y * r - z * q);
      d.DQ[2] = 0.5 * ( w * q - x * r + z * p);
      d.DQ[3] = 0.5 * ( w * r + x * q - y * p);

      // Euler: I wdot = M - w x (I w). The gyroscopic term is what makes this a genuine 6-DOF model rather
      // than three decoupled rotations, and it is why a spinning airframe precesses under an off-axis thruster.
      Vec3 Iw;
      Iw.X =  kIxx * p - kIxy * q - kIxz * r;
      Iw.Y = -kIxy * p + kIyy * q - kIyz * r;
      Iw.Z = -kIxz * p - kIyz * q + kIzz * r;
      const Vec3 rhs = Sub(MomentB, Cross(S.Omega, Iw));
      d.DOmega.X = C.Iinv[0] * rhs.X + C.Iinv[1] * rhs.Y + C.Iinv[2] * rhs.Z;
      d.DOmega.Y = C.Iinv[3] * rhs.X + C.Iinv[4] * rhs.Y + C.Iinv[5] * rhs.Z;
      d.DOmega.Z = C.Iinv[6] * rhs.X + C.Iinv[7] * rhs.Y + C.Iinv[8] * rhs.Z;

      // The tank drains at the rate the nozzles pass. Clamped at empty so a stiff step cannot integrate the
      // mass negative and hand the controller a lighter-than-dry vehicle.
      d.DGasMass = (S.GasMass > 0.0) ? -MdotTotal : 0.0;
      return d;
  }

  State Advance(const State& S, const Deriv& D, double H) {
      State o;
      o.Position = Add(S.Position, Scale(D.DPos, H));
      o.Velocity = Add(S.Velocity, Scale(D.DVel, H));
      for (int i = 0; i < 4; ++i) o.Q[i] = S.Q[i] + D.DQ[i] * H;
      o.Omega   = Add(S.Omega, Scale(D.DOmega, H));
      o.GasMass = S.GasMass + D.DGasMass * H;
      if (o.GasMass < 0.0) o.GasMass = 0.0;
      return o;
  }

  void StepRK4(State& S, const Vec3& ForceB, const Vec3& MomentB, double MdotTotal, double H) {
      const Deriv k1 = Derivative(S, ForceB, MomentB, MdotTotal);
      const State s2 = Advance(S, k1, 0.5 * H);
      const Deriv k2 = Derivative(s2, ForceB, MomentB, MdotTotal);
      const State s3 = Advance(S, k2, 0.5 * H);
      const Deriv k3 = Derivative(s3, ForceB, MomentB, MdotTotal);
      const State s4 = Advance(S, k3, H);
      const Deriv k4 = Derivative(s4, ForceB, MomentB, MdotTotal);

      Deriv m;
      m.DPos   = Scale(Add(Add(k1.DPos, Scale(k2.DPos, 2.0)), Add(Scale(k3.DPos, 2.0), k4.DPos)), 1.0 / 6.0);
      m.DVel   = Scale(Add(Add(k1.DVel, Scale(k2.DVel, 2.0)), Add(Scale(k3.DVel, 2.0), k4.DVel)), 1.0 / 6.0);
      m.DOmega = Scale(Add(Add(k1.DOmega, Scale(k2.DOmega, 2.0)),
                          Add(Scale(k3.DOmega, 2.0), k4.DOmega)), 1.0 / 6.0);
      m.DGasMass = (k1.DGasMass + 2.0 * k2.DGasMass + 2.0 * k3.DGasMass + k4.DGasMass) / 6.0;
      for (int i = 0; i < 4; ++i)
          m.DQ[i] = (k1.DQ[i] + 2.0 * k2.DQ[i] + 2.0 * k3.DQ[i] + k4.DQ[i]) / 6.0;

      S = Advance(S, m, H);
      Normalize(S);
  }

  void Report(const State& S, const Vec3& ForceB, const Vec3& MomentB, double MdotTotal, double* Out) {
      // Accelerations are taken AT the state being returned, not carried over from the integrator's last
      // stage, so what the controller reads is what the vehicle is doing now.
      const Deriv d = Derivative(S, ForceB, MomentB, MdotTotal);

      double roll, pitch, yaw;
      EulerFromQuat(S.Q, roll, pitch, yaw);

      Out[0]  = S.Position.X;  Out[1]  = S.Position.Y;  Out[2]  = S.Position.Z;
      Out[3]  = S.Velocity.X;  Out[4]  = S.Velocity.Y;  Out[5]  = S.Velocity.Z;
      Out[6]  = d.DVel.X;      Out[7]  = d.DVel.Y;      Out[8]  = d.DVel.Z;
      Out[9]  = S.Q[0];        Out[10] = S.Q[1];        Out[11] = S.Q[2];   Out[12] = S.Q[3];
      Out[13] = roll;          Out[14] = pitch;         Out[15] = yaw;
      Out[16] = S.Omega.X;     Out[17] = S.Omega.Y;     Out[18] = S.Omega.Z;
      Out[19] = d.DOmega.X;    Out[20] = d.DOmega.Y;    Out[21] = d.DOmega.Z;
      Out[22] = kDryMass + S.GasMass;
      Out[23] = S.GasMass;
      Out[24] = (S.GasMass > 0.0) ? MdotTotal : 0.0;
  }

  State InitialState() {
      State S;
      S.Position.X = kInitPosX; S.Position.Y = kInitPosY; S.Position.Z = kInitPosZ;
      S.Velocity.X = kInitVelX; S.Velocity.Y = kInitVelY; S.Velocity.Z = kInitVelZ;
      S.Omega.X    = kInitP;    S.Omega.Y    = kInitQ;    S.Omega.Z    = kInitR;
      QuatFromEuler(kInitRoll, kInitPitch, kInitYaw, S.Q);
      Normalize(S);
      S.GasMass = (kInitialGasMass > 0.0) ? kInitialGasMass : 0.0;
      return S;
  }

  State& Live() {
      // Same first-use initialisation as the derived table, and the reason this plant needs no configure call:
      // by the time anything can reach the state, it is already the configured vehicle at its start point.
      static State s = InitialState();
      return s;
  }

  int HilStaticReset(void) {
      Live() = InitialState();
      return Cfg().Ok ? kStateDoubles : 0;
  }

  int HilStaticSetInitialState(const double* PositionXYZ, const double* EulerRPY,
          const double* VelocityXYZ, const double* RatesPQR) {
      if (!Cfg().Ok) return 0;
      State& S = Live();
      if (PositionXYZ != 0) { S.Position.X = PositionXYZ[0]; S.Position.Y = PositionXYZ[1];
                              S.Position.Z = PositionXYZ[2]; }
      if (EulerRPY != 0) {
          // Built, not read -- so a pitch of exactly +-90 degrees is a perfectly ordinary input here even
          // though it is the angle triple that OutState cannot uniquely report back.
          QuatFromEuler(EulerRPY[0], EulerRPY[1], EulerRPY[2], S.Q);
          Normalize(S);
      }
      if (VelocityXYZ != 0) { S.Velocity.X = VelocityXYZ[0]; S.Velocity.Y = VelocityXYZ[1];
                              S.Velocity.Z = VelocityXYZ[2]; }
      if (RatesPQR != 0)    { S.Omega.X = RatesPQR[0]; S.Omega.Y = RatesPQR[1]; S.Omega.Z = RatesPQR[2]; }
      return kStateDoubles;
  }

  int HilStaticStep(const double* Pressures, double* OutState) {
      if (OutState == 0 || !Cfg().Ok) return 0;
      State& S = Live();

      // No pressures is a READ: report the current state without advancing. That is how the initial condition
      // comes back, and how a slow logger samples without disturbing the run.
      if (Pressures == 0) {
          Vec3 zero; zero.X = zero.Y = zero.Z = 0.0;
          Report(S, zero, zero, 0.0, OutState);
          return kStateDoubles;
      }

      // Thrust and mass flow per nozzle, from its own measured pressure. Capped at the manifold: a branch
      // downstream of the supply cannot carry more total pressure than the supply, and a sensor reading high
      // on noise would otherwise inject thrust the hardware never made.
      //
      // THEN capped at the FEED CEILING (kNozzleSupplyLimit), which is the tighter of the two on this
      // vehicle: the manifold runs to 310 bar and the feed can only deliver 124.11 bar to a nozzle.
      //
      // AN EMPTY TANK PRODUCES NOTHING, whatever the pressure array says. A stale or stuck sensor cannot fly
      // the vehicle on propellant it does not have.
      double thrusts[kNozzleCount] = { 0.0 };
      double mdotTotal = 0.0;
      if (S.GasMass > 0.0) {
          const double T0       = StagTemperature(S);
          const double manifold = Pressures[0];
          for (int i = 0; i < kNozzleCount; ++i) {
              double p0 = Pressures[1 + i];
              if (manifold > 0.0 && p0 > manifold) p0 = manifold;
              if (kNozzleSupplyLimit > 0.0 && p0 > kNozzleSupplyLimit) p0 = kNozzleSupplyLimit;
              double mdot = 0.0;
              NozzleFlow(Cfg().Nozzles[i], p0, T0, thrusts[i], mdot);
              mdotTotal += mdot;
          }
      }

      Vec3 forceB, momentB;
      BodyLoads(thrusts, forceB, momentB);

      // ONE STEP per call, at the header's kDt, optionally split into substeps for accuracy.
      const int    subs = (kSubsteps > 0) ? kSubsteps : 1;
      const double h    = kDt / subs;
      for (int i = 0; i < subs; ++i) StepRK4(S, forceB, momentB, mdotTotal, h);

      Report(S, forceB, momentB, mdotTotal, OutState);
      return kStateDoubles;
  }

}}

#endif // HIL_SECTION


// private model entry point functions
extern void HWIL_derivatives();
//extern "C"
//{
//  real_T rtNaN { -std::numeric_limits<real_T>::quiet_NaN() };
//
//  real_T rtInf { std::numeric_limits<real_T>::infinity() };
//
//  real_T rtMinusInf { -std::numeric_limits<real_T>::infinity() };
//
//  real32_T rtNaNF { -std::numeric_limits<real32_T>::quiet_NaN() };
//
//  real32_T rtInfF { std::numeric_limits<real32_T>::infinity() };
//
//  real32_T rtMinusInfF { -std::numeric_limits<real32_T>::infinity() };
//}

//
// This function updates continuous states using the ODE3 fixed-step
// solver algorithm
//
void HWIL::rt_ertODEUpdateContinuousStates(RTWSolverInfo *si )
{
  // Solver Matrices
  static const real_T rt_ODE3_A[3]{
    1.0/2.0, 3.0/4.0, 1.0
  };

  static const real_T rt_ODE3_B[3][3]{
    { 1.0/2.0, 0.0, 0.0 },

    { 0.0, 3.0/4.0, 0.0 },

    { 2.0/9.0, 1.0/3.0, 4.0/9.0 }
  };

  time_T t { rtsiGetT(si) };

  time_T tnew { rtsiGetSolverStopTime(si) };

  time_T h { rtsiGetStepSize(si) };

  real_T *x { rtsiGetContStates(si) };

  ODE3_IntgData *id { static_cast<ODE3_IntgData *>(rtsiGetSolverData(si)) };

  real_T *y { id->y };

  real_T *f0 { id->f[0] };

  real_T *f1 { id->f[1] };

  real_T *f2 { id->f[2] };

  real_T hB[3];
  int_T i;
  int_T nXc { 2 };

  rtsiSetSimTimeStep(si,MINOR_TIME_STEP);

  // Save the state values at time t in y, we'll use x as ynew.
  (void) std::memcpy(y, x,
                     static_cast<uint_T>(nXc)*sizeof(real_T));

  // Assumes that rtsiSetT and ModelOutputs are up-to-date
  // f0 = f(t,y)
  rtsiSetdX(si, f0);
  HWIL_derivatives();

  // f(:,2) = feval(odefile, t + hA(1), y + f*hB(:,1), args(:)(*));
  hB[0] = h * rt_ODE3_B[0][0];
  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[0]);
  rtsiSetdX(si, f1);
  this->step();
  HWIL_derivatives();

  // f(:,3) = feval(odefile, t + hA(2), y + f*hB(:,2), args(:)(*));
  for (i = 0; i <= 1; i++) {
    hB[i] = h * rt_ODE3_B[1][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[1]);
  rtsiSetdX(si, f2);
  this->step();
  HWIL_derivatives();

  // tnew = t + hA(3);
  // ynew = y + f*hB(:,3);
  for (i = 0; i <= 2; i++) {
    hB[i] = h * rt_ODE3_B[2][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1] + f2[i]*hB[2]);
  }

  rtsiSetT(si, tnew);
  rtsiSetSimTimeStep(si,MAJOR_TIME_STEP);
}

// Model step function
void HWIL::step()
{
  real_T rtb_CastToDouble1;
  real32_T rtb_Bias;
  real32_T tmp;
  real32_T tmp_0;
  real32_T tmp_1;
  real32_T tmp_2;

  // run HIL
  double Pressures[5] = {rtU.P_nozzle_manifold, rtU.P_nozzle_1, rtU.P_nozzle_2, rtU.P_nozzle_3, rtU.P_nozzle_4};
  for (int i = 0; i < 5; i++) {
    Pressures[i] = Pressures[i] * HIL::CFG::kPsiToPascals;
  }

  double OutState[25];
  HIL::CFG::HilStaticStep(Pressures, OutState);



  if ((&rtM)->isMajorTimeStep()) {
    // set solver stop time
    rtsiSetSolverStopTime(&(&rtM)->solverInfo,(((&rtM)->Timing.clockTick0+1)*
      (&rtM)->Timing.stepSize0));
  }                                    // end MajorTimeStep

  // Update absolute time of base rate at minor time step
  if ((&rtM)->isMinorTimeStep()) {
    (&rtM)->Timing.t[0] = rtsiGetT(&(&rtM)->solverInfo);
  }

  // Saturate: '<S1>/Saturation5' incorporates:
  //   Inport: '<Root>/P_nozzle_manifold'

  if (rtU.P_nozzle_manifold <= 0.0F) {
    tmp = 0.0F;
  } else {
    tmp = rtU.P_nozzle_manifold;
  }

  // Bias: '<S1>/Bias' incorporates:
  //   Gain: '<S1>/Gain'
  //   Gain: '<S1>/Gain2'
  //   Saturate: '<S1>/Saturation5'

  rtb_Bias = 0.069F * tmp * 0.0095F + 34.0F;

  // Outport: '<Root>/mass'
  rtY.mass = rtb_Bias;

  // Integrator: '<S1>/Integrator1' incorporates:
  //   Inport: '<Root>/X0'

  // Limited  Integrator
  if (rtDW.Integrator1_DWORK1) {
    rtX.Integrator1_CSTATE = rtU.X0;
  }

  if ((!(rtX.Integrator1_CSTATE >= (rtInf))) && (rtX.Integrator1_CSTATE <= 0.0))
  {
    rtX.Integrator1_CSTATE = 0.0;
  }

  // Switch: '<S1>/Switch' incorporates:
  //   Constant: '<S1>/Constant'
  //   Inport: '<Root>/enable_spring'

  if (rtU.enable_spring) {
    // Gain: '<S1>/Gain4' incorporates:
    //   Integrator: '<S1>/Integrator1'
    //   Sum: '<S1>/Sum2'

    rtb_CastToDouble1 = (rtX.Integrator1_CSTATE - 4.613664285714286) * -140.0;

    // Saturate: '<S1>/Saturation'
    if (rtb_CastToDouble1 > 623.35000000000014) {
      rtb_CastToDouble1 = 623.35000000000014;
    } else if (rtb_CastToDouble1 < 0.0) {
      rtb_CastToDouble1 = 0.0;
    }

    // End of Saturate: '<S1>/Saturation'
  } else {
    rtb_CastToDouble1 = 0.0;
  }

  // End of Switch: '<S1>/Switch'

  // Saturate: '<S1>/Saturation4' incorporates:
  //   Inport: '<Root>/F_nozzle_1'

  if (rtU.F_nozzle_1 <= 0.0F) {
    tmp = 0.0F;
  } else {
    tmp = rtU.F_nozzle_1;
  }

  // Saturate: '<S1>/Saturation3' incorporates:
  //   Inport: '<Root>/F_nozzle_2'

  if (rtU.F_nozzle_2 <= 0.0F) {
    tmp_0 = 0.0F;
  } else {
    tmp_0 = rtU.F_nozzle_2;
  }

  // Saturate: '<S1>/Saturation2' incorporates:
  //   Inport: '<Root>/F_nozzle_3'

  if (rtU.F_nozzle_3 <= 0.0F) {
    tmp_1 = 0.0F;
  } else {
    tmp_1 = rtU.F_nozzle_3;
  }

  // Saturate: '<S1>/Saturation1' incorporates:
  //   Inport: '<Root>/F_nozzle_4'

  if (rtU.F_nozzle_4 <= 0.0F) {
    tmp_2 = 0.0F;
  } else {
    tmp_2 = rtU.F_nozzle_4;
  }

  // Product: '<S1>/Divide' incorporates:
  //   DataTypeConversion: '<S1>/Cast To Double1'
  //   Gain: '<S1>/Gain1'
  //   Saturate: '<S1>/Saturation1'
  //   Saturate: '<S1>/Saturation2'
  //   Saturate: '<S1>/Saturation3'
  //   Saturate: '<S1>/Saturation4'
  //   Sum: '<S1>/Sum'
  //   Sum: '<S1>/Sum1'

  rtDW.Divide = (((((tmp + tmp_0) + tmp_1) + tmp_2) + rtb_CastToDouble1) +
                 -9.81F * rtb_Bias) / rtb_Bias;

  // Outport: '<Root>/acceleration' incorporates:
  //   Constant: '<S1>/Constant4'

  rtY.acceleration[0] = 0.0;
  rtY.acceleration[1] = 0.0;
  rtY.acceleration[2] = rtDW.Divide;

  // Outport: '<Root>/position' incorporates:
  //   Integrator: '<S1>/Integrator1'

  rtY.position = rtX.Integrator1_CSTATE;

  // Outport: '<Root>/lidar_height' incorporates:
  //   Integrator: '<S1>/Integrator1'

  rtY.lidar_height = rtX.Integrator1_CSTATE;

  // Integrator: '<S1>/Integrator' incorporates:
  //   Inport: '<Root>/V0'

  if (rtDW.Integrator_DWORK1) {
    rtX.Integrator_CSTATE = rtU.V0;
  }

  // Integrator: '<S1>/Integrator'
  rtDW.Integrator = rtX.Integrator_CSTATE;

  // Outport: '<Root>/velocity'
  rtY.velocity = rtDW.Integrator;
  if ((&rtM)->isMajorTimeStep()) {
    // Update for Integrator: '<S1>/Integrator1'
    rtDW.Integrator1_DWORK1 = false;

    // Update for Integrator: '<S1>/Integrator'
    rtDW.Integrator_DWORK1 = false;
  }                                    // end MajorTimeStep

  if ((&rtM)->isMajorTimeStep()) {
    rt_ertODEUpdateContinuousStates(&(&rtM)->solverInfo);

    // Update absolute time for base rate
    // The "clockTick0" counts the number of times the code of this task has
    //  been executed. The absolute time is the multiplication of "clockTick0"
    //  and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
    //  overflow during the application lifespan selected.

    ++(&rtM)->Timing.clockTick0;
    (&rtM)->Timing.t[0] = rtsiGetSolverStopTime(&(&rtM)->solverInfo);

    {
      // Update absolute timer for sample time: [0.001s, 0.0s]
      // The "clockTick1" counts the number of times the code of this task has
      //  been executed. The resolution of this integer timer is 0.001, which is the step size
      //  of the task. Size of "clockTick1" ensures timer will not overflow during the
      //  application lifespan selected.

      (&rtM)->Timing.clockTick1++;
    }
  }

  /*
   *       Out[0]  = S.Position.X;  Out[1]  = S.Position.Y;  Out[2]  = S.Position.Z;
      Out[3]  = S.Velocity.X;  Out[4]  = S.Velocity.Y;  Out[5]  = S.Velocity.Z;
      Out[6]  = d.DVel.X;      Out[7]  = d.DVel.Y;      Out[8]  = d.DVel.Z;
      Out[9]  = S.Q[0];        Out[10] = S.Q[1];        Out[11] = S.Q[2];   Out[12] = S.Q[3];
      Out[13] = roll;          Out[14] = pitch;         Out[15] = yaw;
      Out[16] = S.Omega.X;     Out[17] = S.Omega.Y;     Out[18] = S.Omega.Z;
      Out[19] = d.DOmega.X;    Out[20] = d.DOmega.Y;    Out[21] = d.DOmega.Z;
      Out[22] = kDryMass + S.GasMass;
      Out[23] = S.GasMass;
      Out[24] = (S.GasMass > 0.0) ? MdotTotal : 0.0;
   * */

  rtY.position = OutState[2];
  rtY.acceleration[0] = OutState[6];
  rtY.acceleration[1] = OutState[7];
  rtY.acceleration[2] = OutState[8];

  rtY.velocity = OutState[5];

  rtY.angular_velocity[0] = OutState[16];
  rtY.angular_velocity[1] = OutState[17];
  rtY.angular_velocity[2] = OutState[18];

  rtY.quaternion_sim[0] = OutState[10];
  rtY.quaternion_sim[1] = OutState[11];
  rtY.quaternion_sim[2] = OutState[12];
  rtY.quaternion_sim[3] = OutState[9];

  rtY.mass = OutState[22];

  // end MajorTimeStep
}

// Derivatives for root system: '<Root>'
void HWIL::HWIL_derivatives()
{
  HWIL::XDot *_rtXdot;
  boolean_T lsat;
  boolean_T usat;
  _rtXdot = ((XDot *) (&rtM)->derivs);

  // Derivatives for Integrator: '<S1>/Integrator1'
  lsat = (rtX.Integrator1_CSTATE <= 0.0);
  usat = (rtX.Integrator1_CSTATE >= (rtInf));
  if (((!lsat) && (!usat)) || (lsat && (rtDW.Integrator > 0.0)) || (usat &&
       (rtDW.Integrator < 0.0))) {
    _rtXdot->Integrator1_CSTATE = rtDW.Integrator;
  } else {
    // in saturation
    _rtXdot->Integrator1_CSTATE = 0.0;
  }

  // End of Derivatives for Integrator: '<S1>/Integrator1'

  // Derivatives for Integrator: '<S1>/Integrator'
  _rtXdot->Integrator_CSTATE = rtDW.Divide;
}

// Model initialize function
void HWIL::initialize()
{

  // Initialize the HIL to the initial settings
  HIL::CFG::HilStaticReset();

  // Registration code
  {
    // Setup solver object
    rtsiSetSimTimeStepPtr(&(&rtM)->solverInfo, &(&rtM)->Timing.simTimeStep);
    rtsiSetTPtr(&(&rtM)->solverInfo, (&rtM)->getTPtrPtr());
    rtsiSetStepSizePtr(&(&rtM)->solverInfo, &(&rtM)->Timing.stepSize0);
    rtsiSetdXPtr(&(&rtM)->solverInfo, &(&rtM)->derivs);
    rtsiSetContStatesPtr(&(&rtM)->solverInfo, (real_T **) &(&rtM)->contStates);
    rtsiSetNumContStatesPtr(&(&rtM)->solverInfo, &(&rtM)->Sizes.numContStates);
    rtsiSetNumPeriodicContStatesPtr(&(&rtM)->solverInfo, &(&rtM)
      ->Sizes.numPeriodicContStates);
    rtsiSetPeriodicContStateIndicesPtr(&(&rtM)->solverInfo, &(&rtM)
      ->periodicContStateIndices);
    rtsiSetPeriodicContStateRangesPtr(&(&rtM)->solverInfo, &(&rtM)
      ->periodicContStateRanges);
    rtsiSetContStateDisabledPtr(&(&rtM)->solverInfo, (boolean_T**) &(&rtM)
      ->contStateDisabled);
    rtsiSetErrorStatusPtr(&(&rtM)->solverInfo, (&rtM)->getErrorStatusPtr());
    rtsiSetRTModelPtr(&(&rtM)->solverInfo, (&rtM));
  }

  rtsiSetSimTimeStep(&(&rtM)->solverInfo, MAJOR_TIME_STEP);
  rtsiSetIsMinorTimeStepWithModeChange(&(&rtM)->solverInfo, false);
  rtsiSetIsContModeFrozen(&(&rtM)->solverInfo, false);
  (&rtM)->intgData.y = (&rtM)->odeY;
  (&rtM)->intgData.f[0] = (&rtM)->odeF[0];
  (&rtM)->intgData.f[1] = (&rtM)->odeF[1];
  (&rtM)->intgData.f[2] = (&rtM)->odeF[2];
  (&rtM)->contStates = ((X *) &rtX);
  (&rtM)->contStateDisabled = ((XDis *) &rtXDis);
  (&rtM)->Timing.tStart = (0.0);
  rtsiSetSolverData(&(&rtM)->solverInfo, static_cast<void *>(&(&rtM)->intgData));
  rtsiSetSolverName(&(&rtM)->solverInfo,"ode3");
  (&rtM)->setTPtr(&(&rtM)->Timing.tArray[0]);
  (&rtM)->Timing.stepSize0 = 0.001;
  (&rtM)->setFirstInitCond(1);

  // InitializeConditions for Integrator: '<S1>/Integrator1' incorporates:
  //   Integrator: '<S1>/Integrator'

  if ((&rtM)->isFirstInitCond()) {
    rtX.Integrator1_CSTATE = 0.0;
    rtX.Integrator_CSTATE = 0.0;
  }

  rtDW.Integrator1_DWORK1 = true;

  // End of InitializeConditions for Integrator: '<S1>/Integrator1'

  // InitializeConditions for Integrator: '<S1>/Integrator'
  rtDW.Integrator_DWORK1 = true;

  // ConstCode for Outport: '<Root>/quaternion_sim' incorporates:
  //   Constant: '<S1>/Constant2'

  rtY.quaternion_sim[0] = 0.0F;
  rtY.quaternion_sim[1] = 0.0F;
  rtY.quaternion_sim[2] = 0.0F;
  rtY.quaternion_sim[3] = 1.0F;

  // set "at time zero" to false
  if ((&rtM)->isFirstInitCond()) {
    (&rtM)->setFirstInitCond(0);
  }


  // initialize the HIL as well
  double PositionXYZ[3] = {0.0, 0.0, 2.0};
  double EulerRPY[3] = {0.0, 0.0, 0.0};
  double VelocityXYZ[3] = {0.0, 0.0, 0.0};
  double RatesPQR[3] = {0.0, 0.0, 0.0};

  HIL::CFG::HilStaticSetInitialState(PositionXYZ, EulerRPY, VelocityXYZ, RatesPQR);

}

time_T** HWIL::RT_MODEL::getTPtrPtr()
{
  return &(Timing.t);
}

time_T* HWIL::RT_MODEL::getTPtr() const
{
  return (Timing.t);
}

void HWIL::RT_MODEL::setTPtr(time_T* aTPtr)
{
  (Timing.t = aTPtr);
}

boolean_T HWIL::RT_MODEL::getStopRequested() const
{
  return (Timing.stopRequestedFlag);
}

void HWIL::RT_MODEL::setStopRequested(boolean_T aStopRequested)
{
  (Timing.stopRequestedFlag = aStopRequested);
}

boolean_T HWIL::RT_MODEL::isMinorTimeStep() const
{
  return ((Timing.simTimeStep) == MINOR_TIME_STEP);
}

void HWIL::RT_MODEL::setFirstInitCond(boolean_T aFirstInitCond)
{
  (Timing.firstInitCondFlag = aFirstInitCond);
}

boolean_T HWIL::RT_MODEL::isFirstInitCond() const
{
  return (Timing.firstInitCondFlag);
}

boolean_T HWIL::RT_MODEL::isMajorTimeStep() const
{
  return ((Timing.simTimeStep) == MAJOR_TIME_STEP);
}

boolean_T* HWIL::RT_MODEL::getStopRequestedPtr()
{
  return (&(Timing.stopRequestedFlag));
}

const char_T** HWIL::RT_MODEL::getErrorStatusPtr()
{
  return &errorStatus;
}

time_T HWIL::RT_MODEL::getTStart() const
{
  return (Timing.tStart);
}

const char_T* HWIL::RT_MODEL::getErrorStatus() const
{
  return (errorStatus);
}

void HWIL::RT_MODEL::setErrorStatus(const char_T* const aErrorStatus)
{
  (errorStatus = aErrorStatus);
}

// Constructor
HWIL::HWIL() :
  rtU(),
  rtY(),
  rtDW(),
  rtX(),
  rtXDis(),
  rtM()
{
  // Currently there is no constructor body generated.
}

// Destructor
// Currently there is no destructor body generated.
HWIL::~HWIL() = default;

// Real-Time Model get method
HWIL::RT_MODEL * HWIL::getRTM()
{
  return (&rtM);
}

//
// File trailer for generated code.
//
// [EOF]
//
