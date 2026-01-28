//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: actuatorKalman.h
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
#ifndef actuatorKalman_h_
#define actuatorKalman_h_
#include <cmath>
#include "rtwtypes.h"

// Class declaration for model actuatorKalman
class Kalman final
{
  // public data and function members
 public:
  // Block signals and states (default storage) for system '<Root>/Kalman'
  struct DW_ActuatorKalman {
    real_T UnitDelay_Pk_DSTATE[4];     // '<S1>/Unit Delay_Pk'
    real_T ResettableDelay_qk_DSTATE[2];// '<S1>/Resettable Delay_qk'
    boolean_T icLoad;                  // '<S1>/Resettable Delay_qk'
  };

  // Block signals and states (default storage) for system '<Root>'
  struct DW {
    DW_ActuatorKalman Kalman_o;        // '<Root>/Kalman'
  };

  // External inputs (root inport signals with default storage)
  struct ExtU {
    real_T Encoder;                    // '<Root>/Encoder'
  };

  // External outputs (root outports fed by signals with default storage)
  struct ExtY {
    real_T Angle;                      // '<Root>/Angle'
    real_T vel;                        // '<Root>/vel'
  };

  // Real-time Model Data Structure
  struct RT_MODEL {
    const char_T * volatile errorStatus;
    const char_T* getErrorStatus() const;
    void setErrorStatus(const char_T* const volatile aErrorStatus);
  };

  // Copy Constructor
  Kalman(Kalman const&) = delete;

  // Assignment Operator
  Kalman& operator= (Kalman const&) & = delete;

  // Move Constructor
  Kalman(Kalman &&) = delete;

  // Move Assignment Operator
  Kalman& operator= (Kalman &&) = delete;

  // Real-Time Model get method
  Kalman::RT_MODEL * getRTM();

  // External inputs
  ExtU rtU;

  // External outputs
  ExtY rtY;

  // model initialize function
  void initialize();

  // model step function
  void step();

  // Constructor
  Kalman();

  // Destructor
  ~Kalman();

  // private data and function members
 private:
  // Block states
  DW rtDW;

  // private member function(s) for subsystem '<Root>/Kalman'
  static void ActuatorKalman_Init(DW_ActuatorKalman *localDW);
  static void ActuatorKalman(real_T rtu_z, real_T *rty_EncX_PosErrInc, real_T
    *rty_Vel_Est, real_T rtp_lambda_RQ, DW_ActuatorKalman *localDW);

  // Real-Time Model
  RT_MODEL rtM;
};

//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Use the MATLAB hilite_system command to trace the generated code back
//  to the model.  For example,
//
//  hilite_system('<S3>')    - opens system 3
//  hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'actuatorKalman'
//  '<S1>'   : 'actuatorKalman/Kalman'
//  '<S2>'   : 'actuatorKalman/Kalman/TPE_KALMAN_spd'

#endif                                 // actuatorKalman_h_

//
// File trailer for generated code.
//
// [EOF]
//
