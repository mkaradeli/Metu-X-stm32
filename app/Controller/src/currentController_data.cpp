//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: currentController_data.cpp
//
// Code generated for Simulink model 'currentController'.
//
// Model version                  : 1.77
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Tue Feb  3 13:11:00 2026
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
  // Mask Parameter: ChatGPT_ICPrevInput
  //  Referenced by: '<S2>/Delay Input'

  0.0F,

  // Mask Parameter: GORKEM_ICPrevInput
  //  Referenced by: '<S3>/Delay Input'

  0.0F,

  // Mask Parameter: ChatGPT_ICPrevOutput
  //  Referenced by: '<S2>/Delay Output'

  0.0F,

  // Mask Parameter: GORKEM_ICPrevOutput
  //  Referenced by: '<S3>/Delay Output'

  0.0F,

  // Mask Parameter: ChatGPT_PoleZ
  //  Referenced by: '<S2>/GainPole'

  0.9999F,

  // Mask Parameter: GORKEM_PoleZ
  //  Referenced by: '<S3>/GainPole'

  0.819F,

  // Mask Parameter: ChatGPT_ZeroZ
  //  Referenced by: '<S2>/GainZero'

  0.522F,

  // Mask Parameter: GORKEM_ZeroZ
  //  Referenced by: '<S3>/GainZero'

  0.00345426681F,

  // Computed Parameter: Duty_Y0
  //  Referenced by: '<S1>/Duty'

  0.0F,

  // Computed Parameter: Gain5_Gain
  //  Referenced by: '<S1>/Gain5'

  5.1125F,

  // Computed Parameter: Gain2_Gain
  //  Referenced by: '<S1>/Gain2'

  0.0416666679F,

  // Computed Parameter: Saturation_UpperSat
  //  Referenced by: '<S2>/Saturation'

  1.0F,

  // Computed Parameter: Saturation_LowerSat
  //  Referenced by: '<S2>/Saturation'

  -1.0F,

  // Computed Parameter: Gain1_Gain
  //  Referenced by: '<S1>/Gain1'

  1.957F,

  // Computed Parameter: Gain4_Gain
  //  Referenced by: '<S1>/Gain4'

  0.0416666679F,

  // Computed Parameter: Saturation_UpperSat_l
  //  Referenced by: '<S3>/Saturation'

  1.0F,

  // Computed Parameter: Saturation_LowerSat_j
  //  Referenced by: '<S3>/Saturation'

  -1.0F,

  // Computed Parameter: ManualSwitch_CurrentSetting
  //  Referenced by: '<S1>/Manual Switch'

  0U
};

//
// File trailer for generated code.
//
// [EOF]
//
