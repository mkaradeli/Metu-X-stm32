//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: actuatorKalman.cpp
//
// Code generated for Simulink model 'actuatorKalman'.
//
// Model version                  : 1.29
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Wed Mar  4 22:38:31 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#include "actuatorKalman.h"
#include "rtwtypes.h"

// Named constants for MATLAB Function: '<S1>/TPE_KALMAN_spd'
const real_T Ts{ 0.00025 };

// Model step function
void Kalman::step()
{
  real_T K1;
  real_T K2;
  real_T P11;
  real_T P12;
  real_T P21;
  real_T PosPrior;
  real_T dummy;
  real_T tmp;

  // Delay: '<S1>/Resettable Delay_qk' incorporates:
  //   Inport: '<Root>/Encoder'

  if (rtDW.icLoad) {
    rtDW.ResettableDelay_qk_DSTATE[0] = rtU.Encoder;
    rtDW.ResettableDelay_qk_DSTATE[1] = rtU.Encoder;
  }

  // MATLAB Function: '<S1>/TPE_KALMAN_spd' incorporates:
  //   Delay: '<S1>/Resettable Delay_qk'
  //   Inport: '<Root>/Encoder'
  //   UnitDelay: '<S1>/Unit Delay_Pk'

  PosPrior = rtDW.ResettableDelay_qk_DSTATE[1] * Ts +
    rtDW.ResettableDelay_qk_DSTATE[0];
  P12 = Ts * rtDW.UnitDelay_Pk_DSTATE[3];
  dummy = P12 + rtDW.UnitDelay_Pk_DSTATE[1];
  P11 = ((Ts * rtDW.UnitDelay_Pk_DSTATE[2] + rtDW.UnitDelay_Pk_DSTATE[0]) + Ts *
         dummy) + 1.3333333333333337E-8;
  P21 = dummy + 8.0E-5;
  P12 = (P12 + rtDW.UnitDelay_Pk_DSTATE[2]) + 8.0E-5;
  dummy = 1.0 / (P11 + 3.0174851417541504E-5);
  K1 = P11 * dummy;
  K2 = P21 * dummy;
  dummy = rtU.Encoder - PosPrior;
  PosPrior += K1 * dummy;
  dummy = K2 * dummy + rtDW.ResettableDelay_qk_DSTATE[1];

  // Outport: '<Root>/Pos_Est' incorporates:
  //   DataTypeConversion: '<Root>/Cast To Single'
  //   MATLAB Function: '<S1>/TPE_KALMAN_spd'

  rtY.Pos_Est = static_cast<real32_T>(PosPrior);

  // Outport: '<Root>/Vel_Est' incorporates:
  //   DataTypeConversion: '<Root>/Cast To Single1'
  //   MATLAB Function: '<S1>/TPE_KALMAN_spd'

  rtY.Vel_Est = static_cast<real32_T>(dummy);

  // MATLAB Function: '<S1>/TPE_KALMAN_spd' incorporates:
  //   UnitDelay: '<S1>/Unit Delay_Pk'

  tmp = rtDW.UnitDelay_Pk_DSTATE[3];

  // Update for UnitDelay: '<S1>/Unit Delay_Pk' incorporates:
  //   MATLAB Function: '<S1>/TPE_KALMAN_spd'

  rtDW.UnitDelay_Pk_DSTATE[0] = P11 - K1 * P11;
  rtDW.UnitDelay_Pk_DSTATE[2] = P12 - K1 * P12;
  rtDW.UnitDelay_Pk_DSTATE[1] = P21 - K2 * P11;
  rtDW.UnitDelay_Pk_DSTATE[3] = (tmp + 0.64) - K2 * P12;

  // Update for Delay: '<S1>/Resettable Delay_qk' incorporates:
  //   MATLAB Function: '<S1>/TPE_KALMAN_spd'

  rtDW.icLoad = false;
  rtDW.ResettableDelay_qk_DSTATE[0] = PosPrior;
  rtDW.ResettableDelay_qk_DSTATE[1] = dummy;
}

// Model initialize function
void Kalman::initialize()
{
  // InitializeConditions for UnitDelay: '<S1>/Unit Delay_Pk'
  rtDW.UnitDelay_Pk_DSTATE[0] = 0.155;
  rtDW.UnitDelay_Pk_DSTATE[1] = 65.0;
  rtDW.UnitDelay_Pk_DSTATE[2] = 65.0;
  rtDW.UnitDelay_Pk_DSTATE[3] = 57000.0;

  // InitializeConditions for Delay: '<S1>/Resettable Delay_qk'
  rtDW.icLoad = true;
}

const char_T* Kalman::RT_MODEL::getErrorStatus() const
{
  return (errorStatus);
}

void Kalman::RT_MODEL::setErrorStatus(const char_T* const volatile aErrorStatus)
{
  (errorStatus = aErrorStatus);
}

// Constructor
Kalman::Kalman() :
  rtU(),
  rtY(),
  rtDW(),
  rtM()
{
  // Currently there is no constructor body generated.
}

// Destructor
// Currently there is no destructor body generated.
Kalman::~Kalman() = default;

// Real-Time Model get method
Kalman::RT_MODEL * Kalman::getRTM()
{
  return (&rtM);
}

//
// File trailer for generated code.
//
// [EOF]
//
