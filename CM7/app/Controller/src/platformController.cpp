//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: platformController.cpp
//
// Code generated for Simulink model 'platformController'.
//
// Model version                  : 1.116
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Thu Sep 17 01:17:07 2026
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
struct_RzX5A87yYhLUhlAxm0ffzC platform_targets{
  0.3,
  0.1,
  1.0,
  3.7,
  5.0,
  5.0,
  0.02,
  50.0,

  {
    9.9,
    24.6,
    18.769230769230766,
    78.285338593030872
  },

  {
    3.0,
    4.5,
    8.0,

    { -1.0, 2.2 }
  },

  {
    4.0,
    1.5,
    0.66,
    40.3,
    105.0,
    0.33,

    { 1.0, 0.0, -0.33, 0.0, 1.0, 0.0, 0.33, 0.0, 1.0, 0.33, 0.0, 0.0, 1.0, -0.33,
      0.0, 0.0 },

    { 0.0, 0.0, 0.0 }
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
                                          //    '<S4>/Constant3'
                                          //    '<S4>/Constant4'
                                          //    '<S4>/Gain1'
                                          //    '<S4>/Gain5'
                                          //    '<S4>/Saturation1'
                                          //    '<S4>/Saturation3'
                                          //    '<S8>/Constant'
                                          //    '<S10>/Constant'
                                          //    '<S74>/Constant6'
                                          //    '<S138>/Gain3'
                                          //    '<S139>/Gain4'
                                          //    '<S139>/Gain5'
                                          //    '<S168>/Kb'
                                          //    '<S173>/Integral Gain'
                                          //    '<S181>/Proportional Gain'


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
  if ((rtM->Timing.TaskCounters.TID[2]) > 19) {// Sample time: [0.02s, 0.0s]
    rtM->Timing.TaskCounters.TID[2] = 0;
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
  real_T rtb_Product1_d;
  real_T rtb_RateTransition;
  real_T rtb_RateTransition2;
  real_T rtb_RateTransition3;
  real_T rtb_RateTransition4;
  real_T rtb_RateTransition6_idx_0;
  real_T rtb_RateTransition6_idx_1;
  real_T rtb_Saturation1;
  real_T rtb_Saturation3;
  real_T rtb_Sqrt;
  real_T rtb_Sum;
  real_T rtb_Sum_d;
  real_T rtb_Sum_f;
  real_T rtb_Sum_o;
  real_T rtb_Switch2;
  real_T rtb_Switch2_j;
  real_T rtb_UnaryMinus_l;
  real_T rtb_error_idx_0;
  real_T rtb_error_idx_1;
  real_T rtb_error_idx_2;
  int8_T tmp_0;
  int8_T tmp_1;
  boolean_T rtb_AND;
  boolean_T rtb_AND_d;
  boolean_T rtb_LogicalOperator4;
  boolean_T rtb_LogicalOperator4_o;
  boolean_T rtb_NOT;
  boolean_T rtb_RateTransition5;
  boolean_T tmp;

  // RateTransition: '<S2>/Rate Transition4' incorporates:
  //   RateTransition: '<S2>/Rate Transition1'
  //   RateTransition: '<S2>/Rate Transition2'
  //   RateTransition: '<S2>/Rate Transition3'
  //   RateTransition: '<S2>/Rate Transition5'
  //   RateTransition: '<S2>/Rate Transition6'

  rtb_LogicalOperator4_o = ((&rtM)->Timing.TaskCounters.TID[1] == 0);
  if (rtb_LogicalOperator4_o) {
    // Gain: '<S138>/Gain3' incorporates:
    //   Inport: '<Root>/quaternion'
    //   Inport: '<Root>/quaternion_bias'
    //   Sum: '<S138>/Sum'
    //   UnaryMinus: '<S138>/Unary Minus'

    rtb_error_idx_2 = -(rtU.quaternion[0] - rtU.quaternion_bias[0]) *
      platform_targets.attitude.Kp_att;

    // Switch: '<S140>/Switch2' incorporates:
    //   Constant: '<S138>/Constant'
    //   Constant: '<S138>/Constant1'
    //   RelationalOperator: '<S140>/LowerRelop1'
    //   RelationalOperator: '<S140>/UpperRelop'
    //   Switch: '<S140>/Switch'

    if (rtb_error_idx_2 > 0.52) {
      rtb_error_idx_2 = 0.52;
    } else if (rtb_error_idx_2 < -0.52) {
      // Switch: '<S140>/Switch' incorporates:
      //   Constant: '<S138>/Constant1'

      rtb_error_idx_2 = -0.52;
    }

    rtb_error_idx_0 = rtb_error_idx_2;

    // Outport: '<Root>/omega_demand'
    rtY.omega_demand[0] = rtb_error_idx_2;

    // Gain: '<S138>/Gain3' incorporates:
    //   Inport: '<Root>/quaternion'
    //   Inport: '<Root>/quaternion_bias'
    //   Sum: '<S138>/Sum'
    //   UnaryMinus: '<S138>/Unary Minus'

    rtb_error_idx_2 = -(rtU.quaternion[1] - rtU.quaternion_bias[1]) *
      platform_targets.attitude.Kp_att;

    // Switch: '<S140>/Switch2' incorporates:
    //   Constant: '<S138>/Constant'
    //   Constant: '<S138>/Constant1'
    //   RelationalOperator: '<S140>/LowerRelop1'
    //   RelationalOperator: '<S140>/UpperRelop'
    //   Switch: '<S140>/Switch'

    if (rtb_error_idx_2 > 0.52) {
      rtb_error_idx_2 = 0.52;
    } else if (rtb_error_idx_2 < -0.52) {
      // Switch: '<S140>/Switch' incorporates:
      //   Constant: '<S138>/Constant1'

      rtb_error_idx_2 = -0.52;
    }

    rtb_error_idx_1 = rtb_error_idx_2;

    // Outport: '<Root>/omega_demand'
    rtY.omega_demand[1] = rtb_error_idx_2;

    // Gain: '<S138>/Gain3' incorporates:
    //   Inport: '<Root>/quaternion'
    //   Inport: '<Root>/quaternion_bias'
    //   Sum: '<S138>/Sum'
    //   UnaryMinus: '<S138>/Unary Minus'

    rtb_error_idx_2 = -(rtU.quaternion[2] - rtU.quaternion_bias[2]) *
      platform_targets.attitude.Kp_att;

    // Switch: '<S140>/Switch2' incorporates:
    //   Constant: '<S138>/Constant'
    //   Constant: '<S138>/Constant1'
    //   RelationalOperator: '<S140>/LowerRelop1'
    //   RelationalOperator: '<S140>/UpperRelop'
    //   Switch: '<S140>/Switch'

    if (rtb_error_idx_2 > 0.52) {
      rtb_error_idx_2 = 0.52;
    } else if (rtb_error_idx_2 < -0.52) {
      // Switch: '<S140>/Switch' incorporates:
      //   Constant: '<S138>/Constant1'

      rtb_error_idx_2 = -0.52;
    }

    // Outport: '<Root>/omega_demand'
    rtY.omega_demand[2] = rtb_error_idx_2;

    // RateTransition: '<S2>/Rate Transition' incorporates:
    //   Inport: '<Root>/T_max_allowed'

    if ((&rtM)->Timing.TaskCounters.TID[2] == 0) {
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

  tmp = ((&rtM)->Timing.TaskCounters.TID[2] == 0);
  if (tmp) {
    rtb_RateTransition = rtDW.RateTransition_Buffer;
  }

  // RateTransition: '<S2>/Rate Transition4' incorporates:
  //   Inport: '<Root>/ManifoldPressure'

  if (rtb_LogicalOperator4_o && tmp) {
    rtDW.RateTransition4_Buffer = rtU.ManifoldPressure;
  }

  if (tmp) {
    rtb_RateTransition4 = rtDW.RateTransition4_Buffer;

    // Bias: '<S4>/Bias' incorporates:
    //   Gain: '<S4>/Gain'

    rtb_Bias = 0.0095 * rtDW.RateTransition4_Buffer + 31.5;
  }

  // RateTransition: '<S2>/Rate Transition6' incorporates:
  //   Inport: '<Root>/quaternion'

  if (rtb_LogicalOperator4_o && tmp) {
    rtDW.RateTransition6_Buffer[0] = rtU.quaternion[0];
    rtDW.RateTransition6_Buffer[1] = rtU.quaternion[1];
    rtDW.RateTransition6_Buffer[2] = rtU.quaternion[2];
    rtDW.RateTransition6_Buffer[3] = rtU.quaternion[3];
  }

  if (tmp) {
    rtb_RateTransition6_idx_0 = rtDW.RateTransition6_Buffer[0];
    rtb_RateTransition6_idx_1 = rtDW.RateTransition6_Buffer[1];

    // Bias: '<S4>/Bias5' incorporates:
    //   Gain: '<S4>/Gain3'
    //   Math: '<S4>/Square'
    //   Math: '<S4>/Square1'
    //   Sum: '<S4>/Sum1'

    rtb_Product1_d = (rtDW.RateTransition6_Buffer[0] *
                      rtDW.RateTransition6_Buffer[0] +
                      rtDW.RateTransition6_Buffer[1] *
                      rtDW.RateTransition6_Buffer[1]) * -2.0 + 1.0;

    // Saturate: '<S4>/Saturation4'
    if (rtb_Product1_d < 0.7) {
      rtb_Product1_d = 0.7;
    }

    // Math: '<S4>/Math Function' incorporates:
    //   Saturate: '<S4>/Saturation4'
    //
    //  About '<S4>/Math Function':
    //   Operator: reciprocal

    rtb_MathFunction = 1.0 / rtb_Product1_d;

    // Bias: '<S4>/Bias4' incorporates:
    //   Product: '<S4>/Product2'

    rtb_Diff = rtb_RateTransition / rtb_Bias / rtb_MathFunction - 9.81;
  }

  // RateTransition: '<S2>/Rate Transition3' incorporates:
  //   Inport: '<Root>/Velocity'

  if (rtb_LogicalOperator4_o && tmp) {
    rtDW.RateTransition3_Buffer = rtU.Velocity;
  }

  if (tmp) {
    rtb_RateTransition3 = rtDW.RateTransition3_Buffer;
  }

  // RateTransition: '<S2>/Rate Transition2' incorporates:
  //   Inport: '<Root>/Height'

  if (rtb_LogicalOperator4_o && tmp) {
    rtDW.RateTransition2_Buffer = rtU.Height;
  }

  if (tmp) {
    rtb_RateTransition2 = rtDW.RateTransition2_Buffer;
  }

  // RateTransition: '<S2>/Rate Transition5' incorporates:
  //   Inport: '<Root>/Dropped'
  //   Logic: '<S136>/Logical Operator3'
  //   Switch: '<S136>/Switch'

  if (rtb_LogicalOperator4_o && tmp) {
    rtDW.RateTransition5_Buffer = rtU.Dropped;
  }

  if (tmp) {
    rtb_RateTransition5 = rtDW.RateTransition5_Buffer;

    // DiscreteIntegrator: '<S135>/Discrete-Time Integrator' incorporates:
    //   DiscreteIntegrator: '<S73>/Discrete-Time Integrator'
    //   Logic: '<S3>/NOT'
    //   Logic: '<S4>/NOT'

    rtb_NOT = !rtDW.RateTransition5_Buffer;
    if ((rtDW.RateTransition5_Buffer && (rtDW.DiscreteTimeIntegrator_PrevRese <=
          0)) || (rtb_NOT && (rtDW.DiscreteTimeIntegrator_PrevRese == 1))) {
      rtDW.DiscreteTimeIntegrator_DSTATE = 0.0;
    }

    // Logic: '<S135>/AND' incorporates:
    //   Constant: '<S137>/Constant'
    //   DiscreteIntegrator: '<S135>/Discrete-Time Integrator'
    //   RelationalOperator: '<S137>/Compare'

    rtb_AND = ((rtDW.DiscreteTimeIntegrator_DSTATE <= 4.0) &&
               rtDW.RateTransition5_Buffer);

    // Logic: '<S136>/Logical Operator4' incorporates:
    //   Logic: '<S136>/Logical Operator5'
    //   RelationalOperator: '<S132>/FixPt Relational Operator'
    //   UnitDelay: '<S132>/Delay Input1'
    //   UnitDelay: '<S136>/Unit Delay'
    //
    //  Block description for '<S132>/Delay Input1':
    //
    //   Store in Global RAM

    rtb_LogicalOperator4 = ((static_cast<int32_T>(rtb_AND) > static_cast<int32_T>
      (rtDW.DelayInput1_DSTATE)) || (rtConstB.LogicalOperator &&
      rtDW.UnitDelay_DSTATE));

    // Outputs for Enabled SubSystem: '<S74>/Enabled Subsystem' incorporates:
    //   EnablePort: '<S133>/Enable'

    if (!rtb_LogicalOperator4) {
      // SignalConversion generated from: '<S133>/Height'
      rtDW.Height = rtb_RateTransition2;
    }

    // End of Outputs for SubSystem: '<S74>/Enabled Subsystem'

    // Switch: '<S74>/Switch' incorporates:
    //   Constant: '<S74>/Constant6'
    //   Logic: '<S136>/Logical Operator3'
    //   Switch: '<S136>/Switch'

    if (rtb_AND) {
      rtb_Product1_d = platform_targets.hover.h_ref;
    } else {
      rtb_Product1_d = rtDW.Height;
    }

    // Gain: '<S4>/Gain5' incorporates:
    //   Sum: '<S4>/Sum2'
    //   Switch: '<S74>/Switch'

    rtb_Saturation1 = (rtb_Product1_d - rtb_RateTransition2) *
      platform_targets.hover.Kh;

    // Saturate: '<S4>/Saturation1'
    if (rtb_Saturation1 > platform_targets.hover.v_sat[1]) {
      rtb_Saturation1 = platform_targets.hover.v_sat[1];
    } else if (rtb_Saturation1 < platform_targets.hover.v_sat[0]) {
      rtb_Saturation1 = platform_targets.hover.v_sat[0];
    }

    // End of Saturate: '<S4>/Saturation1'

    // Sum: '<S4>/Sum'
    rtb_Sum_o = rtb_Saturation1 - rtb_RateTransition3;

    // DiscreteIntegrator: '<S112>/Integrator'
    if (rtb_NOT || (rtDW.Integrator_PrevResetState != 0)) {
      rtDW.Integrator_DSTATE = -9.81;
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

    // Sum: '<S122>/Sum' incorporates:
    //   Constant: '<S4>/Constant3'
    //   DiscreteIntegrator: '<S112>/Integrator'
    //   Product: '<S117>/PProd Out'
    //   UnaryMinus: '<S106>/Unary Minus'

    rtb_Sum = (rtb_Sum_o * platform_targets.altitude.kP_hover +
               rtDW.Integrator_DSTATE) + rtb_Saturation3;

    // Switch: '<S120>/Switch2' incorporates:
    //   Constant: '<S4>/Constant1'
    //   RelationalOperator: '<S120>/LowerRelop1'
    //   RelationalOperator: '<S120>/UpperRelop'
    //   Switch: '<S120>/Switch'

    if (rtb_Sum > rtb_Diff) {
      rtb_Switch2 = rtb_Diff;
    } else if (rtb_Sum < -9.81) {
      // Switch: '<S120>/Switch' incorporates:
      //   Constant: '<S4>/Constant1'

      rtb_Switch2 = -9.81;
    } else {
      rtb_Switch2 = rtb_Sum;
    }

    // End of Switch: '<S120>/Switch2'

    // DiscreteIntegrator: '<S73>/Discrete-Time Integrator'
    if ((rtDW.RateTransition5_Buffer && (rtDW.DiscreteTimeIntegrator_PrevRe_j <=
          0)) || (rtb_NOT && (rtDW.DiscreteTimeIntegrator_PrevRe_j == 1))) {
      rtDW.DiscreteTimeIntegrator_DSTATE_j = 0.0;
    }

    // Bias: '<S3>/Bias' incorporates:
    //   Gain: '<S3>/Gain'

    rtb_Bias_e = 0.0095 * rtb_RateTransition4 + 31.5;

    // Bias: '<S3>/Bias5' incorporates:
    //   Gain: '<S3>/Gain3'
    //   Math: '<S3>/Square'
    //   Math: '<S3>/Square1'
    //   Sum: '<S3>/Sum1'

    rtb_Product1_d = (rtb_RateTransition6_idx_0 * rtb_RateTransition6_idx_0 +
                      rtb_RateTransition6_idx_1 * rtb_RateTransition6_idx_1) *
      -2.0 + 1.0;

    // Saturate: '<S3>/Saturation4'
    if (rtb_Product1_d < 0.7) {
      rtb_Product1_d = 0.7;
    }

    // Math: '<S3>/Math Function' incorporates:
    //   Saturate: '<S3>/Saturation4'
    //
    //  About '<S3>/Math Function':
    //   Operator: reciprocal

    rtb_MathFunction_n = 1.0 / rtb_Product1_d;

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

    // DiscreteIntegrator: '<S51>/Integrator'
    if (rtb_NOT || (rtDW.Integrator_PrevResetState_n != 0)) {
      rtDW.Integrator_DSTATE_p = 0.0;
    }

    // Switch: '<S3>/Switch' incorporates:
    //   Abs: '<S3>/Abs1'
    //   Constant: '<S3>/Constant'
    //   Constant: '<S8>/Constant'
    //   Gain: '<S3>/Gain1'
    //   Product: '<S3>/Divide'
    //   RelationalOperator: '<S8>/Compare'

    if (rtb_Sqrt < platform_targets.V_max) {
      rtb_Product1_d = std::abs(rtb_RateTransition3) / rtb_Sqrt *
        platform_targets.a_dec;
    } else {
      rtb_Product1_d = 0.0;
    }

    // End of Switch: '<S3>/Switch'

    // Saturate: '<S3>/Saturation3'
    rtb_Sqrt = std::fmin(rtb_Product1_d, platform_targets.a_dec);

    // Sum: '<S61>/Sum' incorporates:
    //   Constant: '<S3>/Constant2'
    //   Constant: '<S3>/Constant3'
    //   DiscreteIntegrator: '<S51>/Integrator'
    //   Product: '<S44>/DProd Out'
    //   Product: '<S56>/PProd Out'
    //   UnaryMinus: '<S45>/Unary Minus'

    rtb_Sum_f = (rtb_Sum_d * platform_targets.altitude.kP +
                 rtDW.Integrator_DSTATE_p) + -rtb_Sqrt * -0.9;

    // Switch: '<S59>/Switch2' incorporates:
    //   Constant: '<S3>/Constant1'
    //   RelationalOperator: '<S59>/LowerRelop1'
    //   RelationalOperator: '<S59>/UpperRelop'
    //   Switch: '<S59>/Switch'

    if (rtb_Sum_f > rtb_Diff_j) {
      rtb_Switch2_j = rtb_Diff_j;
    } else if (rtb_Sum_f < -9.81) {
      // Switch: '<S59>/Switch' incorporates:
      //   Constant: '<S3>/Constant1'

      rtb_Switch2_j = -9.81;
    } else {
      rtb_Switch2_j = rtb_Sum_f;
    }

    // End of Switch: '<S59>/Switch2'

    // MultiPortSwitch generated from: '<S2>/Multiport Switch' incorporates:
    //   Constant: '<S2>/Constant'
    //   Constant: '<S2>/Constant1'

    switch (mission_mode) {
     case mission_modes::HOVER:
      // Switch: '<S4>/Switch' incorporates:
      //   Constant: '<S131>/Constant'
      //   Constant: '<S4>/Constant6'
      //   DiscreteIntegrator: '<S73>/Discrete-Time Integrator'
      //   Logic: '<S73>/AND'
      //   RelationalOperator: '<S131>/Compare'

      if ((rtDW.DiscreteTimeIntegrator_DSTATE_j <= 5.0) &&
          rtDW.RateTransition5_Buffer) {
        // Product: '<S4>/Product1' incorporates:
        //   Bias: '<S4>/Bias3'

        rtb_Product1_d = (rtb_Switch2 + 9.81) * rtb_Bias * rtb_MathFunction;

        // Switch: '<S72>/Switch2' incorporates:
        //   Constant: '<S4>/Constant5'
        //   RelationalOperator: '<S72>/LowerRelop1'
        //   RelationalOperator: '<S72>/UpperRelop'
        //   Switch: '<S72>/Switch'

        if (rtb_Product1_d > rtb_RateTransition) {
          rtb_Product1_d = rtb_RateTransition;
        } else if (rtb_Product1_d < 0.0) {
          // Switch: '<S72>/Switch' incorporates:
          //   Constant: '<S4>/Constant5'

          rtb_Product1_d = 0.0;
        }

        // End of Switch: '<S72>/Switch2'
      } else {
        rtb_Product1_d = 0.0;
      }

      // End of Switch: '<S4>/Switch'
      break;

     case mission_modes::DROP:
      // Switch: '<S3>/Switch1' incorporates:
      //   Constant: '<S10>/Constant'
      //   Constant: '<S3>/Constant8'
      //   RelationalOperator: '<S10>/Compare'

      if (rtb_RateTransition2 >= platform_targets.h_cut) {
        // Product: '<S3>/Product1' incorporates:
        //   Bias: '<S3>/Bias3'

        rtb_Product1_d = (rtb_Switch2_j + 9.81) * rtb_Bias_e *
          rtb_MathFunction_n;

        // Switch: '<S13>/Switch2' incorporates:
        //   Constant: '<S3>/Constant5'
        //   RelationalOperator: '<S13>/LowerRelop1'
        //   RelationalOperator: '<S13>/UpperRelop'
        //   Switch: '<S13>/Switch'

        if (rtb_Product1_d > rtb_RateTransition) {
          rtb_Product1_d = rtb_RateTransition;
        } else if (rtb_Product1_d < 0.0) {
          // Switch: '<S13>/Switch' incorporates:
          //   Constant: '<S3>/Constant5'

          rtb_Product1_d = 0.0;
        }

        // End of Switch: '<S13>/Switch2'
      } else {
        rtb_Product1_d = 0.0;
      }

      // End of Switch: '<S3>/Switch1'
      break;

     default:
      rtb_Product1_d = 0.0;
      break;
    }

    // Gain: '<S2>/Gain1'
    rtb_Product1_d *= 0.25;

    // RelationalOperator: '<S6>/Compare' incorporates:
    //   Constant: '<S6>/Constant'

    rtDW.Compare = (rtb_Product1_d >= 40.0);

    // Gain: '<S2>/Gain'
    rtDW.Gain = rtb_Product1_d;

    // Outport: '<Root>/VerticalThrustCmd'
    rtY.VerticalThrustCmd = rtb_Product1_d;
  }

  // RateTransition: '<S2>/Rate Transition1' incorporates:
  //   Inport: '<Root>/T_alloc_total'

  if (rtb_LogicalOperator4_o) {
    // Logic: '<S194>/Logical Operator4' incorporates:
    //   Logic: '<S194>/Logical Operator5'
    //   UnitDelay: '<S194>/Unit Delay'

    rtb_LogicalOperator4_o = (rtDW.Compare || (rtConstB.LogicalOperator_d &&
      rtDW.UnitDelay_DSTATE_b));

    // Switch: '<S5>/Switch' incorporates:
    //   Constant: '<S5>/Constant'
    //   Switch: '<S194>/Switch'

    if (!rtb_LogicalOperator4_o) {
      rtb_error_idx_0 = 0.0;
      rtb_error_idx_1 = 0.0;
      rtb_error_idx_2 = 0.0;
    }

    // Sum: '<S139>/Subtract3' incorporates:
    //   Inport: '<Root>/gyro'
    //   Switch: '<S5>/Switch'

    rtb_error_idx_0 -= rtU.gyro[0];
    rtb_error_idx_1 -= rtU.gyro[1];
    rtb_error_idx_2 -= rtU.gyro[2];

    // Logic: '<S139>/AND' incorporates:
    //   Inport: '<Root>/Dropped'
    //   Switch: '<S194>/Switch'

    rtb_AND_d = (rtU.Dropped && rtb_LogicalOperator4_o);

    // DiscreteIntegrator: '<S176>/Integrator'
    if (rtb_AND_d && (rtDW.Integrator_PrevResetState_m <= 0)) {
      rtDW.Integrator_DSTATE_f[0] = 0.0;
      rtDW.Integrator_DSTATE_f[1] = 0.0;
      rtDW.Integrator_DSTATE_f[2] = 0.0;
    }

    // Gain: '<S168>/Kb'
    rtb_RateTransition2 = platform_targets.attitude.Ki_rate /
      platform_targets.attitude.Kp_rate;

    // Sum: '<S185>/Sum' incorporates:
    //   DiscreteIntegrator: '<S176>/Integrator'
    //   Gain: '<S181>/Proportional Gain'

    rtb_RateTransition4 = platform_targets.attitude.Kp_rate * rtb_error_idx_0 +
      rtDW.Integrator_DSTATE_f[0];

    // Saturate: '<S183>/Saturation'
    if (rtb_RateTransition4 > 16.5) {
      rtb_RateTransition6_idx_0 = 16.5;
    } else if (rtb_RateTransition4 < -16.5) {
      rtb_RateTransition6_idx_0 = -16.5;
    } else {
      rtb_RateTransition6_idx_0 = rtb_RateTransition4;
    }

    rtb_RateTransition = rtb_RateTransition6_idx_0;

    // Sum: '<S168>/SumI4' incorporates:
    //   Gain: '<S168>/Kb'
    //   Gain: '<S173>/Integral Gain'
    //   Gain: '<S181>/Proportional Gain'
    //   Sum: '<S168>/SumI2'

    rtb_error_idx_0 = (rtb_RateTransition6_idx_0 - rtb_RateTransition4) *
      rtb_RateTransition2 + platform_targets.attitude.Ki_rate * rtb_error_idx_0;

    // Sum: '<S185>/Sum' incorporates:
    //   DiscreteIntegrator: '<S176>/Integrator'
    //   Gain: '<S181>/Proportional Gain'

    rtb_RateTransition4 = platform_targets.attitude.Kp_rate * rtb_error_idx_1 +
      rtDW.Integrator_DSTATE_f[1];

    // Saturate: '<S183>/Saturation'
    if (rtb_RateTransition4 > 16.5) {
      rtb_RateTransition6_idx_0 = 16.5;
    } else if (rtb_RateTransition4 < -16.5) {
      rtb_RateTransition6_idx_0 = -16.5;
    } else {
      rtb_RateTransition6_idx_0 = rtb_RateTransition4;
    }

    // Sum: '<S168>/SumI4' incorporates:
    //   Gain: '<S168>/Kb'
    //   Gain: '<S173>/Integral Gain'
    //   Gain: '<S181>/Proportional Gain'
    //   Sum: '<S168>/SumI2'

    rtb_error_idx_1 = (rtb_RateTransition6_idx_0 - rtb_RateTransition4) *
      rtb_RateTransition2 + platform_targets.attitude.Ki_rate * rtb_error_idx_1;

    // Sum: '<S185>/Sum' incorporates:
    //   DiscreteIntegrator: '<S176>/Integrator'
    //   Gain: '<S181>/Proportional Gain'

    rtb_RateTransition4 = platform_targets.attitude.Kp_rate * rtb_error_idx_2 +
      rtDW.Integrator_DSTATE_f[2];

    // Saturate: '<S183>/Saturation'
    if (rtb_RateTransition4 > 16.5) {
      rtb_Product1_d = 16.5;
    } else if (rtb_RateTransition4 < -16.5) {
      rtb_Product1_d = -16.5;
    } else {
      rtb_Product1_d = rtb_RateTransition4;
    }

    // Sum: '<S168>/SumI4' incorporates:
    //   Gain: '<S168>/Kb'
    //   Gain: '<S173>/Integral Gain'
    //   Gain: '<S181>/Proportional Gain'
    //   Saturate: '<S183>/Saturation'
    //   Sum: '<S168>/SumI2'

    rtb_RateTransition4 = (rtb_Product1_d - rtb_RateTransition4) *
      rtb_RateTransition2 + platform_targets.attitude.Ki_rate * rtb_error_idx_2;

    // Gain: '<S139>/Gain4' incorporates:
    //   Gain: '<S139>/Gain5'

    rtb_Product1_d = 1.0 / (2.0 * platform_targets.attitude.r);
    rtb_RateTransition2 = rtb_Product1_d * rtb_RateTransition;

    // Saturate: '<S139>/Saturation'
    if (rtb_RateTransition2 > 272.0) {
      rtb_error_idx_2 = 272.0;
    } else if (rtb_RateTransition2 < -272.0) {
      rtb_error_idx_2 = -272.0;
    } else {
      rtb_error_idx_2 = rtb_RateTransition2;
    }

    // End of Saturate: '<S139>/Saturation'

    // Switch: '<S2>/Switch' incorporates:
    //   Constant: '<S2>/Constant4'

    if (rtb_LogicalOperator4_o) {
      rtb_RateTransition = rtDW.Gain;
    } else {
      rtb_RateTransition = 0.0;
    }

    // End of Switch: '<S2>/Switch'

    // Outport: '<Root>/Fy_pos' incorporates:
    //   Sum: '<S2>/Sum1'

    rtY.Fy_pos = rtb_RateTransition + rtb_error_idx_2;

    // Gain: '<S139>/Gain5'
    rtb_error_idx_2 = rtb_Product1_d * rtb_RateTransition6_idx_0;

    // Saturate: '<S139>/Saturation2'
    if (rtb_error_idx_2 > 272.0) {
      rtb_Product1_d = 272.0;
    } else if (rtb_error_idx_2 < -272.0) {
      rtb_Product1_d = -272.0;
    } else {
      rtb_Product1_d = rtb_error_idx_2;
    }

    // Outport: '<Root>/Fx_neg' incorporates:
    //   Saturate: '<S139>/Saturation2'
    //   Sum: '<S2>/Sum3'

    rtY.Fx_neg = rtb_RateTransition + rtb_Product1_d;

    // Saturate: '<S139>/Saturation3' incorporates:
    //   UnaryMinus: '<S139>/Unary Minus1'

    if (-rtb_error_idx_2 > 272.0) {
      rtb_Product1_d = 272.0;
    } else if (-rtb_error_idx_2 < -272.0) {
      rtb_Product1_d = -272.0;
    } else {
      rtb_Product1_d = -rtb_error_idx_2;
    }

    // Outport: '<Root>/Fx_pos' incorporates:
    //   Saturate: '<S139>/Saturation3'
    //   Sum: '<S2>/Sum2'

    rtY.Fx_pos = rtb_RateTransition + rtb_Product1_d;

    // Saturate: '<S139>/Saturation1' incorporates:
    //   UnaryMinus: '<S139>/Unary Minus'

    if (-rtb_RateTransition2 > 272.0) {
      rtb_Product1_d = 272.0;
    } else if (-rtb_RateTransition2 < -272.0) {
      rtb_Product1_d = -272.0;
    } else {
      rtb_Product1_d = -rtb_RateTransition2;
    }

    // Outport: '<Root>/Fy_neg' incorporates:
    //   Saturate: '<S139>/Saturation1'
    //   Sum: '<S2>/Sum'

    rtY.Fy_neg = rtb_RateTransition + rtb_Product1_d;
    if (tmp) {
      rtDW.RateTransition1_Buffer = rtU.T_alloc_total;
    }

    // Update for UnitDelay: '<S194>/Unit Delay' incorporates:
    //   Inport: '<Root>/T_alloc_total'

    rtDW.UnitDelay_DSTATE_b = rtb_LogicalOperator4_o;

    // Update for DiscreteIntegrator: '<S176>/Integrator'
    rtb_error_idx_0 = 0.005 * rtb_error_idx_0 + rtDW.Integrator_DSTATE_f[0];
    rtDW.Integrator_DSTATE_f[0] = rtb_error_idx_0;
    if (rtb_error_idx_0 > 16.5) {
      rtDW.Integrator_DSTATE_f[0] = 16.5;
    } else if (rtb_error_idx_0 < -16.5) {
      rtDW.Integrator_DSTATE_f[0] = -16.5;
    }

    rtb_error_idx_0 = 0.005 * rtb_error_idx_1 + rtDW.Integrator_DSTATE_f[1];
    rtDW.Integrator_DSTATE_f[1] = rtb_error_idx_0;
    if (rtb_error_idx_0 > 16.5) {
      rtDW.Integrator_DSTATE_f[1] = 16.5;
    } else if (rtb_error_idx_0 < -16.5) {
      rtDW.Integrator_DSTATE_f[1] = -16.5;
    }

    rtb_error_idx_0 = 0.005 * rtb_RateTransition4 + rtDW.Integrator_DSTATE_f[2];
    rtDW.Integrator_DSTATE_f[2] = rtb_error_idx_0;
    if (rtb_error_idx_0 > 16.5) {
      rtDW.Integrator_DSTATE_f[2] = 16.5;
    } else if (rtb_error_idx_0 < -16.5) {
      rtDW.Integrator_DSTATE_f[2] = -16.5;
    }

    rtDW.Integrator_PrevResetState_m = static_cast<int8_T>(rtb_AND_d);

    // End of Update for DiscreteIntegrator: '<S176>/Integrator'
  }

  if (tmp) {
    // Sum: '<S64>/SumI1' incorporates:
    //   Bias: '<S3>/Bias6'
    //   Constant: '<S3>/Constant4'
    //   Product: '<S3>/Product3'
    //   Product: '<S48>/IProd Out'
    //   Sum: '<S63>/SumI3'

    rtb_Product1_d = ((rtDW.RateTransition1_Buffer / rtb_Bias_e /
                       rtb_MathFunction_n - 9.81) - rtb_Switch2_j) + rtb_Sum_d *
      platform_targets.altitude.kI;

    // Switch: '<S43>/Switch' incorporates:
    //   RelationalOperator: '<S43>/u_GTE_up'

    if (rtb_Sum_f < rtb_Diff_j) {
      // Switch: '<S43>/Switch1' incorporates:
      //   Constant: '<S3>/Constant1'
      //   RelationalOperator: '<S43>/u_GT_lo'

      if (rtb_Sum_f > -9.81) {
        rtb_Diff_j = rtb_Sum_f;
      } else {
        rtb_Diff_j = -9.81;
      }

      // End of Switch: '<S43>/Switch1'
    }

    // End of Switch: '<S43>/Switch'

    // Sum: '<S43>/Diff'
    rtb_Diff_j = rtb_Sum_f - rtb_Diff_j;

    // Switch: '<S40>/Switch1' incorporates:
    //   Constant: '<S40>/Clamping_zero'
    //   Constant: '<S40>/Constant'
    //   Constant: '<S40>/Constant2'
    //   RelationalOperator: '<S40>/fix for DT propagation issue'

    if (rtb_Diff_j > 0.0) {
      tmp_0 = 1;
    } else {
      tmp_0 = -1;
    }

    // Switch: '<S40>/Switch2' incorporates:
    //   Constant: '<S40>/Clamping_zero'
    //   Constant: '<S40>/Constant3'
    //   Constant: '<S40>/Constant4'
    //   RelationalOperator: '<S40>/fix for DT propagation issue1'

    if (rtb_Product1_d > 0.0) {
      tmp_1 = 1;
    } else {
      tmp_1 = -1;
    }

    // Switch: '<S40>/Switch' incorporates:
    //   Constant: '<S40>/Clamping_zero'
    //   Constant: '<S40>/Constant1'
    //   Logic: '<S40>/AND3'
    //   RelationalOperator: '<S40>/Equal1'
    //   RelationalOperator: '<S40>/Relational Operator'
    //   Switch: '<S40>/Switch1'
    //   Switch: '<S40>/Switch2'

    if ((rtb_Diff_j != 0.0) && (tmp_0 == tmp_1)) {
      rtb_Bias_e = 0.0;
    } else {
      rtb_Bias_e = rtb_Product1_d;
    }

    // End of Switch: '<S40>/Switch'

    // Sum: '<S125>/SumI1' incorporates:
    //   Bias: '<S4>/Bias6'
    //   Constant: '<S4>/Constant4'
    //   Product: '<S109>/IProd Out'
    //   Product: '<S4>/Product3'
    //   Sum: '<S124>/SumI3'

    rtb_Product1_d = ((rtDW.RateTransition1_Buffer / rtb_Bias / rtb_MathFunction
                       - 9.81) - rtb_Switch2) +
      platform_targets.altitude.kI_hover * 2.0 * rtb_Sum_o;

    // Switch: '<S104>/Switch' incorporates:
    //   RelationalOperator: '<S104>/u_GTE_up'

    if (rtb_Sum < rtb_Diff) {
      // Switch: '<S104>/Switch1' incorporates:
      //   Constant: '<S4>/Constant1'
      //   RelationalOperator: '<S104>/u_GT_lo'

      if (rtb_Sum > -9.81) {
        rtb_Diff = rtb_Sum;
      } else {
        rtb_Diff = -9.81;
      }

      // End of Switch: '<S104>/Switch1'
    }

    // End of Switch: '<S104>/Switch'

    // Sum: '<S104>/Diff'
    rtb_Diff = rtb_Sum - rtb_Diff;

    // MultiPortSwitch generated from: '<S2>/Multiport Switch' incorporates:
    //   Constant: '<S2>/Constant'

    switch (mission_mode) {
     case mission_modes::HOVER:
      // Outport: '<Root>/a_cmd'
      rtY.a_cmd = rtb_Switch2;

      // Outport: '<Root>/a_ff' incorporates:
      //   MultiPortSwitch: '<S2>/Multiport Switch2'

      rtY.a_ff = rtb_Saturation3;

      // Outport: '<Root>/V_target' incorporates:
      //   MultiPortSwitch: '<S2>/Multiport Switch1'

      rtY.V_target = rtb_Saturation1;
      break;

     case mission_modes::DROP:
      // Outport: '<Root>/a_cmd'
      rtY.a_cmd = rtb_Switch2_j;

      // Outport: '<Root>/a_ff' incorporates:
      //   MultiPortSwitch: '<S2>/Multiport Switch2'

      rtY.a_ff = rtb_Sqrt;

      // Outport: '<Root>/V_target' incorporates:
      //   MultiPortSwitch: '<S2>/Multiport Switch1'

      rtY.V_target = rtb_UnaryMinus_l;
      break;

     default:
      // Outport: '<Root>/a_cmd' incorporates:
      //   Constant: '<S2>/Constant1'

      rtY.a_cmd = 0.0;

      // Outport: '<Root>/a_ff' incorporates:
      //   Constant: '<S2>/Constant3'
      //   MultiPortSwitch: '<S2>/Multiport Switch2'

      rtY.a_ff = 0.0;

      // Outport: '<Root>/V_target' incorporates:
      //   Constant: '<S2>/Constant2'
      //   MultiPortSwitch: '<S2>/Multiport Switch1'

      rtY.V_target = 0.0;
      break;
    }

    // Update for DiscreteIntegrator: '<S135>/Discrete-Time Integrator' incorporates:
    //   Constant: '<S135>/Constant'

    rtDW.DiscreteTimeIntegrator_DSTATE += 0.02;
    rtDW.DiscreteTimeIntegrator_PrevRese = static_cast<int8_T>
      (rtb_RateTransition5);

    // Update for UnitDelay: '<S132>/Delay Input1'
    //
    //  Block description for '<S132>/Delay Input1':
    //
    //   Store in Global RAM

    rtDW.DelayInput1_DSTATE = rtb_AND;

    // Update for UnitDelay: '<S136>/Unit Delay'
    rtDW.UnitDelay_DSTATE = rtb_LogicalOperator4;

    // Switch: '<S101>/Switch1' incorporates:
    //   Constant: '<S101>/Clamping_zero'
    //   Constant: '<S101>/Constant'
    //   Constant: '<S101>/Constant2'
    //   RelationalOperator: '<S101>/fix for DT propagation issue'

    if (rtb_Diff > 0.0) {
      tmp_0 = 1;
    } else {
      tmp_0 = -1;
    }

    // Switch: '<S101>/Switch2' incorporates:
    //   Constant: '<S101>/Clamping_zero'
    //   Constant: '<S101>/Constant3'
    //   Constant: '<S101>/Constant4'
    //   RelationalOperator: '<S101>/fix for DT propagation issue1'

    if (rtb_Product1_d > 0.0) {
      tmp_1 = 1;
    } else {
      tmp_1 = -1;
    }

    // Switch: '<S101>/Switch' incorporates:
    //   Constant: '<S101>/Clamping_zero'
    //   Constant: '<S101>/Constant1'
    //   Logic: '<S101>/AND3'
    //   RelationalOperator: '<S101>/Equal1'
    //   RelationalOperator: '<S101>/Relational Operator'
    //   Switch: '<S101>/Switch1'
    //   Switch: '<S101>/Switch2'

    if ((rtb_Diff != 0.0) && (tmp_0 == tmp_1)) {
      rtb_Product1_d = 0.0;
    }

    // Update for DiscreteIntegrator: '<S112>/Integrator' incorporates:
    //   Switch: '<S101>/Switch'

    rtDW.Integrator_DSTATE += 0.02 * rtb_Product1_d;
    if (rtDW.Integrator_DSTATE > 3.0) {
      rtDW.Integrator_DSTATE = 3.0;
    } else if (rtDW.Integrator_DSTATE < -10.0) {
      rtDW.Integrator_DSTATE = -10.0;
    }

    rtDW.Integrator_PrevResetState = static_cast<int8_T>(rtb_NOT);

    // Update for DiscreteIntegrator: '<S73>/Discrete-Time Integrator' incorporates:
    //   Constant: '<S73>/Constant'
    //   DiscreteIntegrator: '<S135>/Discrete-Time Integrator'

    rtDW.DiscreteTimeIntegrator_DSTATE_j += 0.02;
    rtDW.DiscreteTimeIntegrator_PrevRe_j = static_cast<int8_T>
      (rtb_RateTransition5);

    // Update for DiscreteIntegrator: '<S51>/Integrator' incorporates:
    //   DiscreteIntegrator: '<S112>/Integrator'

    rtDW.Integrator_DSTATE_p += 0.02 * rtb_Bias_e;
    if (rtDW.Integrator_DSTATE_p > 3.0) {
      rtDW.Integrator_DSTATE_p = 3.0;
    } else if (rtDW.Integrator_DSTATE_p < -3.0) {
      rtDW.Integrator_DSTATE_p = -3.0;
    }

    rtDW.Integrator_PrevResetState_n = static_cast<int8_T>(rtb_NOT);

    // End of Update for DiscreteIntegrator: '<S51>/Integrator'
  }

  rate_scheduler((&rtM));
}

// Model initialize function
void PlatformController::initialize()
{
  // InitializeConditions for DiscreteIntegrator: '<S135>/Discrete-Time Integrator' 
  rtDW.DiscreteTimeIntegrator_PrevRese = 2;

  // InitializeConditions for DiscreteIntegrator: '<S112>/Integrator'
  rtDW.Integrator_DSTATE = -9.81;

  // InitializeConditions for DiscreteIntegrator: '<S73>/Discrete-Time Integrator' 
  rtDW.DiscreteTimeIntegrator_PrevRe_j = 2;

  // InitializeConditions for DiscreteIntegrator: '<S176>/Integrator'
  rtDW.Integrator_PrevResetState_m = 2;
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
