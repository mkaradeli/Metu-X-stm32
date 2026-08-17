//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: actuatorController_data.cpp
//
// Code generated for Simulink model 'actuatorController'.
//
// Model version                  : 1.73
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Mon Aug 17 23:22:53 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#include "actuatorController.h"

// Block parameters (default storage)
controller::actuatorController::P controller::actuatorController::rtP{
  // Mask Parameter: PIDController_InitialConditionF
  //  Referenced by: '<S53>/Integrator'

  0.0,

  // Mask Parameter: PIDController_Kt
  //  Referenced by: '<S64>/Kt'

  100.0,

  // Mask Parameter: CompareToConstant3_const
  //  Referenced by: '<S6>/Constant'

  controller_modes::SPEED,

  // Mask Parameter: CompareToConstant1_const
  //  Referenced by: '<S4>/Constant'

  controller_modes::POSITION,

  // Mask Parameter: CompareToConstant_const
  //  Referenced by: '<S3>/Constant'

  controller_modes::SPEED,

  // Mask Parameter: CompareToConstant4_const
  //  Referenced by: '<S7>/Constant'

  controller_modes::POSITION,

  // Mask Parameter: CompareToConstant6_const
  //  Referenced by: '<S9>/Constant'

  controller_modes::PRESSURE,

  // Mask Parameter: CompareToConstant5_const
  //  Referenced by: '<S8>/Constant'

  controller_modes::FORCE,

  // Mask Parameter: CompareToConstant2_const
  //  Referenced by: '<S5>/Constant'

  controller_modes::PRESSURE,

  // Mask Parameter: DiscreteDerivative_ICPrevScaled
  //  Referenced by: '<S71>/UD'

  0.0F,

  // Mask Parameter: CompareToConstant_const_p
  //  Referenced by: '<S16>/Constant'

  -500.0F,

  // Expression: 0
  //  Referenced by: '<S2>/Constant'

  0.0,

  // Expression: 0
  //  Referenced by: '<S2>/Constant1'

  0.0,

  // Computed Parameter: Theta_Y0
  //  Referenced by: '<S2>/Theta'

  0.0,

  // Computed Parameter: Integrator_gainval
  //  Referenced by: '<S53>/Integrator'

  0.001,

  // Expression: 1200
  //  Referenced by: '<S2>/Saturation3'

  1200.0,

  // Expression: 0
  //  Referenced by: '<S2>/Saturation3'

  0.0,

  // Expression: 2000
  //  Referenced by: '<S2>/Rate Limiter'

  2000.0,

  // Expression: -2000
  //  Referenced by: '<S2>/Rate Limiter'

  -2000.0,

  // Expression: 0
  //  Referenced by: '<S2>/Rate Limiter'

  0.0,

  // Computed Parameter: Gain_Gain
  //  Referenced by: '<S2>/Gain'

  0.9F,

  // Computed Parameter: Saturation_UpperSat
  //  Referenced by: '<S2>/Saturation'

  5000.0F,

  // Computed Parameter: Saturation_LowerSat
  //  Referenced by: '<S2>/Saturation'

  15.0F,

  // Computed Parameter: Saturation1_UpperSat
  //  Referenced by: '<S2>/Saturation1'

  5000.0F,

  // Computed Parameter: Saturation1_LowerSat
  //  Referenced by: '<S2>/Saturation1'

  15.0F,

  // Computed Parameter: uDLookupTable_tableData
  //  Referenced by: '<S2>/1-D Lookup Table'

  { 0.0F, 100.0F, 200.0F, 300.0F, 400.0F, 500.0F, 600.0F, 800.0F, 900.0F,
    1100.0F, 1200.0F },

  // Computed Parameter: SpeedDemand_Y0
  //  Referenced by: '<S12>/SpeedDemand'

  0.0F,

  // Computed Parameter: TSamp_WtEt
  //  Referenced by: '<S71>/TSamp'

  1000.0F,

  // Computed Parameter: Y_Y0
  //  Referenced by: '<S13>/Y'

  0.0F,

  // Computed Parameter: DiscreteTimeIntegrator_gainval
  //  Referenced by: '<S13>/Discrete-Time Integrator'

  0.001F,

  // Computed Parameter: DiscreteTimeIntegrator_IC
  //  Referenced by: '<S13>/Discrete-Time Integrator'

  0.0F,

  // Computed Parameter: RateLimiter_IC_o
  //  Referenced by: '<S13>/Rate Limiter'

  0.0F,

  // Computed Parameter: P_nozzle_demand_Y0
  //  Referenced by: '<S14>/P_nozzle_demand'

  0.0F,

  // Computed Parameter: Saturation1_UpperSat_g
  //  Referenced by: '<S14>/Saturation1'

  rtInfF,

  // Computed Parameter: Saturation1_LowerSat_h
  //  Referenced by: '<S14>/Saturation1'

  0.0F,

  // Computed Parameter: Saturation_UpperSat_p
  //  Referenced by: '<S14>/Saturation'

  rtInfF,

  // Computed Parameter: Saturation_LowerSat_k
  //  Referenced by: '<S14>/Saturation'

  0.0F,

  // Computed Parameter: DiscreteTimeIntegrator_gainva_l
  //  Referenced by: '<S72>/Discrete-Time Integrator'

  0.001F,

  // Computed Parameter: DiscreteTimeIntegrator_IC_c
  //  Referenced by: '<S72>/Discrete-Time Integrator'

  0.0F,

  // Computed Parameter: Gain1_Gain
  //  Referenced by: '<S10>/Gain1'

  0.0174532924F,

  // Computed Parameter: Gain1_Gain_p
  //  Referenced by: '<S11>/Gain1'

  0.0174532924F,

  // Computed Parameter: Saturation1_UpperSat_k
  //  Referenced by: '<S15>/Saturation1'

  1440.0F,

  // Computed Parameter: Saturation1_LowerSat_k
  //  Referenced by: '<S15>/Saturation1'

  0.0F,

  // Computed Parameter: K_Gain
  //  Referenced by: '<S72>/K'

  1.0F,

  // Computed Parameter: Gain_Gain_h
  //  Referenced by: '<S72>/Gain'

  188.49556F,

  // Computed Parameter: Saturation_UpperSat_k
  //  Referenced by: '<S72>/Saturation'

  2400.0F,

  // Computed Parameter: Saturation_LowerSat_kk
  //  Referenced by: '<S72>/Saturation'

  -2400.0F,

  // Computed Parameter: Gain_Gain_j
  //  Referenced by: '<S1>/Gain'

  0.8F
};

//
// File trailer for generated code.
//
// [EOF]
//
