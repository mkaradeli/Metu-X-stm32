//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: platformController_data.cpp
//
// Code generated for Simulink model 'platformController'.
//
// Model version                  : 1.106
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Tue Sep 15 14:42:45 2026
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
  // Variable: mass
  //  Referenced by:
  //    '<S3>/Bias'
  //    '<S4>/Bias'

  {
    31.5,
    37.3
  },

  // Mask Parameter: PIDController_InitialConditionF
  //  Referenced by: '<S174>/Integrator'

  0.0,

  // Mask Parameter: PIDController_InitialConditio_j
  //  Referenced by: '<S110>/Integrator'

  -9.81,

  // Mask Parameter: PIDController_InitialConditi_jf
  //  Referenced by: '<S49>/Integrator'

  0.0,

  // Mask Parameter: PIDController_Kt
  //  Referenced by: '<S61>/Kt'

  1.0,

  // Mask Parameter: PIDController_Kt_b
  //  Referenced by: '<S122>/Kt'

  1.0,

  // Mask Parameter: PIDController_LowerIntegratorSa
  //  Referenced by: '<S174>/Integrator'

  -16.5,

  // Mask Parameter: PIDController_LowerIntegrator_l
  //  Referenced by: '<S110>/Integrator'

  -10.0,

  // Mask Parameter: PIDController_LowerIntegrator_n
  //  Referenced by: '<S49>/Integrator'

  -3.0,

  // Mask Parameter: PIDController_LowerSaturationLi
  //  Referenced by: '<S181>/Saturation'

  -16.5,

  // Mask Parameter: PIDController_UpperIntegratorSa
  //  Referenced by: '<S174>/Integrator'

  16.5,

  // Mask Parameter: PIDController_UpperIntegrator_c
  //  Referenced by: '<S110>/Integrator'

  3.0,

  // Mask Parameter: PIDController_UpperIntegrator_m
  //  Referenced by: '<S49>/Integrator'

  3.0,

  // Mask Parameter: PIDController_UpperSaturationLi
  //  Referenced by: '<S181>/Saturation'

  16.5,

  // Mask Parameter: Subsystem_t_close
  //  Referenced by: '<S135>/Constant'

  4.0,

  // Mask Parameter: Subsystem_t_close_e
  //  Referenced by: '<S129>/Constant'

  5.0,

  // Mask Parameter: SetResetFlipFlop_Q0
  //  Referenced by: '<S134>/Unit Delay'

  false,

  // Mask Parameter: DetectIncrease_vinit
  //  Referenced by: '<S130>/Delay Input1'

  false,

  // Expression: -9.8
  //  Referenced by: '<S3>/Constant'

  -9.8,

  // Computed Parameter: Out1_Y0
  //  Referenced by: '<S131>/Out1'

  0.0,

  // Expression: 0
  //  Referenced by: '<S4>/Constant5'

  0.0,

  // Expression: 9.81
  //  Referenced by: '<S4>/Bias3'

  9.81,

  // Expression: 0
  //  Referenced by: '<S4>/Constant6'

  0.0,

  // Expression: 0
  //  Referenced by: '<S3>/Constant5'

  0.0,

  // Expression: 9.81
  //  Referenced by: '<S3>/Bias3'

  9.81,

  // Expression: 0
  //  Referenced by: '<S3>/Constant8'

  0.0,

  // Expression: 0
  //  Referenced by: '<S2>/Constant2'

  0.0,

  // Expression: 0
  //  Referenced by: '<S2>/Constant3'

  0.0,

  // Expression: 0
  //  Referenced by: '<S38>/Constant1'

  0.0,

  // Expression: 0
  //  Referenced by: '<S99>/Constant1'

  0.0,

  // Expression: 0.52
  //  Referenced by: '<S136>/Constant'

  0.52,

  // Expression: -0.52
  //  Referenced by: '<S136>/Constant1'

  -0.52,

  // Computed Parameter: Integrator_gainval
  //  Referenced by: '<S174>/Integrator'

  0.005,

  // Expression: 0
  //  Referenced by: '<S5>/Rate Transition1'

  0.0,

  // Expression: 272
  //  Referenced by: '<S137>/Saturation'

  272.0,

  // Expression: -272
  //  Referenced by: '<S137>/Saturation'

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

  // Computed Parameter: DiscreteTimeIntegrator_gainval
  //  Referenced by: '<S133>/Discrete-Time Integrator'

  0.02,

  // Expression: 0
  //  Referenced by: '<S133>/Discrete-Time Integrator'

  0.0,

  // Computed Parameter: Integrator_gainval_l
  //  Referenced by: '<S110>/Integrator'

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

  // Computed Parameter: DiscreteTimeIntegrator_gainva_o
  //  Referenced by: '<S71>/Discrete-Time Integrator'

  0.02,

  // Expression: 0
  //  Referenced by: '<S71>/Discrete-Time Integrator'

  0.0,

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

  // Computed Parameter: Integrator_gainval_n
  //  Referenced by: '<S49>/Integrator'

  0.02,

  // Expression: -inf
  //  Referenced by: '<S3>/Saturation3'

  rtMinusInf,

  // Expression: -0.9
  //  Referenced by: '<S3>/Constant2'

  -0.9,

  // Expression: -9.81
  //  Referenced by: '<S3>/Constant1'

  -9.81,

  // Expression: 0
  //  Referenced by: '<S2>/Constant1'

  0.0,

  // Expression: 1/4
  //  Referenced by: '<S2>/Gain'

  0.25,

  // Expression: 272
  //  Referenced by: '<S137>/Saturation1'

  272.0,

  // Expression: -272
  //  Referenced by: '<S137>/Saturation1'

  -272.0,

  // Expression: 272
  //  Referenced by: '<S137>/Saturation2'

  272.0,

  // Expression: -272
  //  Referenced by: '<S137>/Saturation2'

  -272.0,

  // Expression: 272
  //  Referenced by: '<S137>/Saturation3'

  272.0,

  // Expression: -272
  //  Referenced by: '<S137>/Saturation3'

  -272.0,

  // Expression: 1/4
  //  Referenced by: '<S2>/Gain1'

  0.25,

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
  //  Referenced by: '<S99>/Clamping_zero'

  0.0,

  // Expression: 1
  //  Referenced by: '<S71>/Constant'

  1.0,

  // Expression: 1
  //  Referenced by: '<S133>/Constant'

  1.0,

  // Expression: isSetWhenUndefinedState
  //  Referenced by: '<S134>/Constant'

  true,

  // Computed Parameter: Constant_Value_cz
  //  Referenced by: '<S38>/Constant'

  1,

  // Computed Parameter: Constant2_Value_o
  //  Referenced by: '<S38>/Constant2'

  -1,

  // Computed Parameter: Constant3_Value_p
  //  Referenced by: '<S38>/Constant3'

  1,

  // Computed Parameter: Constant4_Value
  //  Referenced by: '<S38>/Constant4'

  -1,

  // Computed Parameter: Constant_Value_d
  //  Referenced by: '<S99>/Constant'

  1,

  // Computed Parameter: Constant2_Value_a
  //  Referenced by: '<S99>/Constant2'

  -1,

  // Computed Parameter: Constant3_Value_j
  //  Referenced by: '<S99>/Constant3'

  1,

  // Computed Parameter: Constant4_Value_l
  //  Referenced by: '<S99>/Constant4'

  -1
};

//
// File trailer for generated code.
//
// [EOF]
//
