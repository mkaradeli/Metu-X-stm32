//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: platformController.cpp
//
// Code generated for Simulink model 'platformController'.
//
// Model version                  : 1.87
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Sat Sep 12 11:43:14 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#include "platformController.h"
#include <cmath>
#include "rtwtypes.h"

// Exported block parameters
struct_V02IQCW8z51sc9ktARnSeC platform_targets{
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
    3.0,
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

    { 1.0, 0.0, -0.33, 0.0, 1.0, 0.0, 0.33, 0.0, 1.0, 0.33, 0.0, 0.0, 1.0, -0.33,
      0.0, 0.0 }
  }
} ;                                    // Variable: platform_targets
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
                                          //    '<S127>/Gain3'
                                          //    '<S128>/Gain4'
                                          //    '<S128>/Gain5'
                                          //    '<S157>/Kb'
                                          //    '<S162>/Integral Gain'
                                          //    '<S170>/Proportional Gain'


mission_modes mission_mode{ mission_modes::DISABLE };// Variable: mission_mode
                                                        //  Referenced by: '<S2>/Constant'


static void rate_scheduler(PlatformController::RT_MODEL *const rtM);

//
//         This function updates active task flag for each subrate.
//         The function is called at model base rate, hence the
//         generated code self-manages all its subrates.
//
static void rate_scheduler(PlatformController::RT_MODEL *const rtM)
{
  // Compute which subrates run during the next base time step.  Subrates
  //  are an integer multiple of the base rate counter.  Therefore, the subtask
  //  counter is reset when it reaches its limit (zero means run).

  (rtM->Timing.TaskCounters.TID[1])++;
  if ((rtM->Timing.TaskCounters.TID[1]) > 4) {// Sample time: [0.005s, 0.0s]
    rtM->Timing.TaskCounters.TID[1] = 0;
  }

  (rtM->Timing.TaskCounters.TID[2])++;
  if ((rtM->Timing.TaskCounters.TID[2]) > 9) {// Sample time: [0.01s, 0.0s]
    rtM->Timing.TaskCounters.TID[2] = 0;
  }

  (rtM->Timing.TaskCounters.TID[3])++;
  if ((rtM->Timing.TaskCounters.TID[3]) > 19) {// Sample time: [0.02s, 0.0s]
    rtM->Timing.TaskCounters.TID[3] = 0;
  }
}

// Model step function
void PlatformController::step()
{
  real_T rtb_Bias;
  real_T rtb_Bias_e;
  real_T rtb_Diff;
  real_T rtb_Diff_j;
  real_T rtb_MathFunction;
  real_T rtb_MathFunction_n;
  real_T rtb_PProdOut;
  real_T rtb_RateTransition;
  real_T rtb_RateTransition2;
  real_T rtb_RateTransition3;
  real_T rtb_RateTransition4;
  real_T rtb_RateTransition6_n_idx_0;
  real_T rtb_RateTransition6_n_idx_1;
  real_T rtb_Saturation1;
  real_T rtb_Saturation2;
  real_T rtb_Saturation3;
  real_T rtb_Saturation_idx_0;
  real_T rtb_Saturation_idx_1;
  real_T rtb_Sqrt;
  real_T rtb_Sum;
  real_T rtb_Sum_d;
  real_T rtb_Sum_h_0;
  real_T rtb_Sum_h_idx_0;
  real_T rtb_Sum_h_idx_1;
  real_T rtb_Sum_h_idx_2;
  real_T rtb_Sum_m;
  real_T rtb_Sum_o;
  real_T rtb_Switch2_f;
  real_T rtb_Switch2_k;
  real_T rtb_Switch_he_idx_0;
  real_T rtb_Switch_he_idx_1;
  real_T rtb_Switch_he_idx_2;
  real_T rtb_UnaryMinus_l;
  real_T u0;
  int8_T tmp_2;
  int8_T tmp_3;
  boolean_T rtb_NOT;
  boolean_T tmp;
  boolean_T tmp_0;
  boolean_T tmp_1;

  // RateTransition: '<S5>/Rate Transition6' incorporates:
  //   Inport: '<Root>/quaternion'
  //   RateTransition: '<S2>/Rate Transition1'
  //   RateTransition: '<S2>/Rate Transition2'
  //   RateTransition: '<S2>/Rate Transition3'
  //   RateTransition: '<S2>/Rate Transition4'
  //   RateTransition: '<S2>/Rate Transition5'
  //   RateTransition: '<S2>/Rate Transition6'
  //   RateTransition: '<S5>/Rate Transition1'
  //
  tmp = ((&rtM)->Timing.TaskCounters.TID[1] == 0);
  if (tmp && ((&rtM)->Timing.TaskCounters.TID[2] == 0)) {
    rtDW.RateTransition6_Buffer[0] = rtU.quaternion[0];
    rtDW.RateTransition6_Buffer[1] = rtU.quaternion[1];
    rtDW.RateTransition6_Buffer[2] = rtU.quaternion[2];
    rtDW.RateTransition6_Buffer[3] = rtU.quaternion[3];
  }

  tmp_0 = ((&rtM)->Timing.TaskCounters.TID[2] == 0);
  if (tmp_0) {
    // Gain: '<S127>/Gain3' incorporates:
    //   Inport: '<Root>/quaternion_bias'
    //   Sum: '<S127>/Sum'
    //   UnaryMinus: '<S127>/Unary Minus'

    rtb_Switch_he_idx_2 = -(rtDW.RateTransition6_Buffer[0] -
      rtU.quaternion_bias[0]) * platform_targets.attitude.Kp_att;

    // Switch: '<S129>/Switch2' incorporates:
    //   Constant: '<S127>/Constant'
    //   Constant: '<S127>/Constant1'
    //   RelationalOperator: '<S129>/LowerRelop1'
    //   RelationalOperator: '<S129>/UpperRelop'
    //   Switch: '<S129>/Switch'

    if (rtb_Switch_he_idx_2 > 0.52) {
      rtb_Switch_he_idx_2 = 0.52;
    } else if (rtb_Switch_he_idx_2 < -0.52) {
      // Switch: '<S129>/Switch' incorporates:
      //   Constant: '<S127>/Constant1'

      rtb_Switch_he_idx_2 = -0.52;
    }

    rtb_Switch_he_idx_0 = rtb_Switch_he_idx_2;

    // Outport: '<Root>/omega_demand'
    rtY.omega_demand[0] = rtb_Switch_he_idx_2;

    // Gain: '<S127>/Gain3' incorporates:
    //   Inport: '<Root>/quaternion_bias'
    //   Sum: '<S127>/Sum'
    //   UnaryMinus: '<S127>/Unary Minus'

    rtb_Switch_he_idx_2 = -(rtDW.RateTransition6_Buffer[1] -
      rtU.quaternion_bias[1]) * platform_targets.attitude.Kp_att;

    // Switch: '<S129>/Switch2' incorporates:
    //   Constant: '<S127>/Constant'
    //   Constant: '<S127>/Constant1'
    //   RelationalOperator: '<S129>/LowerRelop1'
    //   RelationalOperator: '<S129>/UpperRelop'
    //   Switch: '<S129>/Switch'

    if (rtb_Switch_he_idx_2 > 0.52) {
      rtb_Switch_he_idx_2 = 0.52;
    } else if (rtb_Switch_he_idx_2 < -0.52) {
      // Switch: '<S129>/Switch' incorporates:
      //   Constant: '<S127>/Constant1'

      rtb_Switch_he_idx_2 = -0.52;
    }

    rtb_Switch_he_idx_1 = rtb_Switch_he_idx_2;

    // Outport: '<Root>/omega_demand'
    rtY.omega_demand[1] = rtb_Switch_he_idx_2;

    // Gain: '<S127>/Gain3' incorporates:
    //   Inport: '<Root>/quaternion_bias'
    //   Sum: '<S127>/Sum'
    //   UnaryMinus: '<S127>/Unary Minus'

    rtb_Switch_he_idx_2 = -(rtDW.RateTransition6_Buffer[2] -
      rtU.quaternion_bias[2]) * platform_targets.attitude.Kp_att;

    // Switch: '<S129>/Switch2' incorporates:
    //   Constant: '<S127>/Constant'
    //   Constant: '<S127>/Constant1'
    //   RelationalOperator: '<S129>/LowerRelop1'
    //   RelationalOperator: '<S129>/UpperRelop'
    //   Switch: '<S129>/Switch'

    if (rtb_Switch_he_idx_2 > 0.52) {
      rtb_Switch_he_idx_2 = 0.52;
    } else if (rtb_Switch_he_idx_2 < -0.52) {
      // Switch: '<S129>/Switch' incorporates:
      //   Constant: '<S127>/Constant1'

      rtb_Switch_he_idx_2 = -0.52;
    }

    // Outport: '<Root>/omega_demand'
    rtY.omega_demand[2] = rtb_Switch_he_idx_2;
  }

  // End of RateTransition: '<S5>/Rate Transition6'
  if (tmp) {
    // DiscreteIntegrator: '<S165>/Integrator' incorporates:
    //   Inport: '<Root>/Dropped'

    if (rtU.Dropped && (rtDW.Integrator_PrevResetState <= 0)) {
      rtDW.Integrator_DSTATE[0] = 0.0;
      rtDW.Integrator_DSTATE[1] = 0.0;
      rtDW.Integrator_DSTATE[2] = 0.0;
    }

    // RateTransition: '<S5>/Rate Transition1'
    if (tmp_0) {
      // RateTransition: '<S5>/Rate Transition1'
      rtDW.RateTransition1[0] = rtDW.RateTransition1_Buffer0[0];
      rtDW.RateTransition1[1] = rtDW.RateTransition1_Buffer0[1];
      rtDW.RateTransition1[2] = rtDW.RateTransition1_Buffer0[2];
    }

    // Gain: '<S157>/Kb'
    rtb_Saturation2 = platform_targets.attitude.Ki_rate /
      platform_targets.attitude.Kp_rate;

    // Sum: '<S128>/Subtract3' incorporates:
    //   Inport: '<Root>/gyro'

    rtb_Sum_h_idx_2 = rtDW.RateTransition1[0] - rtU.gyro[0];

    // Sum: '<S174>/Sum' incorporates:
    //   DiscreteIntegrator: '<S165>/Integrator'
    //   Gain: '<S170>/Proportional Gain'

    rtb_Sum_h_0 = platform_targets.attitude.Kp_rate * rtb_Sum_h_idx_2 +
      rtDW.Integrator_DSTATE[0];

    // Saturate: '<S172>/Saturation'
    if (rtb_Sum_h_0 > 16.5) {
      rtb_Saturation_idx_1 = 16.5;
    } else if (rtb_Sum_h_0 < -16.5) {
      rtb_Saturation_idx_1 = -16.5;
    } else {
      rtb_Saturation_idx_1 = rtb_Sum_h_0;
    }

    rtb_Saturation_idx_0 = rtb_Saturation_idx_1;

    // Sum: '<S157>/SumI4' incorporates:
    //   Gain: '<S157>/Kb'
    //   Gain: '<S162>/Integral Gain'
    //   Sum: '<S157>/SumI2'

    rtb_Sum_h_idx_0 = (rtb_Saturation_idx_1 - rtb_Sum_h_0) * rtb_Saturation2 +
      platform_targets.attitude.Ki_rate * rtb_Sum_h_idx_2;

    // Sum: '<S128>/Subtract3' incorporates:
    //   Inport: '<Root>/gyro'

    rtb_Sum_h_idx_2 = rtDW.RateTransition1[1] - rtU.gyro[1];

    // Sum: '<S174>/Sum' incorporates:
    //   DiscreteIntegrator: '<S165>/Integrator'
    //   Gain: '<S170>/Proportional Gain'

    rtb_Sum_h_0 = platform_targets.attitude.Kp_rate * rtb_Sum_h_idx_2 +
      rtDW.Integrator_DSTATE[1];

    // Saturate: '<S172>/Saturation'
    if (rtb_Sum_h_0 > 16.5) {
      rtb_Saturation_idx_1 = 16.5;
    } else if (rtb_Sum_h_0 < -16.5) {
      rtb_Saturation_idx_1 = -16.5;
    } else {
      rtb_Saturation_idx_1 = rtb_Sum_h_0;
    }

    // Sum: '<S157>/SumI4' incorporates:
    //   Gain: '<S157>/Kb'
    //   Gain: '<S162>/Integral Gain'
    //   Sum: '<S157>/SumI2'

    rtb_Sum_h_idx_1 = (rtb_Saturation_idx_1 - rtb_Sum_h_0) * rtb_Saturation2 +
      platform_targets.attitude.Ki_rate * rtb_Sum_h_idx_2;

    // Sum: '<S128>/Subtract3' incorporates:
    //   Inport: '<Root>/gyro'

    rtb_Sum_h_idx_2 = rtDW.RateTransition1[2] - rtU.gyro[2];

    // Sum: '<S174>/Sum' incorporates:
    //   DiscreteIntegrator: '<S165>/Integrator'
    //   Gain: '<S170>/Proportional Gain'

    rtb_Sum_h_0 = platform_targets.attitude.Kp_rate * rtb_Sum_h_idx_2 +
      rtDW.Integrator_DSTATE[2];

    // Saturate: '<S172>/Saturation'
    if (rtb_Sum_h_0 > 16.5) {
      u0 = 16.5;
    } else if (rtb_Sum_h_0 < -16.5) {
      u0 = -16.5;
    } else {
      u0 = rtb_Sum_h_0;
    }

    // Sum: '<S157>/SumI4' incorporates:
    //   Gain: '<S157>/Kb'
    //   Gain: '<S162>/Integral Gain'
    //   Saturate: '<S172>/Saturation'
    //   Sum: '<S157>/SumI2'

    rtb_Sum_h_idx_2 = (u0 - rtb_Sum_h_0) * rtb_Saturation2 +
      platform_targets.attitude.Ki_rate * rtb_Sum_h_idx_2;

    // Gain: '<S128>/Gain4'
    rtb_Saturation2 = 1.0 / (2.0 * platform_targets.attitude.r) *
      rtb_Saturation_idx_0;

    // Saturate: '<S128>/Saturation'
    if (rtb_Saturation2 > 272.0) {
      rtb_Saturation_idx_0 = 272.0;
    } else if (rtb_Saturation2 < -272.0) {
      rtb_Saturation_idx_0 = -272.0;
    } else {
      rtb_Saturation_idx_0 = rtb_Saturation2;
    }

    // End of Saturate: '<S128>/Saturation'

    // RateTransition: '<S2>/Rate Transition' incorporates:
    //   Inport: '<Root>/T_max_allowed'

    if ((&rtM)->Timing.TaskCounters.TID[3] == 0) {
      rtDW.RateTransition_Buffer = rtU.T_max_allowed;
    }
  }

  // RateTransition: '<S2>/Rate Transition' incorporates:
  //   RateTransition: '<S2>/Rate Transition1'
  //   RateTransition: '<S2>/Rate Transition2'
  //   RateTransition: '<S2>/Rate Transition3'
  //   RateTransition: '<S2>/Rate Transition4'
  //   RateTransition: '<S2>/Rate Transition5'
  //   RateTransition: '<S2>/Rate Transition6'

  tmp_1 = ((&rtM)->Timing.TaskCounters.TID[3] == 0);
  if (tmp_1) {
    rtb_RateTransition = rtDW.RateTransition_Buffer;
  }

  // RateTransition: '<S2>/Rate Transition4' incorporates:
  //   Inport: '<Root>/ManifoldPressure'

  if (tmp && tmp_1) {
    rtDW.RateTransition4_Buffer = rtU.ManifoldPressure;
  }

  if (tmp_1) {
    rtb_RateTransition4 = rtDW.RateTransition4_Buffer;

    // Bias: '<S4>/Bias' incorporates:
    //   Gain: '<S4>/Gain'

    rtb_Bias = 0.0095 * rtDW.RateTransition4_Buffer + 31.5;
  }

  // RateTransition: '<S2>/Rate Transition6' incorporates:
  //   Inport: '<Root>/quaternion'

  if (tmp && tmp_1) {
    rtDW.RateTransition6_Buffer_n[0] = rtU.quaternion[0];
    rtDW.RateTransition6_Buffer_n[1] = rtU.quaternion[1];
    rtDW.RateTransition6_Buffer_n[2] = rtU.quaternion[2];
    rtDW.RateTransition6_Buffer_n[3] = rtU.quaternion[3];
  }

  if (tmp_1) {
    rtb_RateTransition6_n_idx_0 = rtDW.RateTransition6_Buffer_n[0];
    rtb_RateTransition6_n_idx_1 = rtDW.RateTransition6_Buffer_n[1];

    // Math: '<S4>/Square'
    rtb_Sum_o = rtDW.RateTransition6_Buffer_n[0] *
      rtDW.RateTransition6_Buffer_n[0];

    // Bias: '<S4>/Bias5' incorporates:
    //   Gain: '<S4>/Gain3'
    //   Math: '<S4>/Square1'
    //   Sum: '<S4>/Sum1'

    u0 = (rtDW.RateTransition6_Buffer_n[1] * rtDW.RateTransition6_Buffer_n[1] +
          rtb_Sum_o) * -2.0 + 1.0;

    // Saturate: '<S4>/Saturation4'
    if (u0 < 0.7) {
      u0 = 0.7;
    }

    // Math: '<S4>/Math Function' incorporates:
    //   Saturate: '<S4>/Saturation4'
    //
    //  About '<S4>/Math Function':
    //   Operator: reciprocal

    rtb_MathFunction = 1.0 / u0;

    // Bias: '<S4>/Bias4' incorporates:
    //   Product: '<S4>/Product2'

    rtb_Diff = rtb_RateTransition / rtb_Bias / rtb_MathFunction - 9.81;
  }

  // RateTransition: '<S2>/Rate Transition3' incorporates:
  //   Inport: '<Root>/Velocity'

  if (tmp && tmp_1) {
    rtDW.RateTransition3_Buffer = rtU.Velocity;
  }

  if (tmp_1) {
    rtb_RateTransition3 = rtDW.RateTransition3_Buffer;
  }

  // RateTransition: '<S2>/Rate Transition2' incorporates:
  //   Inport: '<Root>/Height'

  if (tmp && tmp_1) {
    rtDW.RateTransition2_Buffer = rtU.Height;
  }

  if (tmp_1) {
    rtb_RateTransition2 = rtDW.RateTransition2_Buffer;

    // Gain: '<S4>/Gain5' incorporates:
    //   Bias: '<S4>/Bias1'
    //   UnaryMinus: '<S4>/Unary Minus1'

    rtb_Saturation1 = (-rtDW.RateTransition2_Buffer +
                       platform_targets.hover.h_ref) * platform_targets.hover.Kh;

    // Saturate: '<S4>/Saturation1'
    if (rtb_Saturation1 > platform_targets.hover.v_sat[1]) {
      rtb_Saturation1 = platform_targets.hover.v_sat[1];
    } else if (rtb_Saturation1 < platform_targets.hover.v_sat[0]) {
      rtb_Saturation1 = platform_targets.hover.v_sat[0];
    }

    // End of Saturate: '<S4>/Saturation1'

    // Sum: '<S4>/Sum'
    rtb_Sum_o = rtb_Saturation1 - rtb_RateTransition3;

    // Product: '<S113>/PProd Out' incorporates:
    //   Constant: '<S4>/Constant3'

    rtb_PProdOut = rtb_Sum_o * platform_targets.altitude.kP_hover;
  }

  // RateTransition: '<S2>/Rate Transition5' incorporates:
  //   Inport: '<Root>/Dropped'

  if (tmp && tmp_1) {
    rtDW.RateTransition5_Buffer = rtU.Dropped;
  }

  if (tmp_1) {
    // Logic: '<S4>/NOT'
    rtb_NOT = !rtDW.RateTransition5_Buffer;

    // DiscreteIntegrator: '<S108>/Integrator'
    if (rtb_NOT || (rtDW.Integrator_PrevResetState_g != 0)) {
      rtDW.Integrator_DSTATE_g = -9.81;
    }

    // Gain: '<S4>/Gain1' incorporates:
    //   UnaryMinus: '<S4>/Unary Minus'

    rtb_Saturation3 = platform_targets.hover.Kh * -rtb_RateTransition3;

    // Saturate: '<S4>/Saturation3'
    if (rtb_Saturation3 > platform_targets.a_dec_low) {
      rtb_Saturation3 = platform_targets.a_dec_low;
    } else if (rtb_Saturation3 < -9.81) {
      rtb_Saturation3 = -9.81;
    }

    // End of Saturate: '<S4>/Saturation3'

    // Sum: '<S118>/Sum' incorporates:
    //   DiscreteIntegrator: '<S108>/Integrator'
    //   UnaryMinus: '<S102>/Unary Minus'

    rtb_Sum = (rtb_PProdOut + rtDW.Integrator_DSTATE_g) + rtb_Saturation3;

    // Switch: '<S116>/Switch2' incorporates:
    //   Constant: '<S4>/Constant1'
    //   RelationalOperator: '<S116>/LowerRelop1'
    //   RelationalOperator: '<S116>/UpperRelop'
    //   Switch: '<S116>/Switch'

    if (rtb_Sum > rtb_Diff) {
      rtb_Switch2_k = rtb_Diff;
    } else if (rtb_Sum < -9.81) {
      // Switch: '<S116>/Switch' incorporates:
      //   Constant: '<S4>/Constant1'

      rtb_Switch2_k = -9.81;
    } else {
      rtb_Switch2_k = rtb_Sum;
    }

    // End of Switch: '<S116>/Switch2'

    // Bias: '<S3>/Bias' incorporates:
    //   Gain: '<S3>/Gain'

    rtb_Bias_e = 0.0095 * rtb_RateTransition4 + 31.5;

    // Bias: '<S3>/Bias5' incorporates:
    //   Gain: '<S3>/Gain3'
    //   Math: '<S3>/Square'
    //   Math: '<S3>/Square1'
    //   Sum: '<S3>/Sum1'

    u0 = (rtb_RateTransition6_n_idx_0 * rtb_RateTransition6_n_idx_0 +
          rtb_RateTransition6_n_idx_1 * rtb_RateTransition6_n_idx_1) * -2.0 +
      1.0;

    // Saturate: '<S3>/Saturation4'
    if (u0 < 0.7) {
      u0 = 0.7;
    }

    // Math: '<S3>/Math Function' incorporates:
    //   Saturate: '<S3>/Saturation4'
    //
    //  About '<S3>/Math Function':
    //   Operator: reciprocal

    rtb_MathFunction_n = 1.0 / u0;

    // Bias: '<S3>/Bias4' incorporates:
    //   Product: '<S3>/Product2'

    rtb_Diff_j = rtb_RateTransition / rtb_Bias_e / rtb_MathFunction_n - 9.81;

    // Sqrt: '<S3>/Sqrt' incorporates:
    //   Bias: '<S3>/Bias1'
    //   Bias: '<S3>/Bias2'
    //   Gain: '<S3>/Gain2'
    //   Saturate: '<S3>/Saturation1'

    rtb_Sqrt = std::sqrt(2.0 * platform_targets.a_dec * std::fmax
                         (rtb_RateTransition2 - platform_targets.h_cut, 0.0) +
                         platform_targets.V_td * platform_targets.V_td);

    // Saturate: '<S3>/Saturation2'
    rtb_Sum_d = std::fmin(rtb_Sqrt, platform_targets.V_max);

    // UnaryMinus: '<S3>/Unary Minus' incorporates:
    //   Saturate: '<S3>/Saturation2'

    rtb_UnaryMinus_l = -rtb_Sum_d;

    // Sum: '<S3>/Sum' incorporates:
    //   UnaryMinus: '<S3>/Unary Minus'

    rtb_Sum_d = -rtb_Sum_d - rtb_RateTransition3;

    // DiscreteIntegrator: '<S49>/Integrator' incorporates:
    //   Logic: '<S3>/NOT'

    if (rtb_NOT || (rtDW.Integrator_PrevResetState_n != 0)) {
      rtDW.Integrator_DSTATE_p = 0.0;
    }

    // Switch: '<S3>/Switch' incorporates:
    //   Abs: '<S3>/Abs1'
    //   Constant: '<S3>/Constant'
    //   Constant: '<S6>/Constant'
    //   Gain: '<S3>/Gain1'
    //   Product: '<S3>/Divide'
    //   RelationalOperator: '<S6>/Compare'

    if (rtb_Sqrt < platform_targets.V_max) {
      rtb_RateTransition3 = std::abs(rtb_RateTransition3) / rtb_Sqrt *
        platform_targets.a_dec;
    } else {
      rtb_RateTransition3 = -9.8;
    }

    // End of Switch: '<S3>/Switch'

    // Saturate: '<S3>/Saturation3'
    rtb_Sqrt = std::fmin(rtb_RateTransition3, platform_targets.a_dec);

    // Sum: '<S59>/Sum' incorporates:
    //   Constant: '<S3>/Constant3'
    //   DiscreteIntegrator: '<S49>/Integrator'
    //   Product: '<S54>/PProd Out'
    //   UnaryMinus: '<S43>/Unary Minus'

    rtb_Sum_m = (rtb_Sum_d * platform_targets.altitude.kP +
                 rtDW.Integrator_DSTATE_p) + rtb_Sqrt;

    // Switch: '<S57>/Switch2' incorporates:
    //   Constant: '<S3>/Constant1'
    //   RelationalOperator: '<S57>/LowerRelop1'
    //   RelationalOperator: '<S57>/UpperRelop'
    //   Switch: '<S57>/Switch'

    if (rtb_Sum_m > rtb_Diff_j) {
      rtb_Switch2_f = rtb_Diff_j;
    } else if (rtb_Sum_m < -9.81) {
      // Switch: '<S57>/Switch' incorporates:
      //   Constant: '<S3>/Constant1'

      rtb_Switch2_f = -9.81;
    } else {
      rtb_Switch2_f = rtb_Sum_m;
    }

    // End of Switch: '<S57>/Switch2'

    // MultiPortSwitch generated from: '<S2>/Multiport Switch' incorporates:
    //   Constant: '<S2>/Constant'
    //   Constant: '<S2>/Constant1'

    switch (mission_mode) {
     case mission_modes::HOVER:
      // Product: '<S4>/Product1' incorporates:
      //   Bias: '<S4>/Bias3'

      rtb_RateTransition3 = (rtb_Switch2_k + 9.81) * rtb_Bias * rtb_MathFunction;

      // Switch: '<S70>/Switch2' incorporates:
      //   Constant: '<S4>/Constant5'
      //   RelationalOperator: '<S70>/LowerRelop1'
      //   RelationalOperator: '<S70>/UpperRelop'
      //   Switch: '<S70>/Switch'

      if (rtb_RateTransition3 > rtb_RateTransition) {
        rtb_RateTransition3 = rtb_RateTransition;
      } else if (rtb_RateTransition3 < 0.0) {
        // Switch: '<S70>/Switch' incorporates:
        //   Constant: '<S4>/Constant5'

        rtb_RateTransition3 = 0.0;
      }

      // End of Switch: '<S70>/Switch2'
      break;

     case mission_modes::DROP:
      // Switch: '<S3>/Switch1' incorporates:
      //   Constant: '<S3>/Constant8'
      //   Constant: '<S8>/Constant'
      //   RelationalOperator: '<S8>/Compare'

      if (rtb_RateTransition2 <= 0.25) {
        rtb_RateTransition3 = 0.0;
      } else {
        // Product: '<S3>/Product1' incorporates:
        //   Bias: '<S3>/Bias3'

        rtb_RateTransition3 = (rtb_Switch2_f + 9.81) * rtb_Bias_e *
          rtb_MathFunction_n;

        // Switch: '<S11>/Switch2' incorporates:
        //   Constant: '<S3>/Constant5'
        //   RelationalOperator: '<S11>/LowerRelop1'
        //   RelationalOperator: '<S11>/UpperRelop'
        //   Switch: '<S11>/Switch'

        if (rtb_RateTransition3 > rtb_RateTransition) {
          rtb_RateTransition3 = rtb_RateTransition;
        } else if (rtb_RateTransition3 < 0.0) {
          // Switch: '<S11>/Switch' incorporates:
          //   Constant: '<S3>/Constant5'

          rtb_RateTransition3 = 0.0;
        }

        // End of Switch: '<S11>/Switch2'
      }

      // End of Switch: '<S3>/Switch1'
      break;

     default:
      rtb_RateTransition3 = 0.0;
      break;
    }

    // Gain: '<S2>/Gain' incorporates:
    //   Gain: '<S2>/Gain1'

    rtb_RateTransition3 *= 0.25;

    // Gain: '<S2>/Gain'
    rtDW.Gain = rtb_RateTransition3;

    // Outport: '<Root>/VerticalThrustCmd'
    rtY.VerticalThrustCmd = rtb_RateTransition3;

    // MultiPortSwitch generated from: '<S2>/Multiport Switch' incorporates:
    //   Constant: '<S2>/Constant'

    switch (mission_mode) {
     case mission_modes::HOVER:
      // Outport: '<Root>/a_cmd'
      rtY.a_cmd = rtb_Switch2_k;
      break;

     case mission_modes::DROP:
      // Outport: '<Root>/a_cmd'
      rtY.a_cmd = rtb_Switch2_f;
      break;

     default:
      // Outport: '<Root>/a_cmd' incorporates:
      //   Constant: '<S2>/Constant1'

      rtY.a_cmd = 0.0;
      break;
    }
  }

  // RateTransition: '<S2>/Rate Transition1' incorporates:
  //   Inport: '<Root>/T_alloc_total'

  if (tmp) {
    // Outport: '<Root>/Fy_pos' incorporates:
    //   Sum: '<S2>/Sum1'

    rtY.Fy_pos = rtDW.Gain + rtb_Saturation_idx_0;

    // Saturate: '<S128>/Saturation1' incorporates:
    //   UnaryMinus: '<S128>/Unary Minus'

    if (-rtb_Saturation2 > 272.0) {
      u0 = 272.0;
    } else if (-rtb_Saturation2 < -272.0) {
      u0 = -272.0;
    } else {
      u0 = -rtb_Saturation2;
    }

    // Outport: '<Root>/Fy_neg' incorporates:
    //   Saturate: '<S128>/Saturation1'
    //   Sum: '<S2>/Sum'

    rtY.Fy_neg = rtDW.Gain + u0;

    // Gain: '<S128>/Gain5'
    rtb_Saturation_idx_0 = 1.0 / (2.0 * platform_targets.attitude.r) *
      rtb_Saturation_idx_1;

    // Saturate: '<S128>/Saturation2'
    if (rtb_Saturation_idx_0 > 272.0) {
      u0 = 272.0;
    } else if (rtb_Saturation_idx_0 < -272.0) {
      u0 = -272.0;
    } else {
      u0 = rtb_Saturation_idx_0;
    }

    // Outport: '<Root>/Fx_neg' incorporates:
    //   Saturate: '<S128>/Saturation2'
    //   Sum: '<S2>/Sum3'

    rtY.Fx_neg = rtDW.Gain + u0;

    // Saturate: '<S128>/Saturation3' incorporates:
    //   UnaryMinus: '<S128>/Unary Minus1'

    if (-rtb_Saturation_idx_0 > 272.0) {
      u0 = 272.0;
    } else if (-rtb_Saturation_idx_0 < -272.0) {
      u0 = -272.0;
    } else {
      u0 = -rtb_Saturation_idx_0;
    }

    // Outport: '<Root>/Fx_pos' incorporates:
    //   Saturate: '<S128>/Saturation3'
    //   Sum: '<S2>/Sum2'

    rtY.Fx_pos = rtDW.Gain + u0;
    if (tmp_1) {
      rtDW.RateTransition1_Buffer = rtU.T_alloc_total;
    }

    // Update for DiscreteIntegrator: '<S165>/Integrator' incorporates:
    //   Inport: '<Root>/Dropped'
    //   Inport: '<Root>/T_alloc_total'

    rtb_Saturation2 = 0.005 * rtb_Sum_h_idx_0 + rtDW.Integrator_DSTATE[0];
    rtDW.Integrator_DSTATE[0] = rtb_Saturation2;
    if (rtb_Saturation2 > 16.5) {
      rtDW.Integrator_DSTATE[0] = 16.5;
    } else if (rtb_Saturation2 < -16.5) {
      rtDW.Integrator_DSTATE[0] = -16.5;
    }

    rtb_Saturation2 = 0.005 * rtb_Sum_h_idx_1 + rtDW.Integrator_DSTATE[1];
    rtDW.Integrator_DSTATE[1] = rtb_Saturation2;
    if (rtb_Saturation2 > 16.5) {
      rtDW.Integrator_DSTATE[1] = 16.5;
    } else if (rtb_Saturation2 < -16.5) {
      rtDW.Integrator_DSTATE[1] = -16.5;
    }

    rtb_Saturation2 = 0.005 * rtb_Sum_h_idx_2 + rtDW.Integrator_DSTATE[2];
    rtDW.Integrator_DSTATE[2] = rtb_Saturation2;
    if (rtb_Saturation2 > 16.5) {
      rtDW.Integrator_DSTATE[2] = 16.5;
    } else if (rtb_Saturation2 < -16.5) {
      rtDW.Integrator_DSTATE[2] = -16.5;
    }

    rtDW.Integrator_PrevResetState = static_cast<int8_T>(rtU.Dropped);

    // End of Update for DiscreteIntegrator: '<S165>/Integrator'
  }

  if (tmp_1) {
    // Sum: '<S62>/SumI1' incorporates:
    //   Bias: '<S3>/Bias6'
    //   Constant: '<S3>/Constant4'
    //   Product: '<S3>/Product3'
    //   Product: '<S46>/IProd Out'
    //   Sum: '<S61>/SumI3'

    rtb_RateTransition3 = ((rtDW.RateTransition1_Buffer / rtb_Bias_e /
      rtb_MathFunction_n - 9.81) - rtb_Switch2_f) + rtb_Sum_d *
      platform_targets.altitude.kI;

    // Switch: '<S41>/Switch' incorporates:
    //   RelationalOperator: '<S41>/u_GTE_up'

    if (rtb_Sum_m < rtb_Diff_j) {
      // Switch: '<S41>/Switch1' incorporates:
      //   Constant: '<S3>/Constant1'
      //   RelationalOperator: '<S41>/u_GT_lo'

      if (rtb_Sum_m > -9.81) {
        rtb_Diff_j = rtb_Sum_m;
      } else {
        rtb_Diff_j = -9.81;
      }

      // End of Switch: '<S41>/Switch1'
    }

    // End of Switch: '<S41>/Switch'

    // Sum: '<S41>/Diff'
    rtb_Diff_j = rtb_Sum_m - rtb_Diff_j;

    // Switch: '<S38>/Switch1' incorporates:
    //   Constant: '<S38>/Clamping_zero'
    //   Constant: '<S38>/Constant'
    //   Constant: '<S38>/Constant2'
    //   RelationalOperator: '<S38>/fix for DT propagation issue'

    if (rtb_Diff_j > 0.0) {
      tmp_2 = 1;
    } else {
      tmp_2 = -1;
    }

    // Switch: '<S38>/Switch2' incorporates:
    //   Constant: '<S38>/Clamping_zero'
    //   Constant: '<S38>/Constant3'
    //   Constant: '<S38>/Constant4'
    //   RelationalOperator: '<S38>/fix for DT propagation issue1'

    if (rtb_RateTransition3 > 0.0) {
      tmp_3 = 1;
    } else {
      tmp_3 = -1;
    }

    // Switch: '<S38>/Switch' incorporates:
    //   Constant: '<S38>/Clamping_zero'
    //   Constant: '<S38>/Constant1'
    //   Logic: '<S38>/AND3'
    //   RelationalOperator: '<S38>/Equal1'
    //   RelationalOperator: '<S38>/Relational Operator'
    //   Switch: '<S38>/Switch1'
    //   Switch: '<S38>/Switch2'

    if ((rtb_Diff_j != 0.0) && (tmp_2 == tmp_3)) {
      rtb_Saturation2 = 0.0;
    } else {
      rtb_Saturation2 = rtb_RateTransition3;
    }

    // End of Switch: '<S38>/Switch'

    // Sum: '<S121>/SumI1' incorporates:
    //   Bias: '<S4>/Bias6'
    //   Constant: '<S4>/Constant4'
    //   Product: '<S105>/IProd Out'
    //   Product: '<S4>/Product3'
    //   Sum: '<S120>/SumI3'

    rtb_RateTransition3 = ((rtDW.RateTransition1_Buffer / rtb_Bias /
      rtb_MathFunction - 9.81) - rtb_Switch2_k) +
      platform_targets.altitude.kI_hover * 2.0 * rtb_Sum_o;

    // Switch: '<S100>/Switch' incorporates:
    //   RelationalOperator: '<S100>/u_GTE_up'

    if (rtb_Sum < rtb_Diff) {
      // Switch: '<S100>/Switch1' incorporates:
      //   Constant: '<S4>/Constant1'
      //   RelationalOperator: '<S100>/u_GT_lo'

      if (rtb_Sum > -9.81) {
        rtb_Diff = rtb_Sum;
      } else {
        rtb_Diff = -9.81;
      }

      // End of Switch: '<S100>/Switch1'
    }

    // End of Switch: '<S100>/Switch'

    // Sum: '<S100>/Diff'
    rtb_Diff = rtb_Sum - rtb_Diff;

    // Switch: '<S97>/Switch1' incorporates:
    //   Constant: '<S97>/Clamping_zero'
    //   Constant: '<S97>/Constant'
    //   Constant: '<S97>/Constant2'
    //   RelationalOperator: '<S97>/fix for DT propagation issue'

    if (rtb_Diff > 0.0) {
      tmp_2 = 1;
    } else {
      tmp_2 = -1;
    }

    // Switch: '<S97>/Switch2' incorporates:
    //   Constant: '<S97>/Clamping_zero'
    //   Constant: '<S97>/Constant3'
    //   Constant: '<S97>/Constant4'
    //   RelationalOperator: '<S97>/fix for DT propagation issue1'

    if (rtb_RateTransition3 > 0.0) {
      tmp_3 = 1;
    } else {
      tmp_3 = -1;
    }

    // Switch: '<S97>/Switch' incorporates:
    //   Constant: '<S97>/Clamping_zero'
    //   Constant: '<S97>/Constant1'
    //   Logic: '<S97>/AND3'
    //   RelationalOperator: '<S97>/Equal1'
    //   RelationalOperator: '<S97>/Relational Operator'
    //   Switch: '<S97>/Switch1'
    //   Switch: '<S97>/Switch2'

    if ((rtb_Diff != 0.0) && (tmp_2 == tmp_3)) {
      rtb_RateTransition3 = 0.0;
    }

    // End of Switch: '<S97>/Switch'

    // MultiPortSwitch: '<S2>/Multiport Switch2' incorporates:
    //   Constant: '<S2>/Constant'

    switch (mission_mode) {
     case mission_modes::HOVER:
      // Outport: '<Root>/a_ff'
      rtY.a_ff = rtb_Saturation3;

      // Outport: '<Root>/V_target' incorporates:
      //   MultiPortSwitch: '<S2>/Multiport Switch1'

      rtY.V_target = rtb_Saturation1;
      break;

     case mission_modes::DROP:
      // Outport: '<Root>/a_ff'
      rtY.a_ff = rtb_Sqrt;

      // Outport: '<Root>/V_target' incorporates:
      //   MultiPortSwitch: '<S2>/Multiport Switch1'

      rtY.V_target = rtb_UnaryMinus_l;
      break;

     default:
      // Outport: '<Root>/a_ff' incorporates:
      //   Constant: '<S2>/Constant3'

      rtY.a_ff = 0.0;

      // Outport: '<Root>/V_target' incorporates:
      //   Constant: '<S2>/Constant2'
      //   MultiPortSwitch: '<S2>/Multiport Switch1'

      rtY.V_target = 0.0;
      break;
    }

    // End of MultiPortSwitch: '<S2>/Multiport Switch2'

    // Update for DiscreteIntegrator: '<S108>/Integrator'
    rtDW.Integrator_DSTATE_g += 0.02 * rtb_RateTransition3;
    if (rtDW.Integrator_DSTATE_g > 3.0) {
      rtDW.Integrator_DSTATE_g = 3.0;
    } else if (rtDW.Integrator_DSTATE_g < -10.0) {
      rtDW.Integrator_DSTATE_g = -10.0;
    }

    rtDW.Integrator_PrevResetState_g = static_cast<int8_T>(rtb_NOT);

    // End of Update for DiscreteIntegrator: '<S108>/Integrator'

    // Update for DiscreteIntegrator: '<S49>/Integrator'
    rtDW.Integrator_DSTATE_p += 0.02 * rtb_Saturation2;
    if (rtDW.Integrator_DSTATE_p > 3.0) {
      rtDW.Integrator_DSTATE_p = 3.0;
    } else if (rtDW.Integrator_DSTATE_p < -3.0) {
      rtDW.Integrator_DSTATE_p = -3.0;
    }

    rtDW.Integrator_PrevResetState_n = static_cast<int8_T>(rtb_NOT);

    // End of Update for DiscreteIntegrator: '<S49>/Integrator'
  }

  // Update for RateTransition: '<S5>/Rate Transition1'
  if (tmp_0) {
    rtDW.RateTransition1_Buffer0[0] = rtb_Switch_he_idx_0;
    rtDW.RateTransition1_Buffer0[1] = rtb_Switch_he_idx_1;
    rtDW.RateTransition1_Buffer0[2] = rtb_Switch_he_idx_2;
  }

  rate_scheduler((&rtM));
}

// Model initialize function
void PlatformController::initialize()
{
  // InitializeConditions for DiscreteIntegrator: '<S165>/Integrator'
  rtDW.Integrator_PrevResetState = 2;

  // InitializeConditions for DiscreteIntegrator: '<S108>/Integrator'
  rtDW.Integrator_DSTATE_g = -9.81;
}

const char_T* PlatformController::RT_MODEL::getErrorStatus() const
{
  return (errorStatus);
}

void PlatformController::RT_MODEL::setErrorStatus(const char_T* const volatile
  aErrorStatus)
{
  (errorStatus = aErrorStatus);
}

// Constructor
PlatformController::PlatformController() :
  rtU(),
  rtY(),
  rtDW(),
  rtM()
{
  // Currently there is no constructor body generated.
}

// Destructor
// Currently there is no destructor body generated.
PlatformController::~PlatformController() = default;

// Real-Time Model get method
PlatformController::RT_MODEL * PlatformController::getRTM()
{
  return (&rtM);
}

//
// File trailer for generated code.
//
// [EOF]
//
