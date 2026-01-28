//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: actuatorKalman.cpp
//
// Code generated for Simulink model 'actuatorKalman'.
//
// Model version                  : 1.13
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Mon Jan 12 22:24:02 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: STMicroelectronics->ST10/Super10
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#include "actuatorKalman.h"
#include "rtwtypes.h"

// Named constants for MATLAB Function: '<S1>/TPE_KALMAN_spd'
const real_T Ts{ 0.000249003984063745 };

// System initialize for atomic system: '<Root>/Kalman'
void Kalman::ActuatorKalman_Init(DW_ActuatorKalman *localDW)
{
  // InitializeConditions for UnitDelay: '<S1>/Unit Delay_Pk'
  localDW->UnitDelay_Pk_DSTATE[0] = 0.155;
  localDW->UnitDelay_Pk_DSTATE[1] = 65.0;
  localDW->UnitDelay_Pk_DSTATE[2] = 65.0;
  localDW->UnitDelay_Pk_DSTATE[3] = 57000.0;

  // InitializeConditions for Delay: '<S1>/Resettable Delay_qk'
  localDW->icLoad = true;
}

// Output and update for atomic system: '<Root>/Kalman'
void Kalman::ActuatorKalman(real_T rtu_z, real_T *rty_EncX_PosErrInc, real_T
  *rty_Vel_Est, real_T rtp_lambda_RQ, DW_ActuatorKalman *localDW)
{
  real_T K1;
  real_T K2;
  real_T P11;
  real_T P12;
  real_T P21;
  real_T PosPrior;
  real_T dummy;
  real_T qk_idx_1;

  // Delay: '<S1>/Resettable Delay_qk'
  if (localDW->icLoad) {
    localDW->ResettableDelay_qk_DSTATE[0] = rtu_z;
    localDW->ResettableDelay_qk_DSTATE[1] = rtu_z;
  }

  // MATLAB Function: '<S1>/TPE_KALMAN_spd' incorporates:
  //   Constant: '<S1>/lambda_RQ'
  //   Delay: '<S1>/Resettable Delay_qk'
  //   UnitDelay: '<S1>/Unit Delay_Pk'

  PosPrior = localDW->ResettableDelay_qk_DSTATE[1] * Ts +
    localDW->ResettableDelay_qk_DSTATE[0];
  P12 = Ts * localDW->UnitDelay_Pk_DSTATE[3];
  dummy = P12 + localDW->UnitDelay_Pk_DSTATE[1];
  P11 = ((Ts * localDW->UnitDelay_Pk_DSTATE[2] + localDW->UnitDelay_Pk_DSTATE[0])
         + Ts * dummy) + rtp_lambda_RQ * 1.3280212483399732E-6;
  K1 = rtp_lambda_RQ * 0.008;
  P21 = K1 + dummy;
  P12 = (P12 + localDW->UnitDelay_Pk_DSTATE[2]) + K1;
  dummy = 1.0 / (P11 + 0.0064);
  K1 = P11 * dummy;
  K2 = P21 * dummy;
  dummy = rtu_z - PosPrior;
  PosPrior += K1 * dummy;
  qk_idx_1 = K2 * dummy + localDW->ResettableDelay_qk_DSTATE[1];
  *rty_Vel_Est = qk_idx_1;
  *rty_EncX_PosErrInc = PosPrior;
  dummy = localDW->UnitDelay_Pk_DSTATE[3];

  // Update for UnitDelay: '<S1>/Unit Delay_Pk' incorporates:
  //   Constant: '<S1>/lambda_RQ'
  //   MATLAB Function: '<S1>/TPE_KALMAN_spd'

  localDW->UnitDelay_Pk_DSTATE[0] = P11 - K1 * P11;
  localDW->UnitDelay_Pk_DSTATE[2] = P12 - K1 * P12;
  localDW->UnitDelay_Pk_DSTATE[1] = P21 - K2 * P11;
  localDW->UnitDelay_Pk_DSTATE[3] = (rtp_lambda_RQ * 64.256 + dummy) - K2 * P12;

  // Update for Delay: '<S1>/Resettable Delay_qk' incorporates:
  //   MATLAB Function: '<S1>/TPE_KALMAN_spd'

  localDW->icLoad = false;
  localDW->ResettableDelay_qk_DSTATE[0] = PosPrior;
  localDW->ResettableDelay_qk_DSTATE[1] = qk_idx_1;
}

// Model step function
void Kalman::step()
{
  // Outputs for Atomic SubSystem: '<Root>/Kalman'

  // Inport: '<Root>/Encoder' incorporates:
  //   Outport: '<Root>/Angle'
  //   Outport: '<Root>/vel'

  ActuatorKalman(rtU.Encoder, &rtY.Angle, &rtY.vel, 0.05, &rtDW.Kalman_o);

  // End of Outputs for SubSystem: '<Root>/Kalman'
}

// Model initialize function
void Kalman::initialize()
{
  // SystemInitialize for Atomic SubSystem: '<Root>/Kalman'
  ActuatorKalman_Init(&rtDW.Kalman_o);

  // End of SystemInitialize for SubSystem: '<Root>/Kalman'
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
