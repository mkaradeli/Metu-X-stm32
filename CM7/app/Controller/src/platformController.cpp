//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: platformController.cpp
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
#include <cmath>
#include "rtwtypes.h"
#include "limits"
#include "cmath"

// Exported block parameters
mission_modes mission_mode{ mission_modes::DISABLE };// Variable: mission_mode
                                                        //  Referenced by: '<S2>/Constant'


static void rate_scheduler(PlatformController::RT_MODEL *const rtM);
//extern "C"
//{
//  real_T rtNaN { -std::numeric_limits<real_T>::quiet_NaN() };
//
//  real_T rtInf { std::numeric_limits<real_T>::infinity() };
//
//  real_T rtMinusInf { -std::numeric_limits<real_T>::infinity() };
//
//  real32_T rtNaNF { -std::numeric_limits<real32_T>::quiet_NaN() };
//
//  real32_T rtInfF { std::numeric_limits<real32_T>::infinity() };
//
//  real32_T rtMinusInfF { -std::numeric_limits<real32_T>::infinity() };
//}

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
  real_T rtb_Bias_l;
  real_T rtb_Diff;
  real_T rtb_Diff_n;
  real_T rtb_MathFunction;
  real_T rtb_MathFunction_l;
  real_T rtb_PProdOut;
  real_T rtb_RateTransition;
  real_T rtb_RateTransition2;
  real_T rtb_RateTransition3;
  real_T rtb_RateTransition4;
  real_T rtb_RateTransition6_idx_0;
  real_T rtb_RateTransition6_idx_1;
  real_T rtb_Saturation3;
  real_T rtb_Sqrt;
  real_T rtb_Sum;
  real_T rtb_Sum_h;
  real_T rtb_Sum_i_idx_0;
  real_T rtb_Sum_i_idx_1;
  real_T rtb_Sum_l;
  real_T rtb_Switch;
  real_T rtb_Switch2;
  real_T rtb_Switch2_o;
  real_T rtb_Switch_e;
  real_T rtb_error_idx_0;
  real_T rtb_error_idx_1;
  real_T rtb_error_idx_2;
  real_T u0;
  int8_T tmp_1;
  int8_T tmp_2;
  boolean_T rtb_NOT;
  boolean_T tmp;
  boolean_T tmp_0;

  // RateTransition: '<S2>/Rate Transition4' incorporates:
  //   RateTransition: '<S2>/Rate Transition2'
  //   RateTransition: '<S2>/Rate Transition3'
  //   RateTransition: '<S2>/Rate Transition5'
  //   RateTransition: '<S2>/Rate Transition6'
  //   RateTransition: '<S5>/Rate Transition6'

  tmp = ((&rtM)->Timing.TaskCounters.TID[1] == 0);
  if (tmp) {
    // RateTransition: '<S5>/Rate Transition1'
    if ((&rtM)->Timing.TaskCounters.TID[2] == 0) {
      // RateTransition: '<S5>/Rate Transition1'
      rtDW.RateTransition1[0] = rtDW.RateTransition1_Buffer0[0];
      rtDW.RateTransition1[1] = rtDW.RateTransition1_Buffer0[1];
      rtDW.RateTransition1[2] = rtDW.RateTransition1_Buffer0[2];
    }

    // End of RateTransition: '<S5>/Rate Transition1'

    // Sum: '<S128>/Subtract3' incorporates:
    //   Inport: '<Root>/angularVelocity'

    rtb_error_idx_1 = rtDW.RateTransition1[0] - rtU.angularVelocity[0];

    // Sum: '<S173>/Sum' incorporates:
    //   DiscreteIntegrator: '<S164>/Integrator'
    //   Gain: '<S169>/Proportional Gain'

    rtb_Sum_i_idx_0 = rtP.platform_targets.attitude.Kp_rate * rtb_error_idx_1 +
      rtDW.Integrator_DSTATE[0];

    // Gain: '<S161>/Integral Gain'
    rtb_error_idx_0 = rtP.platform_targets.attitude.Ki_rate * rtb_error_idx_1;

    // Sum: '<S128>/Subtract3' incorporates:
    //   Inport: '<Root>/angularVelocity'

    rtb_error_idx_1 = rtDW.RateTransition1[1] - rtU.angularVelocity[1];

    // Sum: '<S173>/Sum' incorporates:
    //   DiscreteIntegrator: '<S164>/Integrator'
    //   Gain: '<S169>/Proportional Gain'

    rtb_Sum_i_idx_1 = rtP.platform_targets.attitude.Kp_rate * rtb_error_idx_1 +
      rtDW.Integrator_DSTATE[1];

    // Gain: '<S161>/Integral Gain' incorporates:
    //   Inport: '<Root>/angularVelocity'
    //   Sum: '<S128>/Subtract3'

    rtb_error_idx_1 *= rtP.platform_targets.attitude.Ki_rate;
    rtb_error_idx_2 = (rtDW.RateTransition1[2] - rtU.angularVelocity[2]) *
      rtP.platform_targets.attitude.Ki_rate;

    // Gain: '<S128>/Gain4'
    rtb_Sum_i_idx_0 *= 1.0 / (2.0 * rtP.platform_targets.attitude.r);

    // Saturate: '<S128>/Saturation'
    if (rtb_Sum_i_idx_0 > rtP.Saturation_UpperSat) {
      rtb_Saturation3 = rtP.Saturation_UpperSat;
    } else if (rtb_Sum_i_idx_0 < rtP.Saturation_LowerSat) {
      rtb_Saturation3 = rtP.Saturation_LowerSat;
    } else {
      rtb_Saturation3 = rtb_Sum_i_idx_0;
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

  tmp_0 = ((&rtM)->Timing.TaskCounters.TID[3] == 0);
  if (tmp_0) {
    rtb_RateTransition = rtDW.RateTransition_Buffer;
  }

  // RateTransition: '<S2>/Rate Transition4' incorporates:
  //   Inport: '<Root>/ManifoldPressure'

  if (tmp && tmp_0) {
    rtDW.RateTransition4_Buffer = rtU.ManifoldPressure;
  }

  if (tmp_0) {
    rtb_RateTransition4 = rtDW.RateTransition4_Buffer;

    // Bias: '<S4>/Bias' incorporates:
    //   Gain: '<S4>/Gain'

    rtb_Bias = rtP.Gain_Gain * rtDW.RateTransition4_Buffer + rtP.mass.empty;
  }

  // RateTransition: '<S2>/Rate Transition6' incorporates:
  //   Inport: '<Root>/quaternion'

  if (tmp && tmp_0) {
    rtDW.RateTransition6_Buffer[0] = rtU.quaternion[0];
    rtDW.RateTransition6_Buffer[1] = rtU.quaternion[1];
    rtDW.RateTransition6_Buffer[2] = rtU.quaternion[2];
    rtDW.RateTransition6_Buffer[3] = rtU.quaternion[3];
  }

  if (tmp_0) {
    rtb_RateTransition6_idx_0 = rtDW.RateTransition6_Buffer[0];
    rtb_RateTransition6_idx_1 = rtDW.RateTransition6_Buffer[1];

    // Math: '<S4>/Square'
    rtb_Sum_h = rtDW.RateTransition6_Buffer[0] * rtDW.RateTransition6_Buffer[0];

    // Bias: '<S4>/Bias5' incorporates:
    //   Gain: '<S4>/Gain3'
    //   Math: '<S4>/Square1'
    //   Sum: '<S4>/Sum1'

    u0 = (rtDW.RateTransition6_Buffer[1] * rtDW.RateTransition6_Buffer[1] +
          rtb_Sum_h) * rtP.Gain3_Gain + rtP.Bias5_Bias;

    // Saturate: '<S4>/Saturation4'
    if (u0 > rtP.Saturation4_UpperSat) {
      u0 = rtP.Saturation4_UpperSat;
    } else if (u0 < rtP.Saturation4_LowerSat) {
      u0 = rtP.Saturation4_LowerSat;
    }

    // Math: '<S4>/Math Function' incorporates:
    //   Saturate: '<S4>/Saturation4'
    //
    //  About '<S4>/Math Function':
    //   Operator: reciprocal

    rtb_MathFunction = 1.0 / u0;

    // Bias: '<S4>/Bias4' incorporates:
    //   Product: '<S4>/Product2'

    rtb_Diff = rtb_RateTransition / rtb_Bias / rtb_MathFunction + rtP.Bias4_Bias;
  }

  // RateTransition: '<S2>/Rate Transition3' incorporates:
  //   Inport: '<Root>/Velocity'

  if (tmp && tmp_0) {
    rtDW.RateTransition3_Buffer = rtU.Velocity;
  }

  if (tmp_0) {
    rtb_RateTransition3 = rtDW.RateTransition3_Buffer;
  }

  // RateTransition: '<S2>/Rate Transition2' incorporates:
  //   Inport: '<Root>/Height'

  if (tmp && tmp_0) {
    rtDW.RateTransition2_Buffer = rtU.Height;
  }

  if (tmp_0) {
    rtb_RateTransition2 = rtDW.RateTransition2_Buffer;

    // Gain: '<S4>/Gain5' incorporates:
    //   Bias: '<S4>/Bias1'
    //   UnaryMinus: '<S4>/Unary Minus1'

    u0 = (-rtDW.RateTransition2_Buffer + rtP.platform_targets.hover.h_ref) *
      rtP.platform_targets.hover.Kh;

    // Saturate: '<S4>/Saturation1'
    if (u0 > rtP.platform_targets.hover.v_sat[1]) {
      u0 = rtP.platform_targets.hover.v_sat[1];
    } else if (u0 < rtP.platform_targets.hover.v_sat[0]) {
      u0 = rtP.platform_targets.hover.v_sat[0];
    }

    // Sum: '<S4>/Sum' incorporates:
    //   Saturate: '<S4>/Saturation1'

    rtb_Sum_h = u0 - rtb_RateTransition3;

    // Product: '<S113>/PProd Out' incorporates:
    //   Constant: '<S4>/Constant3'

    rtb_PProdOut = rtb_Sum_h * rtP.platform_targets.altitude.kP_hover;
  }

  // RateTransition: '<S2>/Rate Transition5' incorporates:
  //   Inport: '<Root>/Dropped'

  if (tmp && tmp_0) {
    rtDW.RateTransition5_Buffer = rtU.Dropped;
  }

  if (tmp_0) {
    // Logic: '<S4>/NOT'
    rtb_NOT = !rtDW.RateTransition5_Buffer;

    // DiscreteIntegrator: '<S108>/Integrator'
    if (rtb_NOT || (rtDW.Integrator_PrevResetState != 0)) {
      rtDW.Integrator_DSTATE_a = rtP.PIDController_InitialConditio_c;
    }

    // Gain: '<S4>/Gain1' incorporates:
    //   UnaryMinus: '<S4>/Unary Minus'

    u0 = rtP.platform_targets.hover.Kh * -rtb_RateTransition3;

    // Saturate: '<S4>/Saturation3'
    if (u0 > rtP.platform_targets.a_dec_low) {
      u0 = rtP.platform_targets.a_dec_low;
    } else if (u0 < rtP.Saturation3_LowerSat) {
      u0 = rtP.Saturation3_LowerSat;
    }

    // Sum: '<S118>/Sum' incorporates:
    //   Constant: '<S4>/Constant2'
    //   DiscreteIntegrator: '<S108>/Integrator'
    //   Product: '<S101>/DProd Out'
    //   Saturate: '<S4>/Saturation3'
    //   UnaryMinus: '<S102>/Unary Minus'

    rtb_Sum = (rtb_PProdOut + rtDW.Integrator_DSTATE_a) + -u0 *
      rtP.Constant2_Value;

    // Switch: '<S116>/Switch2' incorporates:
    //   Constant: '<S4>/Constant1'
    //   RelationalOperator: '<S116>/LowerRelop1'
    //   RelationalOperator: '<S116>/UpperRelop'
    //   Switch: '<S116>/Switch'

    if (rtb_Sum > rtb_Diff) {
      rtb_Switch2 = rtb_Diff;
    } else if (rtb_Sum < rtP.Constant1_Value_l) {
      // Switch: '<S116>/Switch' incorporates:
      //   Constant: '<S4>/Constant1'

      rtb_Switch2 = rtP.Constant1_Value_l;
    } else {
      rtb_Switch2 = rtb_Sum;
    }

    // End of Switch: '<S116>/Switch2'

    // Bias: '<S3>/Bias' incorporates:
    //   Gain: '<S3>/Gain'

    rtb_Bias_l = rtP.Gain_Gain_i * rtb_RateTransition4 + rtP.mass.empty;

    // Bias: '<S3>/Bias5' incorporates:
    //   Gain: '<S3>/Gain3'
    //   Math: '<S3>/Square'
    //   Math: '<S3>/Square1'
    //   Sum: '<S3>/Sum1'

    u0 = (rtb_RateTransition6_idx_0 * rtb_RateTransition6_idx_0 +
          rtb_RateTransition6_idx_1 * rtb_RateTransition6_idx_1) *
      rtP.Gain3_Gain_j + rtP.Bias5_Bias_i;

    // Saturate: '<S3>/Saturation4'
    if (u0 > rtP.Saturation4_UpperSat_l) {
      u0 = rtP.Saturation4_UpperSat_l;
    } else if (u0 < rtP.Saturation4_LowerSat_m) {
      u0 = rtP.Saturation4_LowerSat_m;
    }

    // Math: '<S3>/Math Function' incorporates:
    //   Saturate: '<S3>/Saturation4'
    //
    //  About '<S3>/Math Function':
    //   Operator: reciprocal

    rtb_MathFunction_l = 1.0 / u0;

    // Bias: '<S3>/Bias4' incorporates:
    //   Product: '<S3>/Product2'

    rtb_Diff_n = rtb_RateTransition / rtb_Bias_l / rtb_MathFunction_l +
      rtP.Bias4_Bias_i;

    // Bias: '<S3>/Bias1'
    u0 = rtb_RateTransition2 - rtP.platform_targets.h_cut;

    // Saturate: '<S3>/Saturation1'
    if (u0 > rtP.Saturation1_UpperSat) {
      u0 = rtP.Saturation1_UpperSat;
    } else if (u0 < rtP.Saturation1_LowerSat) {
      u0 = rtP.Saturation1_LowerSat;
    }

    // Sqrt: '<S3>/Sqrt' incorporates:
    //   Bias: '<S3>/Bias2'
    //   Gain: '<S3>/Gain2'
    //   Saturate: '<S3>/Saturation1'

    rtb_Sqrt = std::sqrt(2.0 * rtP.platform_targets.a_dec * u0 +
                         rtP.platform_targets.V_td * rtP.platform_targets.V_td);

    // Saturate: '<S3>/Saturation2'
    if (rtb_Sqrt > rtP.platform_targets.V_max) {
      rtb_RateTransition4 = rtP.platform_targets.V_max;
    } else if (rtb_Sqrt < rtP.Saturation2_LowerSat) {
      rtb_RateTransition4 = rtP.Saturation2_LowerSat;
    } else {
      rtb_RateTransition4 = rtb_Sqrt;
    }

    // Sum: '<S3>/Sum' incorporates:
    //   Saturate: '<S3>/Saturation2'
    //   UnaryMinus: '<S3>/Unary Minus'

    rtb_Sum_l = -rtb_RateTransition4 - rtb_RateTransition3;

    // DiscreteIntegrator: '<S49>/Integrator' incorporates:
    //   Logic: '<S3>/NOT'

    if (rtb_NOT || (rtDW.Integrator_PrevResetState_o != 0)) {
      rtDW.Integrator_DSTATE_m = rtP.PIDController_InitialConditio_m;
    }

    // Switch: '<S3>/Switch' incorporates:
    //   Abs: '<S3>/Abs1'
    //   Constant: '<S3>/Constant'
    //   Constant: '<S6>/Constant'
    //   Gain: '<S3>/Gain1'
    //   Product: '<S3>/Divide'
    //   RelationalOperator: '<S6>/Compare'

    if (rtb_Sqrt < rtP.platform_targets.V_max) {
      rtb_RateTransition3 = std::abs(rtb_RateTransition3) / rtb_Sqrt *
        rtP.platform_targets.a_dec;
    } else {
      rtb_RateTransition3 = rtP.Constant_Value;
    }

    // End of Switch: '<S3>/Switch'

    // Saturate: '<S3>/Saturation3'
    if (rtb_RateTransition3 > rtP.platform_targets.a_dec) {
      rtb_RateTransition3 = rtP.platform_targets.a_dec;
    } else if (rtb_RateTransition3 < rtP.Saturation3_LowerSat_a) {
      rtb_RateTransition3 = rtP.Saturation3_LowerSat_a;
    }

    // Sum: '<S59>/Sum' incorporates:
    //   Constant: '<S3>/Constant2'
    //   Constant: '<S3>/Constant3'
    //   DiscreteIntegrator: '<S49>/Integrator'
    //   Product: '<S42>/DProd Out'
    //   Product: '<S54>/PProd Out'
    //   Saturate: '<S3>/Saturation3'
    //   UnaryMinus: '<S43>/Unary Minus'

    rtb_Sqrt = (rtb_Sum_l * rtP.platform_targets.altitude.kP +
                rtDW.Integrator_DSTATE_m) + -rtb_RateTransition3 *
      rtP.Constant2_Value_c;

    // Switch: '<S57>/Switch2' incorporates:
    //   Constant: '<S3>/Constant1'
    //   RelationalOperator: '<S57>/LowerRelop1'
    //   RelationalOperator: '<S57>/UpperRelop'
    //   Switch: '<S57>/Switch'

    if (rtb_Sqrt > rtb_Diff_n) {
      rtb_Switch2_o = rtb_Diff_n;
    } else if (rtb_Sqrt < rtP.Constant1_Value_h) {
      // Switch: '<S57>/Switch' incorporates:
      //   Constant: '<S3>/Constant1'

      rtb_Switch2_o = rtP.Constant1_Value_h;
    } else {
      rtb_Switch2_o = rtb_Sqrt;
    }

    // End of Switch: '<S57>/Switch2'

    // MultiPortSwitch generated from: '<S2>/Multiport Switch' incorporates:
    //   Constant: '<S2>/Constant'
    //   Constant: '<S2>/Constant1'

    switch (mission_mode) {
     case mission_modes::HOVER:
      // Product: '<S4>/Product1' incorporates:
      //   Bias: '<S4>/Bias3'

      rtb_RateTransition2 = (rtb_Switch2 + rtP.Bias3_Bias) * rtb_Bias *
        rtb_MathFunction;

      // Switch: '<S70>/Switch2' incorporates:
      //   Constant: '<S4>/Constant5'
      //   RelationalOperator: '<S70>/LowerRelop1'
      //   RelationalOperator: '<S70>/UpperRelop'
      //   Switch: '<S70>/Switch'

      if (rtb_RateTransition2 > rtb_RateTransition) {
        rtb_RateTransition3 = rtb_RateTransition;
      } else if (rtb_RateTransition2 < rtP.Constant5_Value) {
        // Switch: '<S70>/Switch' incorporates:
        //   Constant: '<S4>/Constant5'

        rtb_RateTransition3 = rtP.Constant5_Value;
      } else {
        rtb_RateTransition3 = rtb_RateTransition2;
      }

      // End of Switch: '<S70>/Switch2'
      break;

     case mission_modes::DROP:
      // Switch: '<S3>/Switch1' incorporates:
      //   Constant: '<S3>/Constant8'
      //   Constant: '<S8>/Constant'
      //   RelationalOperator: '<S8>/Compare'

      if (rtb_RateTransition2 <= rtP.CompareToConstant2_const) {
        rtb_RateTransition3 = rtP.Constant8_Value;
      } else {
        // Product: '<S3>/Product1' incorporates:
        //   Bias: '<S3>/Bias3'

        rtb_RateTransition2 = (rtb_Switch2_o + rtP.Bias3_Bias_o) * rtb_Bias_l *
          rtb_MathFunction_l;

        // Switch: '<S11>/Switch2' incorporates:
        //   Constant: '<S3>/Constant5'
        //   RelationalOperator: '<S11>/LowerRelop1'
        //   RelationalOperator: '<S11>/UpperRelop'
        //   Switch: '<S11>/Switch'

        if (rtb_RateTransition2 > rtb_RateTransition) {
          rtb_RateTransition3 = rtb_RateTransition;
        } else if (rtb_RateTransition2 < rtP.Constant5_Value_e) {
          // Switch: '<S11>/Switch' incorporates:
          //   Constant: '<S3>/Constant5'

          rtb_RateTransition3 = rtP.Constant5_Value_e;
        } else {
          rtb_RateTransition3 = rtb_RateTransition2;
        }

        // End of Switch: '<S11>/Switch2'
      }

      // End of Switch: '<S3>/Switch1'
      break;

     default:
      rtb_RateTransition3 = rtP.Constant1_Value;
      break;
    }

    // End of MultiPortSwitch generated from: '<S2>/Multiport Switch'

    // Gain: '<S2>/Gain'
    rtDW.Gain = rtP.Gain_Gain_f * rtb_RateTransition3;
  }

  // RateTransition: '<S5>/Rate Transition6' incorporates:
  //   Inport: '<Root>/quaternion'

  if (tmp) {
    // Outport: '<Root>/Fy_pos' incorporates:
    //   Sum: '<S2>/Sum1'

    rtY.Fy_pos = rtDW.Gain + rtb_Saturation3;

    // Saturate: '<S128>/Saturation1' incorporates:
    //   UnaryMinus: '<S128>/Unary Minus'

    if (-rtb_Sum_i_idx_0 > rtP.Saturation1_UpperSat_b) {
      rtb_RateTransition4 = rtP.Saturation1_UpperSat_b;
    } else if (-rtb_Sum_i_idx_0 < rtP.Saturation1_LowerSat_j) {
      rtb_RateTransition4 = rtP.Saturation1_LowerSat_j;
    } else {
      rtb_RateTransition4 = -rtb_Sum_i_idx_0;
    }

    // Outport: '<Root>/Fy_neg' incorporates:
    //   Saturate: '<S128>/Saturation1'
    //   Sum: '<S2>/Sum'

    rtY.Fy_neg = rtDW.Gain + rtb_RateTransition4;

    // Gain: '<S128>/Gain5'
    rtb_Saturation3 = 1.0 / (2.0 * rtP.platform_targets.attitude.r) *
      rtb_Sum_i_idx_1;

    // Saturate: '<S128>/Saturation2'
    if (rtb_Saturation3 > rtP.Saturation2_UpperSat) {
      rtb_RateTransition4 = rtP.Saturation2_UpperSat;
    } else if (rtb_Saturation3 < rtP.Saturation2_LowerSat_d) {
      rtb_RateTransition4 = rtP.Saturation2_LowerSat_d;
    } else {
      rtb_RateTransition4 = rtb_Saturation3;
    }

    // Outport: '<Root>/Fx_neg' incorporates:
    //   Saturate: '<S128>/Saturation2'
    //   Sum: '<S2>/Sum3'

    rtY.Fx_neg = rtDW.Gain + rtb_RateTransition4;

    // Saturate: '<S128>/Saturation3' incorporates:
    //   UnaryMinus: '<S128>/Unary Minus1'

    if (-rtb_Saturation3 > rtP.Saturation3_UpperSat) {
      rtb_RateTransition4 = rtP.Saturation3_UpperSat;
    } else if (-rtb_Saturation3 < rtP.Saturation3_LowerSat_i) {
      rtb_RateTransition4 = rtP.Saturation3_LowerSat_i;
    } else {
      rtb_RateTransition4 = -rtb_Saturation3;
    }

    // Outport: '<Root>/Fx_pos' incorporates:
    //   Saturate: '<S128>/Saturation3'
    //   Sum: '<S2>/Sum2'

    rtY.Fx_pos = rtDW.Gain + rtb_RateTransition4;

    // RateTransition: '<S2>/Rate Transition1' incorporates:
    //   Inport: '<Root>/T_alloc_total'

    if (tmp_0) {
      rtDW.RateTransition1_Buffer = rtU.T_alloc_total;
    }

    if ((&rtM)->Timing.TaskCounters.TID[2] == 0) {
      rtDW.RateTransition6_Buffer_c[0] = rtU.quaternion[0];
      rtDW.RateTransition6_Buffer_c[1] = rtU.quaternion[1];
      rtDW.RateTransition6_Buffer_c[2] = rtU.quaternion[2];
      rtDW.RateTransition6_Buffer_c[3] = rtU.quaternion[3];
    }
  }

  // RateTransition: '<S2>/Rate Transition1'
  if (tmp_0) {
    // Sum: '<S62>/SumI1' incorporates:
    //   Bias: '<S3>/Bias6'
    //   Constant: '<S3>/Constant4'
    //   Gain: '<S61>/Kt'
    //   Product: '<S3>/Product3'
    //   Product: '<S46>/IProd Out'
    //   Sum: '<S61>/SumI3'

    rtb_RateTransition3 = ((rtDW.RateTransition1_Buffer / rtb_Bias_l /
      rtb_MathFunction_l + rtP.Bias6_Bias) - rtb_Switch2_o) *
      rtP.PIDController_Kt + rtb_Sum_l * rtP.platform_targets.altitude.kI;

    // Switch: '<S41>/Switch' incorporates:
    //   RelationalOperator: '<S41>/u_GTE_up'

    if (rtb_Sqrt < rtb_Diff_n) {
      // Switch: '<S41>/Switch1' incorporates:
      //   Constant: '<S3>/Constant1'
      //   RelationalOperator: '<S41>/u_GT_lo'

      if (rtb_Sqrt > rtP.Constant1_Value_h) {
        rtb_Diff_n = rtb_Sqrt;
      } else {
        rtb_Diff_n = rtP.Constant1_Value_h;
      }

      // End of Switch: '<S41>/Switch1'
    }

    // End of Switch: '<S41>/Switch'

    // Sum: '<S41>/Diff'
    rtb_Diff_n = rtb_Sqrt - rtb_Diff_n;

    // Switch: '<S38>/Switch1' incorporates:
    //   Constant: '<S38>/Clamping_zero'
    //   Constant: '<S38>/Constant'
    //   Constant: '<S38>/Constant2'
    //   RelationalOperator: '<S38>/fix for DT propagation issue'

    if (rtb_Diff_n > rtP.Clamping_zero_Value) {
      tmp_1 = rtP.Constant_Value_n;
    } else {
      tmp_1 = rtP.Constant2_Value_l;
    }

    // Switch: '<S38>/Switch2' incorporates:
    //   Constant: '<S38>/Clamping_zero'
    //   Constant: '<S38>/Constant3'
    //   Constant: '<S38>/Constant4'
    //   RelationalOperator: '<S38>/fix for DT propagation issue1'

    if (rtb_RateTransition3 > rtP.Clamping_zero_Value) {
      tmp_2 = rtP.Constant3_Value;
    } else {
      tmp_2 = rtP.Constant4_Value;
    }

    // Switch: '<S38>/Switch' incorporates:
    //   Constant: '<S38>/Clamping_zero'
    //   Constant: '<S38>/Constant1'
    //   Logic: '<S38>/AND3'
    //   RelationalOperator: '<S38>/Equal1'
    //   RelationalOperator: '<S38>/Relational Operator'
    //   Switch: '<S38>/Switch1'
    //   Switch: '<S38>/Switch2'

    if ((rtP.Clamping_zero_Value != rtb_Diff_n) && (tmp_1 == tmp_2)) {
      rtb_Switch = rtP.Constant1_Value_c;
    } else {
      rtb_Switch = rtb_RateTransition3;
    }

    // End of Switch: '<S38>/Switch'

    // Sum: '<S121>/SumI1' incorporates:
    //   Bias: '<S4>/Bias6'
    //   Constant: '<S4>/Constant4'
    //   Gain: '<S120>/Kt'
    //   Product: '<S105>/IProd Out'
    //   Product: '<S4>/Product3'
    //   Sum: '<S120>/SumI3'

    rtb_RateTransition3 = ((rtDW.RateTransition1_Buffer / rtb_Bias /
      rtb_MathFunction + rtP.Bias6_Bias_h) - rtb_Switch2) *
      rtP.PIDController_Kt_n + rtP.platform_targets.altitude.kI_hover * 2.0 *
      rtb_Sum_h;

    // Switch: '<S100>/Switch' incorporates:
    //   RelationalOperator: '<S100>/u_GTE_up'

    if (rtb_Sum < rtb_Diff) {
      // Switch: '<S100>/Switch1' incorporates:
      //   Constant: '<S4>/Constant1'
      //   RelationalOperator: '<S100>/u_GT_lo'

      if (rtb_Sum > rtP.Constant1_Value_l) {
        rtb_Diff = rtb_Sum;
      } else {
        rtb_Diff = rtP.Constant1_Value_l;
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

    if (rtb_Diff > rtP.Clamping_zero_Value_j) {
      tmp_1 = rtP.Constant_Value_nl;
    } else {
      tmp_1 = rtP.Constant2_Value_o;
    }

    // Switch: '<S97>/Switch2' incorporates:
    //   Constant: '<S97>/Clamping_zero'
    //   Constant: '<S97>/Constant3'
    //   Constant: '<S97>/Constant4'
    //   RelationalOperator: '<S97>/fix for DT propagation issue1'

    if (rtb_RateTransition3 > rtP.Clamping_zero_Value_j) {
      tmp_2 = rtP.Constant3_Value_b;
    } else {
      tmp_2 = rtP.Constant4_Value_m;
    }

    // Switch: '<S97>/Switch' incorporates:
    //   Constant: '<S97>/Clamping_zero'
    //   Constant: '<S97>/Constant1'
    //   Logic: '<S97>/AND3'
    //   RelationalOperator: '<S97>/Equal1'
    //   RelationalOperator: '<S97>/Relational Operator'
    //   Switch: '<S97>/Switch1'
    //   Switch: '<S97>/Switch2'

    if ((rtP.Clamping_zero_Value_j != rtb_Diff) && (tmp_1 == tmp_2)) {
      rtb_Switch_e = rtP.Constant1_Value_g;
    } else {
      rtb_Switch_e = rtb_RateTransition3;
    }

    // End of Switch: '<S97>/Switch'
  }

  // RateTransition: '<S5>/Rate Transition6' incorporates:
  //   RateTransition: '<S5>/Rate Transition1'

  if ((&rtM)->Timing.TaskCounters.TID[2] == 0) {
    // Gain: '<S127>/Gain3' incorporates:
    //   UnaryMinus: '<S127>/Unary Minus'

    rtb_Sum_i_idx_1 = rtP.platform_targets.attitude.Kp_att *
      -rtDW.RateTransition6_Buffer_c[0];
    rtb_Saturation3 = rtb_Sum_i_idx_1;

    // DotProduct: '<S127>/Dot Product'
    rtb_RateTransition = rtb_Sum_i_idx_1 * rtb_Sum_i_idx_1;

    // Gain: '<S127>/Gain3' incorporates:
    //   UnaryMinus: '<S127>/Unary Minus'

    rtb_Sum_i_idx_1 = rtP.platform_targets.attitude.Kp_att *
      -rtDW.RateTransition6_Buffer_c[1];
    rtb_RateTransition4 = rtb_Sum_i_idx_1;

    // DotProduct: '<S127>/Dot Product'
    rtb_RateTransition += rtb_Sum_i_idx_1 * rtb_Sum_i_idx_1;

    // Gain: '<S127>/Gain3' incorporates:
    //   UnaryMinus: '<S127>/Unary Minus'

    rtb_Sum_i_idx_1 = rtP.platform_targets.attitude.Kp_att *
      -rtDW.RateTransition6_Buffer_c[2];

    // Product: '<S127>/Divide' incorporates:
    //   Constant: '<S127>/Constant'
    //   DotProduct: '<S127>/Dot Product'
    //   MinMax: '<S127>/Min'
    //   Sqrt: '<S127>/Sqrt'

    rtb_Sum_i_idx_0 = rtP.platform_targets.attitude.w_sat / std::fmax
      (rtP.platform_targets.attitude.w_sat, std::sqrt(rtb_Sum_i_idx_1 *
        rtb_Sum_i_idx_1 + rtb_RateTransition));

    // Product: '<S127>/Product4'
    rtb_RateTransition = rtb_Sum_i_idx_0 * rtb_Saturation3;
    rtb_Saturation3 = rtb_RateTransition;

    // Outport: '<Root>/omega_demand'
    rtY.omega_demand[0] = rtb_RateTransition;

    // Product: '<S127>/Product4'
    rtb_RateTransition = rtb_Sum_i_idx_0 * rtb_RateTransition4;
    rtb_RateTransition4 = rtb_RateTransition;

    // Outport: '<Root>/omega_demand'
    rtY.omega_demand[1] = rtb_RateTransition;

    // Product: '<S127>/Product4' incorporates:
    //   Gain: '<S127>/Gain3'

    rtb_RateTransition = rtb_Sum_i_idx_0 * rtb_Sum_i_idx_1;

    // Outport: '<Root>/omega_demand'
    rtY.omega_demand[2] = rtb_RateTransition;
    rtDW.RateTransition1_Buffer0[0] = rtb_Saturation3;
    rtDW.RateTransition1_Buffer0[1] = rtb_RateTransition4;
    rtDW.RateTransition1_Buffer0[2] = rtb_RateTransition;
  }

  if (tmp) {
    // Update for DiscreteIntegrator: '<S164>/Integrator'
    rtDW.Integrator_DSTATE[0] += rtP.Integrator_gainval * rtb_error_idx_0;
    rtDW.Integrator_DSTATE[1] += rtP.Integrator_gainval * rtb_error_idx_1;
    rtDW.Integrator_DSTATE[2] += rtP.Integrator_gainval * rtb_error_idx_2;
  }

  if (tmp_0) {
    // Update for DiscreteIntegrator: '<S108>/Integrator'
    rtDW.Integrator_DSTATE_a += rtP.Integrator_gainval_c * rtb_Switch_e;
    if (rtDW.Integrator_DSTATE_a > rtP.PIDController_UpperIntegratorSa) {
      rtDW.Integrator_DSTATE_a = rtP.PIDController_UpperIntegratorSa;
    } else if (rtDW.Integrator_DSTATE_a < rtP.PIDController_LowerIntegratorSa) {
      rtDW.Integrator_DSTATE_a = rtP.PIDController_LowerIntegratorSa;
    }

    rtDW.Integrator_PrevResetState = static_cast<int8_T>(rtb_NOT);

    // Update for DiscreteIntegrator: '<S49>/Integrator' incorporates:
    //   DiscreteIntegrator: '<S108>/Integrator'

    rtDW.Integrator_DSTATE_m += rtP.Integrator_gainval_cz * rtb_Switch;
    if (rtDW.Integrator_DSTATE_m > rtP.PIDController_UpperIntegrator_h) {
      rtDW.Integrator_DSTATE_m = rtP.PIDController_UpperIntegrator_h;
    } else if (rtDW.Integrator_DSTATE_m < rtP.PIDController_LowerIntegrator_j) {
      rtDW.Integrator_DSTATE_m = rtP.PIDController_LowerIntegrator_j;
    }

    rtDW.Integrator_PrevResetState_o = static_cast<int8_T>(rtb_NOT);

    // End of Update for DiscreteIntegrator: '<S49>/Integrator'
  }

  rate_scheduler((&rtM));
}

// Model initialize function
void PlatformController::initialize()
{
  // Start for RateTransition: '<S5>/Rate Transition1'
  rtDW.RateTransition1[0] = rtP.RateTransition1_InitialConditio;
  rtDW.RateTransition1[1] = rtP.RateTransition1_InitialConditio;
  rtDW.RateTransition1[2] = rtP.RateTransition1_InitialConditio;

  // InitializeConditions for DiscreteIntegrator: '<S164>/Integrator'
  rtDW.Integrator_DSTATE[0] = rtP.PIDController_InitialConditionF;

  // InitializeConditions for RateTransition: '<S5>/Rate Transition1'
  rtDW.RateTransition1_Buffer0[0] = rtP.RateTransition1_InitialConditio;

  // InitializeConditions for DiscreteIntegrator: '<S164>/Integrator'
  rtDW.Integrator_DSTATE[1] = rtP.PIDController_InitialConditionF;

  // InitializeConditions for RateTransition: '<S5>/Rate Transition1'
  rtDW.RateTransition1_Buffer0[1] = rtP.RateTransition1_InitialConditio;

  // InitializeConditions for DiscreteIntegrator: '<S164>/Integrator'
  rtDW.Integrator_DSTATE[2] = rtP.PIDController_InitialConditionF;

  // InitializeConditions for RateTransition: '<S5>/Rate Transition1'
  rtDW.RateTransition1_Buffer0[2] = rtP.RateTransition1_InitialConditio;

  // InitializeConditions for DiscreteIntegrator: '<S108>/Integrator'
  rtDW.Integrator_DSTATE_a = rtP.PIDController_InitialConditio_c;

  // InitializeConditions for DiscreteIntegrator: '<S49>/Integrator'
  rtDW.Integrator_DSTATE_m = rtP.PIDController_InitialConditio_m;
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

//extern "C"
//{
//  // Test if value is infinite
//  static boolean_T rtIsInf(real_T value)
//  {
//    return std::isinf(value);
//  }
//
//  // Test if single-precision value is infinite
//  static boolean_T rtIsInfF(real32_T value)
//  {
//    return std::isinf(value);
//  }
//
//  // Test if value is not a number
//  static boolean_T rtIsNaN(real_T value)
//  {
//    return std::isnan(value);
//  }
//
//  // Test if single-precision value is not a number
//  static boolean_T rtIsNaNF(real32_T value)
//  {
//    return std::isnan(value);
//  }
//}

//
// File trailer for generated code.
//
// [EOF]
//
