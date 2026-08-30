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
