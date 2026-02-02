//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: actuatorKalman_data.cpp
//
// Code generated for Simulink model 'actuatorKalman'.
//
// Model version                  : 1.22
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Sun Feb  1 19:49:15 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: STMicroelectronics->ST10/Super10
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#include "actuatorKalman.h"

// Block parameters (default storage)
Kalman::P Kalman::rtP{
  // Mask Parameter: Subsystem_lambda_RQ
  //  Referenced by: '<S1>/lambda_RQ'

  0.01,

  // Expression: [0.155 65; 65 5.7e4]
  //  Referenced by: '<S1>/Unit Delay_Pk'

  { 0.155, 65.0, 65.0, 57000.0 }
};

//
// File trailer for generated code.
//
// [EOF]
//
