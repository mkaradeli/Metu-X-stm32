//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: platformController.cpp
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
#include <cmath>
#include "rtwtypes.h"
#include "limits"
#include "cmath"

// Exported block parameters
struct_RzX5A87yYhLUhlAxm0ffzC platform_targets{
  0.6,
  0.15,
  1.0,
  3.0,
  5.0,
  3.0,
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
                                          //    '<S6>/Constant'
                                          //    '<S8>/Constant'
                                          //    '<S72>/Constant6'
                                          //    '<S136>/Gain3'
                                          //    '<S137>/Gain4'
                                          //    '<S137>/Gain5'
                                          //    '<S166>/Kb'
                                          //    '<S171>/Integral Gain'
                                          //    '<S179>/Proportional Gain'


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
  real_T rtb_Bias_e;
  real_T rtb_Diff;
  real_T rtb_Diff_j;
  real_T rtb_MathFunction;
  real_T rtb_MathFunction_n;
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
  real_T rtb_Sum_cj;
  real_T rtb_Sum_d;
  real_T rtb_Sum_f;
  real_T rtb_Sum_h_0;
  real_T rtb_Sum_h_idx_0;
  real_T rtb_Sum_h_idx_1;
  real_T rtb_Sum_h_idx_2;
  real_T rtb_Switch2_f;
  real_T rtb_Switch2_k;
  real_T rtb_Switch_he_idx_0;
  real_T rtb_Switch_he_idx_1;
  real_T rtb_Switch_he_idx_2;
  real_T rtb_UnaryMinus_l;
  real_T u0;
  int8_T tmp_2;
  int8_T tmp_3;
  boolean_T rtb_AND;
  boolean_T rtb_NOT;
  boolean_T rtb_RateTransition5;
  boolean_T rtb_Switch_a;
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
    // Gain: '<S136>/Gain3' incorporates:
    //   Inport: '<Root>/quaternion_bias'
    //   Sum: '<S136>/Sum'
    //   UnaryMinus: '<S136>/Unary Minus'

    rtb_Switch_he_idx_2 = -(rtDW.RateTransition6_Buffer[0] -
      rtU.quaternion_bias[0]) * platform_targets.attitude.Kp_att;

    // Switch: '<S138>/Switch2' incorporates:
    //   Constant: '<S136>/Constant'
    //   Constant: '<S136>/Constant1'
    //   RelationalOperator: '<S138>/LowerRelop1'
    //   RelationalOperator: '<S138>/UpperRelop'
    //   Switch: '<S138>/Switch'

    if (rtb_Switch_he_idx_2 > rtP.Constant_Value_j) {
      rtb_Switch_he_idx_2 = rtP.Constant_Value_j;
    } else if (rtb_Switch_he_idx_2 < rtP.Constant1_Value_d) {
      // Switch: '<S138>/Switch' incorporates:
      //   Constant: '<S136>/Constant1'

      rtb_Switch_he_idx_2 = rtP.Constant1_Value_d;
    }

    rtb_Switch_he_idx_0 = rtb_Switch_he_idx_2;

    // Outport: '<Root>/omega_demand'
    rtY.omega_demand[0] = rtb_Switch_he_idx_2;

    // Gain: '<S136>/Gain3' incorporates:
    //   Inport: '<Root>/quaternion_bias'
    //   Sum: '<S136>/Sum'
    //   UnaryMinus: '<S136>/Unary Minus'

    rtb_Switch_he_idx_2 = -(rtDW.RateTransition6_Buffer[1] -
      rtU.quaternion_bias[1]) * platform_targets.attitude.Kp_att;

    // Switch: '<S138>/Switch2' incorporates:
    //   Constant: '<S136>/Constant'
    //   Constant: '<S136>/Constant1'
    //   RelationalOperator: '<S138>/LowerRelop1'
    //   RelationalOperator: '<S138>/UpperRelop'
    //   Switch: '<S138>/Switch'

    if (rtb_Switch_he_idx_2 > rtP.Constant_Value_j) {
      rtb_Switch_he_idx_2 = rtP.Constant_Value_j;
    } else if (rtb_Switch_he_idx_2 < rtP.Constant1_Value_d) {
      // Switch: '<S138>/Switch' incorporates:
      //   Constant: '<S136>/Constant1'

      rtb_Switch_he_idx_2 = rtP.Constant1_Value_d;
    }

    rtb_Switch_he_idx_1 = rtb_Switch_he_idx_2;

    // Outport: '<Root>/omega_demand'
    rtY.omega_demand[1] = rtb_Switch_he_idx_2;

    // Gain: '<S136>/Gain3' incorporates:
    //   Inport: '<Root>/quaternion_bias'
    //   Sum: '<S136>/Sum'
    //   UnaryMinus: '<S136>/Unary Minus'

    rtb_Switch_he_idx_2 = -(rtDW.RateTransition6_Buffer[2] -
      rtU.quaternion_bias[2]) * platform_targets.attitude.Kp_att;

    // Switch: '<S138>/Switch2' incorporates:
    //   Constant: '<S136>/Constant'
    //   Constant: '<S136>/Constant1'
    //   RelationalOperator: '<S138>/LowerRelop1'
    //   RelationalOperator: '<S138>/UpperRelop'
    //   Switch: '<S138>/Switch'

    if (rtb_Switch_he_idx_2 > rtP.Constant_Value_j) {
      rtb_Switch_he_idx_2 = rtP.Constant_Value_j;
    } else if (rtb_Switch_he_idx_2 < rtP.Constant1_Value_d) {
      // Switch: '<S138>/Switch' incorporates:
      //   Constant: '<S136>/Constant1'

      rtb_Switch_he_idx_2 = rtP.Constant1_Value_d;
    }

    // Outport: '<Root>/omega_demand'
    rtY.omega_demand[2] = rtb_Switch_he_idx_2;
  }

  // End of RateTransition: '<S5>/Rate Transition6'
  if (tmp) {
    // DiscreteIntegrator: '<S174>/Integrator' incorporates:
    //   Inport: '<Root>/Dropped'

    if (rtU.Dropped && (rtDW.Integrator_PrevResetState <= 0)) {
      rtDW.Integrator_DSTATE[0] = rtP.PIDController_InitialConditionF;
      rtDW.Integrator_DSTATE[1] = rtP.PIDController_InitialConditionF;
      rtDW.Integrator_DSTATE[2] = rtP.PIDController_InitialConditionF;
    }

    // RateTransition: '<S5>/Rate Transition1'
    if (tmp_0) {
      // RateTransition: '<S5>/Rate Transition1'
      rtDW.RateTransition1[0] = rtDW.RateTransition1_Buffer0[0];
      rtDW.RateTransition1[1] = rtDW.RateTransition1_Buffer0[1];
      rtDW.RateTransition1[2] = rtDW.RateTransition1_Buffer0[2];
    }

    // Gain: '<S166>/Kb'
    rtb_Saturation2 = platform_targets.attitude.Ki_rate /
      platform_targets.attitude.Kp_rate;

    // Sum: '<S137>/Subtract3' incorporates:
    //   Inport: '<Root>/gyro'

    rtb_Sum_h_idx_2 = rtDW.RateTransition1[0] - rtU.gyro[0];

    // Sum: '<S183>/Sum' incorporates:
    //   DiscreteIntegrator: '<S174>/Integrator'
    //   Gain: '<S179>/Proportional Gain'

    rtb_Sum_h_0 = platform_targets.attitude.Kp_rate * rtb_Sum_h_idx_2 +
      rtDW.Integrator_DSTATE[0];

    // Saturate: '<S181>/Saturation'
    if (rtb_Sum_h_0 > rtP.PIDController_UpperSaturationLi) {
      rtb_Saturation_idx_1 = rtP.PIDController_UpperSaturationLi;
    } else if (rtb_Sum_h_0 < rtP.PIDController_LowerSaturationLi) {
      rtb_Saturation_idx_1 = rtP.PIDController_LowerSaturationLi;
    } else {
      rtb_Saturation_idx_1 = rtb_Sum_h_0;
    }

    rtb_Saturation_idx_0 = rtb_Saturation_idx_1;

    // Sum: '<S166>/SumI4' incorporates:
    //   Gain: '<S166>/Kb'
    //   Gain: '<S171>/Integral Gain'
    //   Sum: '<S166>/SumI2'

    rtb_Sum_h_idx_0 = (rtb_Saturation_idx_1 - rtb_Sum_h_0) * rtb_Saturation2 +
      platform_targets.attitude.Ki_rate * rtb_Sum_h_idx_2;

    // Sum: '<S137>/Subtract3' incorporates:
    //   Inport: '<Root>/gyro'

    rtb_Sum_h_idx_2 = rtDW.RateTransition1[1] - rtU.gyro[1];

    // Sum: '<S183>/Sum' incorporates:
    //   DiscreteIntegrator: '<S174>/Integrator'
    //   Gain: '<S179>/Proportional Gain'

    rtb_Sum_h_0 = platform_targets.attitude.Kp_rate * rtb_Sum_h_idx_2 +
      rtDW.Integrator_DSTATE[1];

    // Saturate: '<S181>/Saturation'
    if (rtb_Sum_h_0 > rtP.PIDController_UpperSaturationLi) {
      rtb_Saturation_idx_1 = rtP.PIDController_UpperSaturationLi;
    } else if (rtb_Sum_h_0 < rtP.PIDController_LowerSaturationLi) {
      rtb_Saturation_idx_1 = rtP.PIDController_LowerSaturationLi;
    } else {
      rtb_Saturation_idx_1 = rtb_Sum_h_0;
    }

    // Sum: '<S166>/SumI4' incorporates:
    //   Gain: '<S166>/Kb'
    //   Gain: '<S171>/Integral Gain'
    //   Sum: '<S166>/SumI2'

    rtb_Sum_h_idx_1 = (rtb_Saturation_idx_1 - rtb_Sum_h_0) * rtb_Saturation2 +
      platform_targets.attitude.Ki_rate * rtb_Sum_h_idx_2;

    // Sum: '<S137>/Subtract3' incorporates:
    //   Inport: '<Root>/gyro'

    rtb_Sum_h_idx_2 = rtDW.RateTransition1[2] - rtU.gyro[2];

    // Sum: '<S183>/Sum' incorporates:
    //   DiscreteIntegrator: '<S174>/Integrator'
    //   Gain: '<S179>/Proportional Gain'

    rtb_Sum_h_0 = platform_targets.attitude.Kp_rate * rtb_Sum_h_idx_2 +
      rtDW.Integrator_DSTATE[2];

    // Saturate: '<S181>/Saturation'
    if (rtb_Sum_h_0 > rtP.PIDController_UpperSaturationLi) {
      u0 = rtP.PIDController_UpperSaturationLi;
    } else if (rtb_Sum_h_0 < rtP.PIDController_LowerSaturationLi) {
      u0 = rtP.PIDController_LowerSaturationLi;
    } else {
      u0 = rtb_Sum_h_0;
    }

    // Sum: '<S166>/SumI4' incorporates:
    //   Gain: '<S166>/Kb'
    //   Gain: '<S171>/Integral Gain'
    //   Saturate: '<S181>/Saturation'
    //   Sum: '<S166>/SumI2'

    rtb_Sum_h_idx_2 = (u0 - rtb_Sum_h_0) * rtb_Saturation2 +
      platform_targets.attitude.Ki_rate * rtb_Sum_h_idx_2;

    // Gain: '<S137>/Gain4'
    rtb_Saturation2 = 1.0 / (2.0 * platform_targets.attitude.r) *
      rtb_Saturation_idx_0;

    // Saturate: '<S137>/Saturation'
    if (rtb_Saturation2 > rtP.Saturation_UpperSat) {
      rtb_Saturation_idx_0 = rtP.Saturation_UpperSat;
    } else if (rtb_Saturation2 < rtP.Saturation_LowerSat) {
      rtb_Saturation_idx_0 = rtP.Saturation_LowerSat;
    } else {
      rtb_Saturation_idx_0 = rtb_Saturation2;
    }

    // End of Saturate: '<S137>/Saturation'

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

    rtb_Bias = rtP.Gain_Gain * rtDW.RateTransition4_Buffer + rtP.mass.empty;
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

    // Bias: '<S4>/Bias5' incorporates:
    //   Gain: '<S4>/Gain3'
    //   Math: '<S4>/Square'
    //   Math: '<S4>/Square1'
    //   Sum: '<S4>/Sum1'

    u0 = (rtDW.RateTransition6_Buffer_n[0] * rtDW.RateTransition6_Buffer_n[0] +
          rtDW.RateTransition6_Buffer_n[1] * rtDW.RateTransition6_Buffer_n[1]) *
      rtP.Gain3_Gain + rtP.Bias5_Bias;

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
  }

  // RateTransition: '<S2>/Rate Transition5' incorporates:
  //   Inport: '<Root>/Dropped'
  //   Logic: '<S134>/Logical Operator3'

  if (tmp && tmp_1) {
    rtDW.RateTransition5_Buffer = rtU.Dropped;
  }

  if (tmp_1) {
    rtb_RateTransition5 = rtDW.RateTransition5_Buffer;

    // DiscreteIntegrator: '<S133>/Discrete-Time Integrator' incorporates:
    //   DiscreteIntegrator: '<S71>/Discrete-Time Integrator'
    //   Logic: '<S3>/NOT'
    //   Logic: '<S4>/NOT'

    rtb_NOT = !rtDW.RateTransition5_Buffer;
    if ((rtDW.RateTransition5_Buffer && (rtDW.DiscreteTimeIntegrator_PrevRese <=
          0)) || (rtb_NOT && (rtDW.DiscreteTimeIntegrator_PrevRese == 1))) {
      rtDW.DiscreteTimeIntegrator_DSTATE = rtP.DiscreteTimeIntegrator_IC;
    }

    // Logic: '<S133>/AND' incorporates:
    //   Constant: '<S135>/Constant'
    //   DiscreteIntegrator: '<S133>/Discrete-Time Integrator'
    //   RelationalOperator: '<S135>/Compare'

    rtb_AND = ((rtDW.DiscreteTimeIntegrator_DSTATE <= rtP.Subsystem_t_close) &&
               rtDW.RateTransition5_Buffer);

    // Switch: '<S134>/Switch' incorporates:
    //   Logic: '<S134>/Logical Operator4'
    //   RelationalOperator: '<S130>/FixPt Relational Operator'
    //   UnitDelay: '<S130>/Delay Input1'
    //   UnitDelay: '<S134>/Unit Delay'
    //
    //  Block description for '<S130>/Delay Input1':
    //
    //   Store in Global RAM

    rtb_Switch_a = ((static_cast<int32_T>(rtb_AND) > static_cast<int32_T>
                     (rtDW.DelayInput1_DSTATE)) || rtDW.UnitDelay_DSTATE);

    // Outputs for Enabled SubSystem: '<S72>/Enabled Subsystem' incorporates:
    //   EnablePort: '<S131>/Enable'

    if (!rtb_Switch_a) {
      // SignalConversion generated from: '<S131>/Height'
      rtDW.Height = rtb_RateTransition2;
    }

    // End of Outputs for SubSystem: '<S72>/Enabled Subsystem'

    // Switch: '<S72>/Switch' incorporates:
    //   Constant: '<S72>/Constant6'
    //   Logic: '<S134>/Logical Operator3'

    if (rtb_AND) {
      u0 = platform_targets.hover.h_ref;
    } else {
      u0 = rtDW.Height;
    }

    // Gain: '<S4>/Gain5' incorporates:
    //   Sum: '<S4>/Sum2'
    //   Switch: '<S72>/Switch'

    rtb_Saturation1 = (u0 - rtb_RateTransition2) * platform_targets.hover.Kh;

    // Saturate: '<S4>/Saturation1'
    if (rtb_Saturation1 > platform_targets.hover.v_sat[1]) {
      rtb_Saturation1 = platform_targets.hover.v_sat[1];
    } else if (rtb_Saturation1 < platform_targets.hover.v_sat[0]) {
      rtb_Saturation1 = platform_targets.hover.v_sat[0];
    }

    // End of Saturate: '<S4>/Saturation1'

    // Sum: '<S4>/Sum'
    rtb_Sum_d = rtb_Saturation1 - rtb_RateTransition3;

    // DiscreteIntegrator: '<S110>/Integrator'
    if (rtb_NOT || (rtDW.Integrator_PrevResetState_g != 0)) {
      rtDW.Integrator_DSTATE_g = rtP.PIDController_InitialConditio_j;
    }

    // Gain: '<S4>/Gain1' incorporates:
    //   UnaryMinus: '<S4>/Unary Minus'

    rtb_Saturation3 = platform_targets.hover.Kh * -rtb_RateTransition3;

    // Saturate: '<S4>/Saturation3'
    if (rtb_Saturation3 > platform_targets.a_dec_low) {
      rtb_Saturation3 = platform_targets.a_dec_low;
    } else if (rtb_Saturation3 < rtP.Saturation3_LowerSat) {
      rtb_Saturation3 = rtP.Saturation3_LowerSat;
    }

    // End of Saturate: '<S4>/Saturation3'

    // Sum: '<S120>/Sum' incorporates:
    //   Constant: '<S4>/Constant2'
    //   Constant: '<S4>/Constant3'
    //   DiscreteIntegrator: '<S110>/Integrator'
    //   Product: '<S103>/DProd Out'
    //   Product: '<S115>/PProd Out'
    //   UnaryMinus: '<S104>/Unary Minus'

    rtb_Sum = (rtb_Sum_d * platform_targets.altitude.kP_hover +
               rtDW.Integrator_DSTATE_g) + -rtb_Saturation3 *
      rtP.Constant2_Value_e;

    // Switch: '<S118>/Switch2' incorporates:
    //   Constant: '<S4>/Constant1'
    //   RelationalOperator: '<S118>/LowerRelop1'
    //   RelationalOperator: '<S118>/UpperRelop'
    //   Switch: '<S118>/Switch'

    if (rtb_Sum > rtb_Diff) {
      rtb_Switch2_k = rtb_Diff;
    } else if (rtb_Sum < rtP.Constant1_Value_a) {
      // Switch: '<S118>/Switch' incorporates:
      //   Constant: '<S4>/Constant1'

      rtb_Switch2_k = rtP.Constant1_Value_a;
    } else {
      rtb_Switch2_k = rtb_Sum;
    }

    // End of Switch: '<S118>/Switch2'

    // DiscreteIntegrator: '<S71>/Discrete-Time Integrator'
    if ((rtDW.RateTransition5_Buffer && (rtDW.DiscreteTimeIntegrator_PrevRe_j <=
          0)) || (rtb_NOT && (rtDW.DiscreteTimeIntegrator_PrevRe_j == 1))) {
      rtDW.DiscreteTimeIntegrator_DSTATE_j = rtP.DiscreteTimeIntegrator_IC_m;
    }

    // Bias: '<S3>/Bias' incorporates:
    //   Gain: '<S3>/Gain'

    rtb_Bias_e = rtP.Gain_Gain_h * rtb_RateTransition4 + rtP.mass.empty;

    // Bias: '<S3>/Bias5' incorporates:
    //   Gain: '<S3>/Gain3'
    //   Math: '<S3>/Square'
    //   Math: '<S3>/Square1'
    //   Sum: '<S3>/Sum1'

    u0 = (rtb_RateTransition6_n_idx_0 * rtb_RateTransition6_n_idx_0 +
          rtb_RateTransition6_n_idx_1 * rtb_RateTransition6_n_idx_1) *
      rtP.Gain3_Gain_n + rtP.Bias5_Bias_g;

    // Saturate: '<S3>/Saturation4'
    if (u0 > rtP.Saturation4_UpperSat_f) {
      u0 = rtP.Saturation4_UpperSat_f;
    } else if (u0 < rtP.Saturation4_LowerSat_e) {
      u0 = rtP.Saturation4_LowerSat_e;
    }

    // Math: '<S3>/Math Function' incorporates:
    //   Saturate: '<S3>/Saturation4'
    //
    //  About '<S3>/Math Function':
    //   Operator: reciprocal

    rtb_MathFunction_n = 1.0 / u0;

    // Bias: '<S3>/Bias4' incorporates:
    //   Product: '<S3>/Product2'

    rtb_Diff_j = rtb_RateTransition / rtb_Bias_e / rtb_MathFunction_n +
      rtP.Bias4_Bias_j;

    // Bias: '<S3>/Bias1'
    u0 = rtb_RateTransition2 - platform_targets.h_cut;

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

    rtb_Sqrt = std::sqrt(2.0 * platform_targets.a_dec * u0 +
                         platform_targets.V_td * platform_targets.V_td);

    // Saturate: '<S3>/Saturation2'
    if (rtb_Sqrt > platform_targets.V_max) {
      rtb_Sum_cj = platform_targets.V_max;
    } else if (rtb_Sqrt < rtP.Saturation2_LowerSat) {
      rtb_Sum_cj = rtP.Saturation2_LowerSat;
    } else {
      rtb_Sum_cj = rtb_Sqrt;
    }

    // UnaryMinus: '<S3>/Unary Minus' incorporates:
    //   Saturate: '<S3>/Saturation2'

    rtb_UnaryMinus_l = -rtb_Sum_cj;

    // Sum: '<S3>/Sum' incorporates:
    //   UnaryMinus: '<S3>/Unary Minus'

    rtb_Sum_cj = -rtb_Sum_cj - rtb_RateTransition3;

    // DiscreteIntegrator: '<S49>/Integrator'
    if (rtb_NOT || (rtDW.Integrator_PrevResetState_n != 0)) {
      rtDW.Integrator_DSTATE_p = rtP.PIDController_InitialConditi_jf;
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
      rtb_RateTransition3 = rtP.Constant_Value;
    }

    // End of Switch: '<S3>/Switch'

    // Saturate: '<S3>/Saturation3'
    if (rtb_RateTransition3 > platform_targets.a_dec) {
      rtb_Sqrt = platform_targets.a_dec;
    } else if (rtb_RateTransition3 < rtP.Saturation3_LowerSat_l) {
      rtb_Sqrt = rtP.Saturation3_LowerSat_l;
    } else {
      rtb_Sqrt = rtb_RateTransition3;
    }

    // End of Saturate: '<S3>/Saturation3'

    // Sum: '<S59>/Sum' incorporates:
    //   Constant: '<S3>/Constant2'
    //   Constant: '<S3>/Constant3'
    //   DiscreteIntegrator: '<S49>/Integrator'
    //   Product: '<S42>/DProd Out'
    //   Product: '<S54>/PProd Out'
    //   UnaryMinus: '<S43>/Unary Minus'

    rtb_Sum_f = (rtb_Sum_cj * platform_targets.altitude.kP +
                 rtDW.Integrator_DSTATE_p) + -rtb_Sqrt * rtP.Constant2_Value_c;

    // Switch: '<S57>/Switch2' incorporates:
    //   Constant: '<S3>/Constant1'
    //   RelationalOperator: '<S57>/LowerRelop1'
    //   RelationalOperator: '<S57>/UpperRelop'
    //   Switch: '<S57>/Switch'

    if (rtb_Sum_f > rtb_Diff_j) {
      rtb_Switch2_f = rtb_Diff_j;
    } else if (rtb_Sum_f < rtP.Constant1_Value_ah) {
      // Switch: '<S57>/Switch' incorporates:
      //   Constant: '<S3>/Constant1'

      rtb_Switch2_f = rtP.Constant1_Value_ah;
    } else {
      rtb_Switch2_f = rtb_Sum_f;
    }

    // End of Switch: '<S57>/Switch2'

    // MultiPortSwitch generated from: '<S2>/Multiport Switch' incorporates:
    //   Constant: '<S2>/Constant'
    //   Constant: '<S2>/Constant1'

    switch (mission_mode) {
     case mission_modes::HOVER:
      // Switch: '<S4>/Switch' incorporates:
      //   Constant: '<S129>/Constant'
      //   Constant: '<S4>/Constant6'
      //   DiscreteIntegrator: '<S71>/Discrete-Time Integrator'
      //   Logic: '<S71>/AND'
      //   RelationalOperator: '<S129>/Compare'

      if ((rtDW.DiscreteTimeIntegrator_DSTATE_j <= rtP.Subsystem_t_close_e) &&
          rtDW.RateTransition5_Buffer) {
        // Product: '<S4>/Product1' incorporates:
        //   Bias: '<S4>/Bias3'

        rtb_RateTransition3 = (rtb_Switch2_k + rtP.Bias3_Bias) * rtb_Bias *
          rtb_MathFunction;

        // Switch: '<S70>/Switch2' incorporates:
        //   Constant: '<S4>/Constant5'
        //   RelationalOperator: '<S70>/LowerRelop1'
        //   RelationalOperator: '<S70>/UpperRelop'
        //   Switch: '<S70>/Switch'

        if (rtb_RateTransition3 > rtb_RateTransition) {
          rtb_RateTransition3 = rtb_RateTransition;
        } else if (rtb_RateTransition3 < rtP.Constant5_Value) {
          // Switch: '<S70>/Switch' incorporates:
          //   Constant: '<S4>/Constant5'

          rtb_RateTransition3 = rtP.Constant5_Value;
        }

        // End of Switch: '<S70>/Switch2'
      } else {
        rtb_RateTransition3 = rtP.Constant6_Value;
      }

      // End of Switch: '<S4>/Switch'
      break;

     case mission_modes::DROP:
      // Switch: '<S3>/Switch1' incorporates:
      //   Constant: '<S3>/Constant8'
      //   Constant: '<S8>/Constant'
      //   RelationalOperator: '<S8>/Compare'

      if (rtb_RateTransition2 >= platform_targets.h_cut) {
        // Product: '<S3>/Product1' incorporates:
        //   Bias: '<S3>/Bias3'

        rtb_RateTransition3 = (rtb_Switch2_f + rtP.Bias3_Bias_e) * rtb_Bias_e *
          rtb_MathFunction_n;

        // Switch: '<S11>/Switch2' incorporates:
        //   Constant: '<S3>/Constant5'
        //   RelationalOperator: '<S11>/LowerRelop1'
        //   RelationalOperator: '<S11>/UpperRelop'
        //   Switch: '<S11>/Switch'

        if (rtb_RateTransition3 > rtb_RateTransition) {
          rtb_RateTransition3 = rtb_RateTransition;
        } else if (rtb_RateTransition3 < rtP.Constant5_Value_o) {
          // Switch: '<S11>/Switch' incorporates:
          //   Constant: '<S3>/Constant5'

          rtb_RateTransition3 = rtP.Constant5_Value_o;
        }

        // End of Switch: '<S11>/Switch2'
      } else {
        rtb_RateTransition3 = rtP.Constant8_Value;
      }

      // End of Switch: '<S3>/Switch1'
      break;

     default:
      rtb_RateTransition3 = rtP.Constant1_Value_k;
      break;
    }

    // Gain: '<S2>/Gain'
    rtDW.Gain = rtP.Gain_Gain_a * rtb_RateTransition3;

    // Outport: '<Root>/VerticalThrustCmd' incorporates:
    //   Gain: '<S2>/Gain1'

    rtY.VerticalThrustCmd = rtP.Gain1_Gain * rtb_RateTransition3;

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

      rtY.a_cmd = rtP.Constant1_Value_k;
      break;
    }
  }

  // RateTransition: '<S2>/Rate Transition1' incorporates:
  //   Inport: '<Root>/T_alloc_total'

  if (tmp) {
    // Outport: '<Root>/Fy_pos' incorporates:
    //   Sum: '<S2>/Sum1'

    rtY.Fy_pos = rtDW.Gain + rtb_Saturation_idx_0;

    // Saturate: '<S137>/Saturation1' incorporates:
    //   UnaryMinus: '<S137>/Unary Minus'

    if (-rtb_Saturation2 > rtP.Saturation1_UpperSat_k) {
      u0 = rtP.Saturation1_UpperSat_k;
    } else if (-rtb_Saturation2 < rtP.Saturation1_LowerSat_h) {
      u0 = rtP.Saturation1_LowerSat_h;
    } else {
      u0 = -rtb_Saturation2;
    }

    // Outport: '<Root>/Fy_neg' incorporates:
    //   Saturate: '<S137>/Saturation1'
    //   Sum: '<S2>/Sum'

    rtY.Fy_neg = rtDW.Gain + u0;

    // Gain: '<S137>/Gain5'
    rtb_Saturation_idx_0 = 1.0 / (2.0 * platform_targets.attitude.r) *
      rtb_Saturation_idx_1;

    // Saturate: '<S137>/Saturation2'
    if (rtb_Saturation_idx_0 > rtP.Saturation2_UpperSat) {
      u0 = rtP.Saturation2_UpperSat;
    } else if (rtb_Saturation_idx_0 < rtP.Saturation2_LowerSat_l) {
      u0 = rtP.Saturation2_LowerSat_l;
    } else {
      u0 = rtb_Saturation_idx_0;
    }

    // Outport: '<Root>/Fx_neg' incorporates:
    //   Saturate: '<S137>/Saturation2'
    //   Sum: '<S2>/Sum3'

    rtY.Fx_neg = rtDW.Gain + u0;

    // Saturate: '<S137>/Saturation3' incorporates:
    //   UnaryMinus: '<S137>/Unary Minus1'

    if (-rtb_Saturation_idx_0 > rtP.Saturation3_UpperSat) {
      u0 = rtP.Saturation3_UpperSat;
    } else if (-rtb_Saturation_idx_0 < rtP.Saturation3_LowerSat_j) {
      u0 = rtP.Saturation3_LowerSat_j;
    } else {
      u0 = -rtb_Saturation_idx_0;
    }

    // Outport: '<Root>/Fx_pos' incorporates:
    //   Saturate: '<S137>/Saturation3'
    //   Sum: '<S2>/Sum2'

    rtY.Fx_pos = rtDW.Gain + u0;
    if (tmp_1) {
      rtDW.RateTransition1_Buffer = rtU.T_alloc_total;
    }

    // Update for DiscreteIntegrator: '<S174>/Integrator' incorporates:
    //   Inport: '<Root>/Dropped'
    //   Inport: '<Root>/T_alloc_total'

    rtb_Saturation2 = rtP.Integrator_gainval * rtb_Sum_h_idx_0 +
      rtDW.Integrator_DSTATE[0];
    rtDW.Integrator_DSTATE[0] = rtb_Saturation2;
    if (rtb_Saturation2 > rtP.PIDController_UpperIntegratorSa) {
      rtDW.Integrator_DSTATE[0] = rtP.PIDController_UpperIntegratorSa;
    } else if (rtb_Saturation2 < rtP.PIDController_LowerIntegratorSa) {
      rtDW.Integrator_DSTATE[0] = rtP.PIDController_LowerIntegratorSa;
    }

    rtb_Saturation2 = rtP.Integrator_gainval * rtb_Sum_h_idx_1 +
      rtDW.Integrator_DSTATE[1];
    rtDW.Integrator_DSTATE[1] = rtb_Saturation2;
    if (rtb_Saturation2 > rtP.PIDController_UpperIntegratorSa) {
      rtDW.Integrator_DSTATE[1] = rtP.PIDController_UpperIntegratorSa;
    } else if (rtb_Saturation2 < rtP.PIDController_LowerIntegratorSa) {
      rtDW.Integrator_DSTATE[1] = rtP.PIDController_LowerIntegratorSa;
    }

    rtb_Saturation2 = rtP.Integrator_gainval * rtb_Sum_h_idx_2 +
      rtDW.Integrator_DSTATE[2];
    rtDW.Integrator_DSTATE[2] = rtb_Saturation2;
    if (rtb_Saturation2 > rtP.PIDController_UpperIntegratorSa) {
      rtDW.Integrator_DSTATE[2] = rtP.PIDController_UpperIntegratorSa;
    } else if (rtb_Saturation2 < rtP.PIDController_LowerIntegratorSa) {
      rtDW.Integrator_DSTATE[2] = rtP.PIDController_LowerIntegratorSa;
    }

    rtDW.Integrator_PrevResetState = static_cast<int8_T>(rtU.Dropped);

    // End of Update for DiscreteIntegrator: '<S174>/Integrator'
  }

  if (tmp_1) {
    // Sum: '<S62>/SumI1' incorporates:
    //   Bias: '<S3>/Bias6'
    //   Constant: '<S3>/Constant4'
    //   Gain: '<S61>/Kt'
    //   Product: '<S3>/Product3'
    //   Product: '<S46>/IProd Out'
    //   Sum: '<S61>/SumI3'

    rtb_RateTransition3 = ((rtDW.RateTransition1_Buffer / rtb_Bias_e /
      rtb_MathFunction_n + rtP.Bias6_Bias) - rtb_Switch2_f) *
      rtP.PIDController_Kt + rtb_Sum_cj * platform_targets.altitude.kI;

    // Switch: '<S41>/Switch' incorporates:
    //   RelationalOperator: '<S41>/u_GTE_up'

    if (rtb_Sum_f < rtb_Diff_j) {
      // Switch: '<S41>/Switch1' incorporates:
      //   Constant: '<S3>/Constant1'
      //   RelationalOperator: '<S41>/u_GT_lo'

      if (rtb_Sum_f > rtP.Constant1_Value_ah) {
        rtb_Diff_j = rtb_Sum_f;
      } else {
        rtb_Diff_j = rtP.Constant1_Value_ah;
      }

      // End of Switch: '<S41>/Switch1'
    }

    // End of Switch: '<S41>/Switch'

    // Sum: '<S41>/Diff'
    rtb_Diff_j = rtb_Sum_f - rtb_Diff_j;

    // Switch: '<S38>/Switch1' incorporates:
    //   Constant: '<S38>/Clamping_zero'
    //   Constant: '<S38>/Constant'
    //   Constant: '<S38>/Constant2'
    //   RelationalOperator: '<S38>/fix for DT propagation issue'

    if (rtb_Diff_j > rtP.Clamping_zero_Value) {
      tmp_2 = rtP.Constant_Value_cz;
    } else {
      tmp_2 = rtP.Constant2_Value_o;
    }

    // Switch: '<S38>/Switch2' incorporates:
    //   Constant: '<S38>/Clamping_zero'
    //   Constant: '<S38>/Constant3'
    //   Constant: '<S38>/Constant4'
    //   RelationalOperator: '<S38>/fix for DT propagation issue1'

    if (rtb_RateTransition3 > rtP.Clamping_zero_Value) {
      tmp_3 = rtP.Constant3_Value_p;
    } else {
      tmp_3 = rtP.Constant4_Value;
    }

    // Switch: '<S38>/Switch' incorporates:
    //   Constant: '<S38>/Clamping_zero'
    //   Constant: '<S38>/Constant1'
    //   Logic: '<S38>/AND3'
    //   RelationalOperator: '<S38>/Equal1'
    //   RelationalOperator: '<S38>/Relational Operator'
    //   Switch: '<S38>/Switch1'
    //   Switch: '<S38>/Switch2'

    if ((rtP.Clamping_zero_Value != rtb_Diff_j) && (tmp_2 == tmp_3)) {
      rtb_Saturation2 = rtP.Constant1_Value;
    } else {
      rtb_Saturation2 = rtb_RateTransition3;
    }

    // End of Switch: '<S38>/Switch'

    // Sum: '<S123>/SumI1' incorporates:
    //   Bias: '<S4>/Bias6'
    //   Constant: '<S4>/Constant4'
    //   Gain: '<S122>/Kt'
    //   Product: '<S107>/IProd Out'
    //   Product: '<S4>/Product3'
    //   Sum: '<S122>/SumI3'

    rtb_RateTransition3 = ((rtDW.RateTransition1_Buffer / rtb_Bias /
      rtb_MathFunction + rtP.Bias6_Bias_i) - rtb_Switch2_k) *
      rtP.PIDController_Kt_b + platform_targets.altitude.kI_hover * 2.0 *
      rtb_Sum_d;

    // Switch: '<S102>/Switch' incorporates:
    //   RelationalOperator: '<S102>/u_GTE_up'

    if (rtb_Sum < rtb_Diff) {
      // Switch: '<S102>/Switch1' incorporates:
      //   Constant: '<S4>/Constant1'
      //   RelationalOperator: '<S102>/u_GT_lo'

      if (rtb_Sum > rtP.Constant1_Value_a) {
        rtb_Diff = rtb_Sum;
      } else {
        rtb_Diff = rtP.Constant1_Value_a;
      }

      // End of Switch: '<S102>/Switch1'
    }

    // End of Switch: '<S102>/Switch'

    // Sum: '<S102>/Diff'
    rtb_Diff = rtb_Sum - rtb_Diff;

    // Switch: '<S99>/Switch1' incorporates:
    //   Constant: '<S99>/Clamping_zero'
    //   Constant: '<S99>/Constant'
    //   Constant: '<S99>/Constant2'
    //   RelationalOperator: '<S99>/fix for DT propagation issue'

    if (rtb_Diff > rtP.Clamping_zero_Value_j) {
      tmp_2 = rtP.Constant_Value_d;
    } else {
      tmp_2 = rtP.Constant2_Value_a;
    }

    // Switch: '<S99>/Switch2' incorporates:
    //   Constant: '<S99>/Clamping_zero'
    //   Constant: '<S99>/Constant3'
    //   Constant: '<S99>/Constant4'
    //   RelationalOperator: '<S99>/fix for DT propagation issue1'

    if (rtb_RateTransition3 > rtP.Clamping_zero_Value_j) {
      tmp_3 = rtP.Constant3_Value_j;
    } else {
      tmp_3 = rtP.Constant4_Value_l;
    }

    // Switch: '<S99>/Switch' incorporates:
    //   Constant: '<S99>/Clamping_zero'
    //   Constant: '<S99>/Constant1'
    //   Logic: '<S99>/AND3'
    //   RelationalOperator: '<S99>/Equal1'
    //   RelationalOperator: '<S99>/Relational Operator'
    //   Switch: '<S99>/Switch1'
    //   Switch: '<S99>/Switch2'

    if ((rtP.Clamping_zero_Value_j != rtb_Diff) && (tmp_2 == tmp_3)) {
      rtb_RateTransition3 = rtP.Constant1_Value_h;
    }

    // End of Switch: '<S99>/Switch'

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

      rtY.a_ff = rtP.Constant3_Value;

      // Outport: '<Root>/V_target' incorporates:
      //   Constant: '<S2>/Constant2'
      //   MultiPortSwitch: '<S2>/Multiport Switch1'

      rtY.V_target = rtP.Constant2_Value;
      break;
    }

    // End of MultiPortSwitch: '<S2>/Multiport Switch2'

    // Update for DiscreteIntegrator: '<S133>/Discrete-Time Integrator' incorporates:
    //   Constant: '<S133>/Constant'

    rtDW.DiscreteTimeIntegrator_DSTATE += rtP.DiscreteTimeIntegrator_gainval *
      rtP.Constant_Value_l5;
    rtDW.DiscreteTimeIntegrator_PrevRese = static_cast<int8_T>
      (rtb_RateTransition5);

    // Update for UnitDelay: '<S130>/Delay Input1'
    //
    //  Block description for '<S130>/Delay Input1':
    //
    //   Store in Global RAM

    rtDW.DelayInput1_DSTATE = rtb_AND;

    // Update for UnitDelay: '<S134>/Unit Delay'
    rtDW.UnitDelay_DSTATE = rtb_Switch_a;

    // Update for DiscreteIntegrator: '<S110>/Integrator'
    rtDW.Integrator_DSTATE_g += rtP.Integrator_gainval_l * rtb_RateTransition3;
    if (rtDW.Integrator_DSTATE_g > rtP.PIDController_UpperIntegrator_c) {
      rtDW.Integrator_DSTATE_g = rtP.PIDController_UpperIntegrator_c;
    } else if (rtDW.Integrator_DSTATE_g < rtP.PIDController_LowerIntegrator_l) {
      rtDW.Integrator_DSTATE_g = rtP.PIDController_LowerIntegrator_l;
    }

    rtDW.Integrator_PrevResetState_g = static_cast<int8_T>(rtb_NOT);

    // End of Update for DiscreteIntegrator: '<S110>/Integrator'

    // Update for DiscreteIntegrator: '<S71>/Discrete-Time Integrator' incorporates:
    //   Constant: '<S71>/Constant'
    //   DiscreteIntegrator: '<S133>/Discrete-Time Integrator'

    rtDW.DiscreteTimeIntegrator_DSTATE_j += rtP.DiscreteTimeIntegrator_gainva_o *
      rtP.Constant_Value_l;
    rtDW.DiscreteTimeIntegrator_PrevRe_j = static_cast<int8_T>
      (rtb_RateTransition5);

    // Update for DiscreteIntegrator: '<S49>/Integrator'
    rtDW.Integrator_DSTATE_p += rtP.Integrator_gainval_n * rtb_Saturation2;
    if (rtDW.Integrator_DSTATE_p > rtP.PIDController_UpperIntegrator_m) {
      rtDW.Integrator_DSTATE_p = rtP.PIDController_UpperIntegrator_m;
    } else if (rtDW.Integrator_DSTATE_p < rtP.PIDController_LowerIntegrator_n) {
      rtDW.Integrator_DSTATE_p = rtP.PIDController_LowerIntegrator_n;
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
  // Start for RateTransition: '<S5>/Rate Transition1'
  rtDW.RateTransition1[0] = rtP.RateTransition1_InitialConditio;
  rtDW.RateTransition1[1] = rtP.RateTransition1_InitialConditio;
  rtDW.RateTransition1[2] = rtP.RateTransition1_InitialConditio;

  // InitializeConditions for DiscreteIntegrator: '<S174>/Integrator'
  rtDW.Integrator_PrevResetState = 2;
  rtDW.Integrator_DSTATE[0] = rtP.PIDController_InitialConditionF;

  // InitializeConditions for RateTransition: '<S5>/Rate Transition1'
  rtDW.RateTransition1_Buffer0[0] = rtP.RateTransition1_InitialConditio;

  // InitializeConditions for DiscreteIntegrator: '<S174>/Integrator'
  rtDW.Integrator_DSTATE[1] = rtP.PIDController_InitialConditionF;

  // InitializeConditions for RateTransition: '<S5>/Rate Transition1'
  rtDW.RateTransition1_Buffer0[1] = rtP.RateTransition1_InitialConditio;

  // InitializeConditions for DiscreteIntegrator: '<S174>/Integrator'
  rtDW.Integrator_DSTATE[2] = rtP.PIDController_InitialConditionF;

  // InitializeConditions for RateTransition: '<S5>/Rate Transition1'
  rtDW.RateTransition1_Buffer0[2] = rtP.RateTransition1_InitialConditio;

  // InitializeConditions for DiscreteIntegrator: '<S133>/Discrete-Time Integrator' 
  rtDW.DiscreteTimeIntegrator_DSTATE = rtP.DiscreteTimeIntegrator_IC;
  rtDW.DiscreteTimeIntegrator_PrevRese = 2;

  // InitializeConditions for UnitDelay: '<S130>/Delay Input1'
  //
  //  Block description for '<S130>/Delay Input1':
  //
  //   Store in Global RAM

  rtDW.DelayInput1_DSTATE = rtP.DetectIncrease_vinit;

  // InitializeConditions for UnitDelay: '<S134>/Unit Delay'
  rtDW.UnitDelay_DSTATE = rtP.SetResetFlipFlop_Q0;

  // InitializeConditions for DiscreteIntegrator: '<S110>/Integrator'
  rtDW.Integrator_DSTATE_g = rtP.PIDController_InitialConditio_j;

  // InitializeConditions for DiscreteIntegrator: '<S71>/Discrete-Time Integrator' 
  rtDW.DiscreteTimeIntegrator_DSTATE_j = rtP.DiscreteTimeIntegrator_IC_m;
  rtDW.DiscreteTimeIntegrator_PrevRe_j = 2;

  // InitializeConditions for DiscreteIntegrator: '<S49>/Integrator'
  rtDW.Integrator_DSTATE_p = rtP.PIDController_InitialConditi_jf;

  // SystemInitialize for Enabled SubSystem: '<S72>/Enabled Subsystem'
  // SystemInitialize for SignalConversion generated from: '<S131>/Height' incorporates:
  //   Outport: '<S131>/Out1'

  rtDW.Height = rtP.Out1_Y0;

  // End of SystemInitialize for SubSystem: '<S72>/Enabled Subsystem'
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

extern "C"
{
  // Test if value is infinite
  static boolean_T rtIsInf(real_T value)
  {
    return std::isinf(value);
  }

  // Test if single-precision value is infinite
  static boolean_T rtIsInfF(real32_T value)
  {
    return std::isinf(value);
  }

  // Test if value is not a number
  static boolean_T rtIsNaN(real_T value)
  {
    return std::isnan(value);
  }

  // Test if single-precision value is not a number
  static boolean_T rtIsNaNF(real32_T value)
  {
    return std::isnan(value);
  }
}

//
// File trailer for generated code.
//
// [EOF]
//
