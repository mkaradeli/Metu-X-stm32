//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: platformController_data.cpp
//
// Code generated for Simulink model 'platformController'.
//
// Model version                  : 1.77
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Sun Sep  6 18:20:35 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#include "platformController.h"

// Block parameters (default storage)
PlatformController::P PlatformController::rtP{
  // Variable: platform_targets
  //  Referenced by:
  //    '<S3>/Bias1'
  //    '<S3>/Bias2'
  //    '<S3>/Constant3'
  //    '<S3>/Constant4'
  //    '<S3>/Gain1'
  //    '<S3>/Gain2'
  //    '<S3>/Saturation2'
  //    '<S3>/Saturation3'
  //    '<S4>/Bias1'
  //    '<S4>/Constant3'
  //    '<S4>/Constant4'
  //    '<S4>/Gain1'
  //    '<S4>/Gain5'
  //    '<S4>/Saturation1'
  //    '<S4>/Saturation3'
  //    '<S6>/Constant'
  //    '<S127>/Constant'
  //    '<S127>/Gain3'
  //    '<S128>/Gain4'
  //    '<S128>/Gain5'
  //    '<S161>/Integral Gain'
  //    '<S169>/Proportional Gain'

  {
    0.1,
    0.25,
    1.0,
    9.0,
    15.0,
    5.0,
    12.0,
    0.02,
    50.0,
    2.7404379212089442,

    {
      9.8,
      32.013333333333343,
      2.3,
      1.763333333333333
    },

    {
      0.5,
      4.0,

      { -1.0, 2.2 }
    },

    {
      4.0,
      1.5,
      0.66,
      30.3,
      15.0,
      0.33,

      { 1.0, 0.0, -0.33, 0.0, 1.0, 0.0, 0.33, 0.0, 1.0, 0.33, 0.0, 0.0, 1.0,
        -0.33, 0.0, 0.0 }
    }
  },

  // Variable: mass
  //  Referenced by:
  //    '<S3>/Bias'
  //    '<S4>/Bias'

  {
    31.5,
    37.3
  },

  // Mask Parameter: PIDController_InitialConditionF
  //  Referenced by: '<S164>/Integrator'

  0.0,

  // Mask Parameter: PIDController_InitialConditio_c
  //  Referenced by: '<S108>/Integrator'

  -9.81,

  // Mask Parameter: PIDController_InitialConditio_m
  //  Referenced by: '<S49>/Integrator'

  0.0,

  // Mask Parameter: PIDController_Kt
  //  Referenced by: '<S61>/Kt'

  1.0,

  // Mask Parameter: PIDController_Kt_n
  //  Referenced by: '<S120>/Kt'

  1.0,

  // Mask Parameter: PIDController_LowerIntegratorSa
  //  Referenced by: '<S108>/Integrator'

  -10.0,

  // Mask Parameter: PIDController_LowerIntegrator_j
  //  Referenced by: '<S49>/Integrator'

  -3.0,

  // Mask Parameter: PIDController_UpperIntegratorSa
  //  Referenced by: '<S108>/Integrator'

  3.0,

  // Mask Parameter: PIDController_UpperIntegrator_h
  //  Referenced by: '<S49>/Integrator'

  3.0,

  // Mask Parameter: CompareToConstant2_const
  //  Referenced by: '<S8>/Constant'

  0.25,

  // Expression: -9.8
  //  Referenced by: '<S3>/Constant'

  -9.8,

  // Expression: 0
  //  Referenced by: '<S4>/Constant5'

  0.0,

  // Expression: 9.81
  //  Referenced by: '<S4>/Bias3'

  9.81,

  // Expression: 0
  //  Referenced by: '<S3>/Constant8'

  0.0,

  // Expression: 0
  //  Referenced by: '<S3>/Constant5'

  0.0,

  // Expression: 9.81
  //  Referenced by: '<S3>/Bias3'

  9.81,

  // Expression: 0
  //  Referenced by: '<S2>/Constant1'

  0.0,

  // Expression: 0
  //  Referenced by: '<S38>/Constant1'

  0.0,

  // Expression: 0
  //  Referenced by: '<S97>/Constant1'

  0.0,

  // Computed Parameter: Integrator_gainval
  //  Referenced by: '<S164>/Integrator'

  0.005,

  // Expression: 0
  //  Referenced by: '<S5>/Rate Transition1'

  0.0,

  // Expression: 272
  //  Referenced by: '<S128>/Saturation'

  272.0,

  // Expression: -272
  //  Referenced by: '<S128>/Saturation'

  -272.0,

  // Expression: 2.85/300
  //  Referenced by: '<S4>/Gain'

  0.0095,

  // Expression: -2
  //  Referenced by: '<S4>/Gain3'

  -2.0,

  // Expression: 1
  //  Referenced by: '<S4>/Bias5'

  1.0,

  // Expression: 1
  //  Referenced by: '<S4>/Saturation4'

  1.0,

  // Expression: 0.7
  //  Referenced by: '<S4>/Saturation4'

  0.7,

  // Expression: -9.81
  //  Referenced by: '<S4>/Bias4'

  -9.81,

  // Computed Parameter: Integrator_gainval_c
  //  Referenced by: '<S108>/Integrator'

  0.02,

  // Expression: -9.81
  //  Referenced by: '<S4>/Saturation3'

  -9.81,

  // Expression: -1
  //  Referenced by: '<S4>/Constant2'

  -1.0,

  // Expression: -9.81
  //  Referenced by: '<S4>/Constant1'

  -9.81,

  // Expression: 2.85/300
  //  Referenced by: '<S3>/Gain'

  0.0095,

  // Expression: -2
  //  Referenced by: '<S3>/Gain3'

  -2.0,

  // Expression: 1
  //  Referenced by: '<S3>/Bias5'

  1.0,

  // Expression: 1
  //  Referenced by: '<S3>/Saturation4'

  1.0,

  // Expression: 0.7
  //  Referenced by: '<S3>/Saturation4'

  0.7,

  // Expression: -9.81
  //  Referenced by: '<S3>/Bias4'

  -9.81,

  // Expression: inf
  //  Referenced by: '<S3>/Saturation1'

  rtInf,

  // Expression: 0
  //  Referenced by: '<S3>/Saturation1'

  0.0,

  // Expression: -inf
  //  Referenced by: '<S3>/Saturation2'

  rtMinusInf,

  // Computed Parameter: Integrator_gainval_cz
  //  Referenced by: '<S49>/Integrator'

  0.02,

  // Expression: -inf
  //  Referenced by: '<S3>/Saturation3'

  rtMinusInf,

  // Expression: -1
  //  Referenced by: '<S3>/Constant2'

  -1.0,

  // Expression: -9.81
  //  Referenced by: '<S3>/Constant1'

  -9.81,

  // Expression: 1/4
  //  Referenced by: '<S2>/Gain'

  0.25,

  // Expression: 272
  //  Referenced by: '<S128>/Saturation1'

  272.0,

  // Expression: -272
  //  Referenced by: '<S128>/Saturation1'

  -272.0,

  // Expression: 272
  //  Referenced by: '<S128>/Saturation2'

  272.0,

  // Expression: -272
  //  Referenced by: '<S128>/Saturation2'

  -272.0,

  // Expression: 272
  //  Referenced by: '<S128>/Saturation3'

  272.0,

  // Expression: -272
  //  Referenced by: '<S128>/Saturation3'

  -272.0,

  // Expression: -9.81
  //  Referenced by: '<S3>/Bias6'

  -9.81,

  // Expression: 0
  //  Referenced by: '<S38>/Clamping_zero'

  0.0,

  // Expression: -9.81
  //  Referenced by: '<S4>/Bias6'

  -9.81,

  // Expression: 0
  //  Referenced by: '<S97>/Clamping_zero'

  0.0,

  // Computed Parameter: Constant_Value_n
  //  Referenced by: '<S38>/Constant'

  1,

  // Computed Parameter: Constant2_Value_l
  //  Referenced by: '<S38>/Constant2'

  -1,

  // Computed Parameter: Constant3_Value
  //  Referenced by: '<S38>/Constant3'

  1,

  // Computed Parameter: Constant4_Value
  //  Referenced by: '<S38>/Constant4'

  -1,

  // Computed Parameter: Constant_Value_nl
  //  Referenced by: '<S97>/Constant'

  1,

  // Computed Parameter: Constant2_Value_o
  //  Referenced by: '<S97>/Constant2'

  -1,

  // Computed Parameter: Constant3_Value_b
  //  Referenced by: '<S97>/Constant3'

  1,

  // Computed Parameter: Constant4_Value_m
  //  Referenced by: '<S97>/Constant4'

  -1
};

//
// File trailer for generated code.
//
// [EOF]
//
