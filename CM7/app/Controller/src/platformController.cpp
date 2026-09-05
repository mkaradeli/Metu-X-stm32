//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: platformController.cpp
//
// Code generated for Simulink model 'platformController'.
//
// Model version                  : 1.69
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Fri Sep  4 20:56:27 2026
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
                                          //    '<S127>/Constant'
                                          //    '<S127>/Gain3'
                                          //    '<S128>/Gain4'
                                          //    '<S128>/Gain5'
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
}

// Model step function
void PlatformController::step()
{
  real_T rtb_Diff;
  real_T rtb_Diff_n;
  real_T rtb_Divide;
  real_T rtb_MathFunction_l;
  real_T rtb_Rads_idx_0;
  real_T rtb_Rads_idx_1;
  real_T rtb_Saturation1;
  real_T rtb_Saturation1_tmp;
  real_T rtb_Sqrt;
  real_T rtb_Sum;
  real_T rtb_SumI1;
  real_T rtb_Sum_h;
  real_T rtb_Sum_i_idx_0;
  real_T rtb_Sum_l;
  real_T rtb_Switch2;
  real_T rtb_Switch2_o;
  real_T rtb_error_idx_0;
  real_T rtb_error_idx_1;
  real_T tmp_0;
  int8_T tmp_1;
  int8_T tmp_2;
  boolean_T rtb_NOT;
  boolean_T tmp;
  rtb_NOT = ((&rtM)->Timing.TaskCounters.TID[1] == 0);
  if (rtb_NOT && ((&rtM)->Timing.TaskCounters.TID[2] == 0)) {
    // RateTransition: '<S5>/Rate Transition' incorporates:
    //   Bias: '<S5>/Bias'
    //   Gain: '<S5>/Gain'
    //   Gain: '<S5>/Gain1'
    //   Gain: '<S5>/Gain2'
    //   Inport: '<Root>/quaternion'
    //   Math: '<S5>/Square'
    //   Math: '<S5>/Square1'
    //   Product: '<S5>/Product'
    //   Product: '<S5>/Product1'
    //   Product: '<S5>/Product2'
    //   Product: '<S5>/Product3'
    //   Sum: '<S5>/Subtract'
    //   Sum: '<S5>/Subtract1'
    //   Sum: '<S5>/Subtract2'

    rtDW.RateTransition_Buffer[0] = (rtU.quaternion[0] * rtU.quaternion[2] -
      rtU.quaternion[1] * rtU.quaternion[3]) * 2.0;
    rtDW.RateTransition_Buffer[1] = (rtU.quaternion[1] * rtU.quaternion[2] +
      rtU.quaternion[0] * rtU.quaternion[3]) * 2.0;
    rtDW.RateTransition_Buffer[2] = (rtU.quaternion[0] * rtU.quaternion[0] +
      rtU.quaternion[1] * rtU.quaternion[1]) * -2.0 + 1.0;
  }

  // RateTransition: '<S5>/Rate Transition' incorporates:
  //   RateTransition: '<S5>/Rate Transition1'
  //
  tmp = ((&rtM)->Timing.TaskCounters.TID[2] == 0);
  if (tmp) {
    // Gain: '<S127>/Gain3' incorporates:
    //   Sum: '<S129>/Sum'

    rtb_Rads_idx_1 = (0.0 - rtDW.RateTransition_Buffer[1]) *
      platform_targets.attitude.Kp_att;
    rtb_Rads_idx_0 = rtb_Rads_idx_1;

    // DotProduct: '<S127>/Dot Product'
    rtb_error_idx_0 = rtb_Rads_idx_1 * rtb_Rads_idx_1;

    // Gain: '<S127>/Gain3'
    rtb_Rads_idx_1 = platform_targets.attitude.Kp_att *
      rtDW.RateTransition_Buffer[0];

    // Product: '<S127>/Divide' incorporates:
    //   Constant: '<S127>/Constant'
    //   DotProduct: '<S127>/Dot Product'
    //   MinMax: '<S127>/Min'
    //   Sqrt: '<S127>/Sqrt'

    rtb_Divide = platform_targets.attitude.w_sat / std::fmax
      (platform_targets.attitude.w_sat, std::sqrt(rtb_Rads_idx_1 *
        rtb_Rads_idx_1 + rtb_error_idx_0));

    // Product: '<S127>/Product4' incorporates:
    //   Gain: '<S127>/Gain3'

    rtb_Rads_idx_0 *= rtb_Divide;
    rtb_Rads_idx_1 *= rtb_Divide;
  }

  if (rtb_NOT) {
    // RateTransition: '<S5>/Rate Transition1'
    if (tmp) {
      // RateTransition: '<S5>/Rate Transition1'
      rtDW.RateTransition1[0] = rtDW.RateTransition1_Buffer0[0];
      rtDW.RateTransition1[1] = rtDW.RateTransition1_Buffer0[1];
      rtDW.RateTransition1[2] = rtDW.RateTransition1_Buffer0[2];
    }

    // Sum: '<S128>/Subtract3' incorporates:
    //   Inport: '<Root>/gyro_x'
    //   Inport: '<Root>/gyro_y'

    rtb_error_idx_0 = rtDW.RateTransition1[0] - rtU.gyro_x;
    rtb_error_idx_1 = rtDW.RateTransition1[1] - rtU.gyro_y;

    // Sum: '<S174>/Sum' incorporates:
    //   DiscreteIntegrator: '<S165>/Integrator'
    //   Gain: '<S170>/Proportional Gain'

    rtb_Divide = platform_targets.attitude.Kp_rate * rtb_error_idx_0 +
      rtDW.Integrator_DSTATE[0];
    rtb_Sum_i_idx_0 = rtb_Divide;

    // Outport: '<Root>/Mxyz'
    rtY.Mxyz[0] = rtb_Divide;

    // Sum: '<S174>/Sum' incorporates:
    //   DiscreteIntegrator: '<S165>/Integrator'
    //   Gain: '<S170>/Proportional Gain'

    rtb_Divide = platform_targets.attitude.Kp_rate * rtb_error_idx_1 +
      rtDW.Integrator_DSTATE[1];

    // Outport: '<Root>/Mxyz' incorporates:
    //   DiscreteIntegrator: '<S165>/Integrator'
    //   Gain: '<S170>/Proportional Gain'
    //   Sum: '<S128>/Subtract3'
    //   Sum: '<S174>/Sum'

    rtY.Mxyz[1] = rtb_Divide;
    rtY.Mxyz[2] = platform_targets.attitude.Kp_rate * rtDW.RateTransition1[2] +
      rtDW.Integrator_DSTATE[2];

    // Gain: '<S128>/Gain5' incorporates:
    //   Gain: '<S128>/Gain4'
    //   Sum: '<S174>/Sum'

    rtb_Saturation1_tmp = 1.0 / (2.0 * platform_targets.attitude.r);
    rtb_Saturation1 = rtb_Saturation1_tmp * rtb_Divide;

    // Bias: '<S4>/Bias' incorporates:
    //   Gain: '<S4>/Gain'
    //   Inport: '<Root>/ManifoldPressure'

    rtb_Divide = 0.0095 * rtU.ManifoldPressure + 31.5;

    // Bias: '<S4>/Bias5' incorporates:
    //   Gain: '<S4>/Gain3'
    //   Inport: '<Root>/quaternion'
    //   Math: '<S4>/Square'
    //   Math: '<S4>/Square1'
    //   Sum: '<S4>/Sum1'

    rtb_MathFunction_l = (rtU.quaternion[0] * rtU.quaternion[0] +
                          rtU.quaternion[1] * rtU.quaternion[1]) * -2.0 + 1.0;

    // Saturate: '<S4>/Saturation4'
    if (rtb_MathFunction_l < 0.7) {
      // Saturate: '<S3>/Saturation4'
      rtb_MathFunction_l = 0.7;
    }

    // Math: '<S4>/Math Function' incorporates:
    //   Saturate: '<S4>/Saturation4'
    //
    //  About '<S4>/Math Function':
    //   Operator: reciprocal

    rtb_MathFunction_l = 1.0 / rtb_MathFunction_l;

    // Bias: '<S4>/Bias4' incorporates:
    //   Inport: '<Root>/T_max_allowed'
    //   Product: '<S4>/Product2'

    rtb_Diff = rtU.T_max_allowed / rtb_Divide / rtb_MathFunction_l - 9.81;

    // Gain: '<S4>/Gain5' incorporates:
    //   Bias: '<S4>/Bias1'
    //   Inport: '<Root>/Height'
    //   UnaryMinus: '<S4>/Unary Minus1'

    rtb_Sum = (-rtU.Height + platform_targets.hover.h_ref) *
      platform_targets.hover.Kh;

    // Saturate: '<S4>/Saturation1'
    if (rtb_Sum > platform_targets.hover.v_sat[1]) {
      rtb_Sum = platform_targets.hover.v_sat[1];
    } else if (rtb_Sum < platform_targets.hover.v_sat[0]) {
      rtb_Sum = platform_targets.hover.v_sat[0];
    }

    // Sum: '<S4>/Sum' incorporates:
    //   Inport: '<Root>/Velocity'
    //   Saturate: '<S4>/Saturation1'

    rtb_Sum_h = rtb_Sum - rtU.Velocity;

    // Logic: '<S4>/NOT' incorporates:
    //   Inport: '<Root>/Dropped'

    rtb_NOT = !rtU.Dropped;

    // DiscreteIntegrator: '<S108>/Integrator'
    if (rtb_NOT && (rtDW.Integrator_PrevResetState <= 0)) {
      rtDW.Integrator_DSTATE_a = -9.81;
    }

    // Gain: '<S4>/Gain1' incorporates:
    //   Inport: '<Root>/Velocity'
    //   UnaryMinus: '<S4>/Unary Minus'

    rtb_Sum = platform_targets.hover.Kh * -rtU.Velocity;

    // Saturate: '<S4>/Saturation3'
    if (rtb_Sum > platform_targets.a_dec_low) {
      rtb_Sum = platform_targets.a_dec_low;
    } else if (rtb_Sum < -9.81) {
      rtb_Sum = -9.81;
    }

    // Sum: '<S118>/Sum' incorporates:
    //   Constant: '<S4>/Constant3'
    //   DiscreteIntegrator: '<S108>/Integrator'
    //   Product: '<S113>/PProd Out'
    //   Saturate: '<S4>/Saturation3'

    rtb_Sum += rtb_Sum_h * platform_targets.altitude.kP_hover +
      rtDW.Integrator_DSTATE_a;

    // Switch: '<S116>/Switch2' incorporates:
    //   Constant: '<S4>/Constant1'
    //   RelationalOperator: '<S116>/LowerRelop1'
    //   RelationalOperator: '<S116>/UpperRelop'
    //   Switch: '<S116>/Switch'

    if (rtb_Sum > rtb_Diff) {
      rtb_Switch2 = rtb_Diff;
    } else if (rtb_Sum < -9.81) {
      // Switch: '<S116>/Switch' incorporates:
      //   Constant: '<S4>/Constant1'

      rtb_Switch2 = -9.81;
    } else {
      rtb_Switch2 = rtb_Sum;
    }

    // End of Switch: '<S116>/Switch2'

    // Bias: '<S3>/Bias4'
    rtb_Diff_n = rtb_Diff;

    // Sqrt: '<S3>/Sqrt' incorporates:
    //   Bias: '<S3>/Bias1'
    //   Bias: '<S3>/Bias2'
    //   Gain: '<S3>/Gain2'
    //   Inport: '<Root>/Height'
    //   Saturate: '<S3>/Saturation1'

    rtb_Sqrt = std::sqrt(2.0 * platform_targets.a_dec * std::fmax(rtU.Height -
      platform_targets.h_cut, 0.0) + platform_targets.V_td *
                         platform_targets.V_td);

    // Sum: '<S3>/Sum' incorporates:
    //   Inport: '<Root>/Velocity'
    //   Saturate: '<S3>/Saturation2'
    //   UnaryMinus: '<S3>/Unary Minus'

    rtb_Sum_l = -std::fmin(rtb_Sqrt, platform_targets.V_max) - rtU.Velocity;

    // DiscreteIntegrator: '<S49>/Integrator' incorporates:
    //   Logic: '<S3>/NOT'

    if (rtb_NOT && (rtDW.Integrator_PrevResetState_o <= 0)) {
      rtDW.Integrator_DSTATE_m = 0.0;
    }

    // Switch: '<S3>/Switch' incorporates:
    //   Abs: '<S3>/Abs1'
    //   Constant: '<S3>/Constant'
    //   Constant: '<S6>/Constant'
    //   Gain: '<S3>/Gain1'
    //   Inport: '<Root>/Velocity'
    //   Product: '<S3>/Divide'
    //   RelationalOperator: '<S6>/Compare'

    if (rtb_Sqrt < platform_targets.V_max) {
      rtb_SumI1 = std::abs(rtU.Velocity) / rtb_Sqrt * platform_targets.a_dec;
    } else {
      rtb_SumI1 = -9.8;
    }

    // End of Switch: '<S3>/Switch'

    // Sum: '<S59>/Sum' incorporates:
    //   Constant: '<S3>/Constant3'
    //   DiscreteIntegrator: '<S49>/Integrator'
    //   Product: '<S54>/PProd Out'
    //   Saturate: '<S3>/Saturation3'

    rtb_Sqrt = (rtb_Sum_l * platform_targets.altitude.kP +
                rtDW.Integrator_DSTATE_m) + std::fmin(rtb_SumI1,
      platform_targets.a_dec);

    // Switch: '<S57>/Switch2' incorporates:
    //   Bias: '<S3>/Bias4'
    //   Constant: '<S3>/Constant1'
    //   RelationalOperator: '<S57>/LowerRelop1'
    //   RelationalOperator: '<S57>/UpperRelop'
    //   Switch: '<S57>/Switch'

    if (rtb_Sqrt > rtb_Diff) {
      rtb_Switch2_o = rtb_Diff;
    } else if (rtb_Sqrt < -9.81) {
      // Switch: '<S57>/Switch' incorporates:
      //   Constant: '<S3>/Constant1'

      rtb_Switch2_o = -9.81;
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

      rtb_SumI1 = (rtb_Switch2 + 9.81) * rtb_Divide * rtb_MathFunction_l;

      // Switch: '<S70>/Switch2' incorporates:
      //   Constant: '<S4>/Constant5'
      //   Inport: '<Root>/T_max_allowed'
      //   RelationalOperator: '<S70>/LowerRelop1'
      //   RelationalOperator: '<S70>/UpperRelop'
      //   Switch: '<S70>/Switch'

      if (rtb_SumI1 > rtU.T_max_allowed) {
        rtb_SumI1 = rtU.T_max_allowed;
      } else if (rtb_SumI1 < 0.0) {
        // Switch: '<S70>/Switch' incorporates:
        //   Constant: '<S4>/Constant5'

        rtb_SumI1 = 0.0;
      }

      // End of Switch: '<S70>/Switch2'
      break;

     case mission_modes::DROP:
      // Switch: '<S3>/Switch1' incorporates:
      //   Constant: '<S3>/Constant8'
      //   Constant: '<S8>/Constant'
      //   Inport: '<Root>/Height'
      //   RelationalOperator: '<S8>/Compare'

      if (rtU.Height <= 0.25) {
        rtb_SumI1 = 0.0;
      } else {
        // Product: '<S3>/Product1' incorporates:
        //   Bias: '<S3>/Bias'
        //   Bias: '<S3>/Bias3'
        //   Math: '<S3>/Math Function'
        //
        //  About '<S3>/Math Function':
        //   Operator: reciprocal

        rtb_SumI1 = (rtb_Switch2_o + 9.81) * rtb_Divide * rtb_MathFunction_l;

        // Switch: '<S11>/Switch2' incorporates:
        //   Constant: '<S3>/Constant5'
        //   Inport: '<Root>/T_max_allowed'
        //   RelationalOperator: '<S11>/LowerRelop1'
        //   RelationalOperator: '<S11>/UpperRelop'
        //   Switch: '<S11>/Switch'

        if (rtb_SumI1 > rtU.T_max_allowed) {
          rtb_SumI1 = rtU.T_max_allowed;
        } else if (rtb_SumI1 < 0.0) {
          // Switch: '<S11>/Switch' incorporates:
          //   Constant: '<S3>/Constant5'

          rtb_SumI1 = 0.0;
        }

        // End of Switch: '<S11>/Switch2'
      }

      // End of Switch: '<S3>/Switch1'
      break;

     default:
      rtb_SumI1 = 0.0;
      break;
    }

    // End of MultiPortSwitch generated from: '<S2>/Multiport Switch'

    // Gain: '<S2>/Gain'
    rtb_SumI1 *= 0.25;

    // Saturate: '<S128>/Saturation3' incorporates:
    //   Gain: '<S128>/Gain7'

    if (-rtb_Saturation1 > 272.0) {
      tmp_0 = 272.0;
    } else if (-rtb_Saturation1 < -272.0) {
      tmp_0 = -272.0;
    } else {
      tmp_0 = -rtb_Saturation1;
    }

    // Outport: '<Root>/FrontThrustCmd' incorporates:
    //   Saturate: '<S128>/Saturation3'
    //   Sum: '<S2>/Sum'

    rtY.FrontThrustCmd = rtb_SumI1 + tmp_0;

    // Saturate: '<S128>/Saturation2'
    if (rtb_Saturation1 > 272.0) {
      rtb_Saturation1 = 272.0;
    } else if (rtb_Saturation1 < -272.0) {
      rtb_Saturation1 = -272.0;
    }

    // Outport: '<Root>/BackThrustCmd' incorporates:
    //   Saturate: '<S128>/Saturation2'
    //   Sum: '<S2>/Sum1'

    rtY.BackThrustCmd = rtb_SumI1 + rtb_Saturation1;

    // Gain: '<S128>/Gain4'
    rtb_Saturation1 = rtb_Saturation1_tmp * rtb_Sum_i_idx_0;

    // Saturate: '<S128>/Saturation1' incorporates:
    //   Gain: '<S128>/Gain6'

    if (-rtb_Saturation1 > 272.0) {
      tmp_0 = 272.0;
    } else if (-rtb_Saturation1 < -272.0) {
      tmp_0 = -272.0;
    } else {
      tmp_0 = -rtb_Saturation1;
    }

    // Outport: '<Root>/LeftThrustCmd' incorporates:
    //   Saturate: '<S128>/Saturation1'
    //   Sum: '<S2>/Sum3'

    rtY.LeftThrustCmd = rtb_SumI1 + tmp_0;

    // Saturate: '<S128>/Saturation'
    if (rtb_Saturation1 > 272.0) {
      rtb_Saturation1 = 272.0;
    } else if (rtb_Saturation1 < -272.0) {
      rtb_Saturation1 = -272.0;
    }

    // Outport: '<Root>/RightThrustCmd' incorporates:
    //   Saturate: '<S128>/Saturation'
    //   Sum: '<S2>/Sum2'

    rtY.RightThrustCmd = rtb_SumI1 + rtb_Saturation1;

    // Bias: '<S3>/Bias6' incorporates:
    //   Bias: '<S3>/Bias'
    //   Bias: '<S4>/Bias6'
    //   Inport: '<Root>/T_alloc_total'
    //   Math: '<S3>/Math Function'
    //   Product: '<S3>/Product3'
    //
    //  About '<S3>/Math Function':
    //   Operator: reciprocal

    rtb_Divide = rtU.T_alloc_total / rtb_Divide / rtb_MathFunction_l - 9.81;

    // Sum: '<S62>/SumI1' incorporates:
    //   Bias: '<S3>/Bias6'
    //   Constant: '<S3>/Constant4'
    //   Product: '<S46>/IProd Out'
    //   Sum: '<S61>/SumI3'

    rtb_SumI1 = (rtb_Divide - rtb_Switch2_o) + rtb_Sum_l *
      platform_targets.altitude.kI;

    // Switch: '<S41>/Switch' incorporates:
    //   Bias: '<S3>/Bias4'
    //   RelationalOperator: '<S41>/u_GTE_up'

    if (rtb_Sqrt < rtb_Diff) {
      // Switch: '<S41>/Switch1' incorporates:
      //   Constant: '<S3>/Constant1'
      //   RelationalOperator: '<S41>/u_GT_lo'

      if (rtb_Sqrt > -9.81) {
        rtb_Diff_n = rtb_Sqrt;
      } else {
        rtb_Diff_n = -9.81;
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

    if (rtb_Diff_n > 0.0) {
      tmp_1 = 1;
    } else {
      tmp_1 = -1;
    }

    // Switch: '<S38>/Switch2' incorporates:
    //   Constant: '<S38>/Clamping_zero'
    //   Constant: '<S38>/Constant3'
    //   Constant: '<S38>/Constant4'
    //   RelationalOperator: '<S38>/fix for DT propagation issue1'

    if (rtb_SumI1 > 0.0) {
      tmp_2 = 1;
    } else {
      tmp_2 = -1;
    }

    // Switch: '<S38>/Switch' incorporates:
    //   Constant: '<S38>/Clamping_zero'
    //   Constant: '<S38>/Constant1'
    //   Logic: '<S38>/AND3'
    //   RelationalOperator: '<S38>/Equal1'
    //   RelationalOperator: '<S38>/Relational Operator'
    //   Switch: '<S38>/Switch1'
    //   Switch: '<S38>/Switch2'

    if ((rtb_Diff_n != 0.0) && (tmp_1 == tmp_2)) {
      rtb_Sum_i_idx_0 = 0.0;
    } else {
      rtb_Sum_i_idx_0 = rtb_SumI1;
    }

    // End of Switch: '<S38>/Switch'

    // Sum: '<S121>/SumI1' incorporates:
    //   Constant: '<S4>/Constant4'
    //   Product: '<S105>/IProd Out'
    //   Sum: '<S120>/SumI3'

    rtb_SumI1 = platform_targets.altitude.kI_hover * 2.0 * rtb_Sum_h +
      (rtb_Divide - rtb_Switch2);

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
      tmp_1 = 1;
    } else {
      tmp_1 = -1;
    }

    // Switch: '<S97>/Switch2' incorporates:
    //   Constant: '<S97>/Clamping_zero'
    //   Constant: '<S97>/Constant3'
    //   Constant: '<S97>/Constant4'
    //   RelationalOperator: '<S97>/fix for DT propagation issue1'

    if (rtb_SumI1 > 0.0) {
      tmp_2 = 1;
    } else {
      tmp_2 = -1;
    }

    // Switch: '<S97>/Switch' incorporates:
    //   Constant: '<S97>/Clamping_zero'
    //   Constant: '<S97>/Constant1'
    //   Logic: '<S97>/AND3'
    //   RelationalOperator: '<S97>/Equal1'
    //   RelationalOperator: '<S97>/Relational Operator'
    //   Switch: '<S97>/Switch1'
    //   Switch: '<S97>/Switch2'

    if ((rtb_Diff != 0.0) && (tmp_1 == tmp_2)) {
      rtb_MathFunction_l = 0.0;
    } else {
      rtb_MathFunction_l = rtb_SumI1;
    }

    // End of Switch: '<S97>/Switch'

    // Update for DiscreteIntegrator: '<S165>/Integrator' incorporates:
    //   Gain: '<S162>/Integral Gain'
    //   Sum: '<S128>/Subtract3'

    rtDW.Integrator_DSTATE[0] += platform_targets.attitude.Ki_rate *
      rtb_error_idx_0 * 0.005;
    rtDW.Integrator_DSTATE[1] += platform_targets.attitude.Ki_rate *
      rtb_error_idx_1 * 0.005;
    rtDW.Integrator_DSTATE[2] += platform_targets.attitude.Ki_rate *
      rtDW.RateTransition1[2] * 0.005;

    // Update for DiscreteIntegrator: '<S108>/Integrator'
    rtDW.Integrator_DSTATE_a += 0.005 * rtb_MathFunction_l;
    if (rtDW.Integrator_DSTATE_a > 3.0) {
      rtDW.Integrator_DSTATE_a = 3.0;
    } else if (rtDW.Integrator_DSTATE_a < -10.0) {
      rtDW.Integrator_DSTATE_a = -10.0;
    }

    rtDW.Integrator_PrevResetState = static_cast<int8_T>(rtb_NOT);

    // End of Update for DiscreteIntegrator: '<S108>/Integrator'

    // Update for DiscreteIntegrator: '<S49>/Integrator'
    rtDW.Integrator_DSTATE_m += 0.005 * rtb_Sum_i_idx_0;
    if (rtDW.Integrator_DSTATE_m > 3.0) {
      rtDW.Integrator_DSTATE_m = 3.0;
    } else if (rtDW.Integrator_DSTATE_m < -3.0) {
      rtDW.Integrator_DSTATE_m = -3.0;
    }

    rtDW.Integrator_PrevResetState_o = static_cast<int8_T>(rtb_NOT);

    // End of Update for DiscreteIntegrator: '<S49>/Integrator'
  }

  // Update for RateTransition: '<S5>/Rate Transition1'
  if (tmp) {
    rtDW.RateTransition1_Buffer0[0] = rtb_Rads_idx_0;
    rtDW.RateTransition1_Buffer0[1] = rtb_Rads_idx_1;
    rtDW.RateTransition1_Buffer0[2] = 0.0;
  }

  rate_scheduler((&rtM));
}

// Model initialize function
void PlatformController::initialize()
{
  // InitializeConditions for DiscreteIntegrator: '<S108>/Integrator'
  rtDW.Integrator_DSTATE_a = -9.81;
  rtDW.Integrator_PrevResetState = 2;

  // InitializeConditions for DiscreteIntegrator: '<S49>/Integrator'
  rtDW.Integrator_PrevResetState_o = 2;
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
