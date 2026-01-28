//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: currentController_data.cpp
//
// Code generated for Simulink model 'currentController'.
//
// Model version                  : 1.19
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Wed Jan 28 20:30:42 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#include "currentController.h"

// Block parameters (default storage)
CurrentController::P CurrentController::rtP{
  // Computed Parameter: debug_Y0
  //  Referenced by: '<S1>/debug'

  {
    0.0F,                              // Up
    0.0F                               // Ui
  },

  // Computed Parameter: Y_Y0
  //  Referenced by: '<S1>/Y'

  0.0F,

  // Computed Parameter: DiscreteTimeIntegrator_gainval
  //  Referenced by: '<S1>/Discrete-Time Integrator'

  0.000125F,

  // Computed Parameter: DiscreteTimeIntegrator_IC
  //  Referenced by: '<S1>/Discrete-Time Integrator'

  0.0F
};

//
// File trailer for generated code.
//
// [EOF]
//
