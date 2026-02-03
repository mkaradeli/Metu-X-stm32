//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: actuatorKalman.h
//
// Code generated for Simulink model 'actuatorKalman'.
//
// Model version                  : 1.23
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Tue Feb  3 13:11:38 2026
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
  // Block signals and states (default storage) for system '<Root>'
  struct DW {
    real_T UnitDelay_Pk_DSTATE[4];     // '<S1>/Unit Delay_Pk'
    real_T ResettableDelay_qk_DSTATE[2];// '<S1>/Resettable Delay_qk'
    boolean_T icLoad;                  // '<S1>/Resettable Delay_qk'
  };

  // External inputs (root inport signals with default storage)
  struct ExtU {
    real_T Encoder;                    // '<Root>/Encoder'
  };

  // External outputs (root outports fed by signals with default storage)
  struct ExtY {
    real32_T Pos_Est;                  // '<Root>/Pos_Est'
    real32_T Vel_Est;                  // '<Root>/Vel_Est'
  };

  // Parameters (default storage)
  struct P {
    real_T Subsystem_lambda_RQ;        // Mask Parameter: Subsystem_lambda_RQ
                                          //  Referenced by: '<S1>/lambda_RQ'

    real_T UnitDelay_Pk_InitialCondition[4];// Expression: [0.155 65; 65 5.7e4]
                                               //  Referenced by: '<S1>/Unit Delay_Pk'

  };

  // Copy Constructor
  Kalman(Kalman const&) = delete;

  // Assignment Operator
  Kalman& operator= (Kalman const&) & = delete;

  // Move Constructor
  Kalman(Kalman &&) = delete;

  // Move Assignment Operator
  Kalman& operator= (Kalman &&) = delete;

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

  // Tunable parameters
  static P rtP;
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
//  '<S1>'   : 'actuatorKalman/Subsystem'
//  '<S2>'   : 'actuatorKalman/Subsystem/TPE_KALMAN_spd'

#endif                                 // actuatorKalman_h_

//
// File trailer for generated code.
//
// [EOF]
//
