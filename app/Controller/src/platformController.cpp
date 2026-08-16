//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: platformController.cpp
//
// Code generated for Simulink model 'platformController'.
//
// Model version                  : 1.31
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Sat Aug 15 19:01:51 2026
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
mission_modes mission_mode{ mission_modes::DISABLE };// Variable: mission_mode
                                                        //  Referenced by: '<S1>/Constant'


// Model step function
void platformController::step()
{
  real_T rtb_Diff_n;
  real_T rtb_MathFunction;
  real_T rtb_MathFunction_b;
  real_T rtb_Sqrt;
  real_T rtb_SumI1;
  real_T rtb_Sum_bx;
  real_T rtb_Sum_e;
  real_T rtb_Sum_o;
  real_T rtb_Switch2;
  real_T rtb_Switch2_n;
  real_T rtb_UnaryMinus;
  int8_T tmp;
  int8_T tmp_0;
  boolean_T rtb_NOT;

  // Sqrt: '<S2>/Sqrt' incorporates:
  //   Bias: '<S2>/Bias1'
  //   Bias: '<S2>/Bias2'
  //   Gain: '<S2>/Gain2'
  //   Inport: '<Root>/Height'
  //   Saturate: '<S2>/Saturation1'

  rtb_Sqrt = std::sqrt(std::fmax(rtU.Height - 0.2, 0.0) * 30.0 + 0.25);

  // Switch: '<S2>/Switch' incorporates:
  //   Abs: '<S2>/Abs1'
  //   Constant: '<S2>/Constant'
  //   Constant: '<S5>/Constant'
  //   Gain: '<S2>/Gain1'
  //   Inport: '<Root>/Velocity'
  //   Product: '<S2>/Divide'
  //   RelationalOperator: '<S5>/Compare'

  if (rtb_Sqrt < 12.0) {
    rtb_Sum_bx = std::abs(rtU.Velocity) / rtb_Sqrt * 15.0;
  } else {
    rtb_Sum_bx = -9.8;
  }

  // End of Switch: '<S2>/Switch'

  // UnaryMinus: '<S42>/Unary Minus' incorporates:
  //   Saturate: '<S2>/Saturation3'

  rtb_UnaryMinus = -std::fmin(rtb_Sum_bx, 15.0);

  // Sum: '<S2>/Sum' incorporates:
  //   Inport: '<Root>/Velocity'
  //   Saturate: '<S2>/Saturation2'
  //   UnaryMinus: '<S2>/Unary Minus'

  rtb_Sum_bx = -std::fmin(rtb_Sqrt, 12.0) - rtU.Velocity;

  // Logic: '<S2>/NOT' incorporates:
  //   Inport: '<Root>/Dropped'

  rtb_NOT = !rtU.Dropped;

  // DiscreteIntegrator: '<S48>/Integrator'
  if (rtb_NOT && (rtDW.Integrator_PrevResetState <= 0)) {
    rtDW.Integrator_DSTATE = 0.0;
  }

  // Sum: '<S58>/Sum' incorporates:
  //   Constant: '<S2>/Constant3'
  //   DiscreteIntegrator: '<S48>/Integrator'
  //   Product: '<S41>/DProd Out'
  //   Product: '<S53>/PProd Out'

  rtb_Sqrt = (rtb_Sum_bx * 9.8 + rtDW.Integrator_DSTATE) - rtb_UnaryMinus;

  // Bias: '<S2>/Bias' incorporates:
  //   Gain: '<S2>/Gain'
  //   Inport: '<Root>/ManifoldPressure'

  rtb_UnaryMinus = 0.0095 * rtU.ManifoldPressure + 34.0;

  // Bias: '<S2>/Bias5' incorporates:
  //   Gain: '<S2>/Gain3'
  //   Inport: '<Root>/quaternion'
  //   Math: '<S2>/Square'
  //   Math: '<S2>/Square1'
  //   Sum: '<S2>/Sum1'

  rtb_MathFunction_b = (rtU.quaternion[0] * rtU.quaternion[0] + rtU.quaternion[1]
                        * rtU.quaternion[1]) * -2.0 + 1.0;

  // Saturate: '<S2>/Saturation4'
  if (rtb_MathFunction_b < 0.7) {
    // Saturate: '<S3>/Saturation4'
    rtb_MathFunction_b = 0.7;
  }

  // Math: '<S2>/Math Function' incorporates:
  //   Saturate: '<S2>/Saturation4'
  //
  //  About '<S2>/Math Function':
  //   Operator: reciprocal

  rtb_MathFunction = 1.0 / rtb_MathFunction_b;

  // Bias: '<S2>/Bias4' incorporates:
  //   Inport: '<Root>/T_max_allowed'
  //   Product: '<S2>/Product2'

  rtb_MathFunction_b = rtU.T_max_allowed / rtb_UnaryMinus / rtb_MathFunction -
    9.81;

  // Switch: '<S56>/Switch2' incorporates:
  //   Constant: '<S2>/Constant1'
  //   RelationalOperator: '<S56>/LowerRelop1'
  //   RelationalOperator: '<S56>/UpperRelop'
  //   Switch: '<S56>/Switch'

  if (rtb_Sqrt > rtb_MathFunction_b) {
    rtb_Switch2 = rtb_MathFunction_b;
  } else if (rtb_Sqrt < -9.81) {
    // Switch: '<S56>/Switch' incorporates:
    //   Constant: '<S2>/Constant1'

    rtb_Switch2 = -9.81;
  } else {
    rtb_Switch2 = rtb_Sqrt;
  }

  // End of Switch: '<S56>/Switch2'

  // Bias: '<S3>/Bias4'
  rtb_Diff_n = rtb_MathFunction_b;

  // Gain: '<S3>/Gain5' incorporates:
  //   Bias: '<S3>/Bias1'
  //   Inport: '<Root>/Height'
  //   UnaryMinus: '<S3>/Unary Minus1'

  rtb_SumI1 = (-rtU.Height + 5.0) * 2.5;

  // Saturate: '<S3>/Saturation1'
  if (rtb_SumI1 > 2.2) {
    rtb_SumI1 = 2.2;
  } else if (rtb_SumI1 < -1.0) {
    rtb_SumI1 = -1.0;
  }

  // Sum: '<S3>/Sum' incorporates:
  //   Inport: '<Root>/Velocity'
  //   Saturate: '<S3>/Saturation1'

  rtb_Sum_e = rtb_SumI1 - rtU.Velocity;

  // DiscreteIntegrator: '<S107>/Integrator' incorporates:
  //   Logic: '<S3>/NOT'

  if (rtb_NOT && (rtDW.Integrator_PrevResetState_b <= 0)) {
    rtDW.Integrator_DSTATE_a = -9.81;
  }

  // Gain: '<S3>/Gain1' incorporates:
  //   Inport: '<Root>/Velocity'
  //   UnaryMinus: '<S3>/Unary Minus'

  rtb_SumI1 = 2.5 * -rtU.Velocity;

  // Saturate: '<S3>/Saturation3'
  if (rtb_SumI1 > 5.0) {
    rtb_SumI1 = 5.0;
  } else if (rtb_SumI1 < -9.81) {
    rtb_SumI1 = -9.81;
  }

  // Sum: '<S117>/Sum' incorporates:
  //   Constant: '<S3>/Constant3'
  //   DiscreteIntegrator: '<S107>/Integrator'
  //   Product: '<S112>/PProd Out'
  //   Saturate: '<S3>/Saturation3'

  rtb_Sum_o = (rtb_Sum_e * 9.8 + rtDW.Integrator_DSTATE_a) + rtb_SumI1;

  // Switch: '<S115>/Switch2' incorporates:
  //   Bias: '<S3>/Bias4'
  //   Constant: '<S3>/Constant1'
  //   RelationalOperator: '<S115>/LowerRelop1'
  //   RelationalOperator: '<S115>/UpperRelop'
  //   Switch: '<S115>/Switch'

  if (rtb_Sum_o > rtb_MathFunction_b) {
    rtb_Switch2_n = rtb_MathFunction_b;
  } else if (rtb_Sum_o < -9.81) {
    // Switch: '<S115>/Switch' incorporates:
    //   Constant: '<S3>/Constant1'

    rtb_Switch2_n = -9.81;
  } else {
    rtb_Switch2_n = rtb_Sum_o;
  }

  // End of Switch: '<S115>/Switch2'

  // MultiPortSwitch generated from: '<S1>/Multiport Switch' incorporates:
  //   Constant: '<S1>/Constant'
  //   Constant: '<S1>/Constant1'

  switch (mission_mode) {
   case mission_modes::HOVER:
    // Product: '<S3>/Product1' incorporates:
    //   Bias: '<S3>/Bias'
    //   Bias: '<S3>/Bias3'
    //   Math: '<S3>/Math Function'
    //
    //  About '<S3>/Math Function':
    //   Operator: reciprocal

    rtb_SumI1 = (rtb_Switch2_n + 9.81) * rtb_UnaryMinus * rtb_MathFunction;

    // Switch: '<S69>/Switch2' incorporates:
    //   Constant: '<S3>/Constant5'
    //   Inport: '<Root>/T_max_allowed'
    //   RelationalOperator: '<S69>/LowerRelop1'
    //   RelationalOperator: '<S69>/UpperRelop'
    //   Switch: '<S69>/Switch'

    if (rtb_SumI1 > rtU.T_max_allowed) {
      rtb_SumI1 = rtU.T_max_allowed;
    } else if (rtb_SumI1 < 0.0) {
      // Switch: '<S69>/Switch' incorporates:
      //   Constant: '<S3>/Constant5'

      rtb_SumI1 = 0.0;
    }

    // End of Switch: '<S69>/Switch2'
    break;

   case mission_modes::DROP:
    // Switch: '<S2>/Switch1' incorporates:
    //   Constant: '<S2>/Constant8'
    //   Constant: '<S7>/Constant'
    //   Inport: '<Root>/Height'
    //   RelationalOperator: '<S7>/Compare'

    if (rtU.Height <= 0.25) {
      rtb_SumI1 = 0.0;
    } else {
      // Product: '<S2>/Product1' incorporates:
      //   Bias: '<S2>/Bias3'

      rtb_SumI1 = (rtb_Switch2 + 9.81) * rtb_UnaryMinus * rtb_MathFunction;

      // Switch: '<S10>/Switch2' incorporates:
      //   Constant: '<S2>/Constant5'
      //   Inport: '<Root>/T_max_allowed'
      //   RelationalOperator: '<S10>/LowerRelop1'
      //   RelationalOperator: '<S10>/UpperRelop'
      //   Switch: '<S10>/Switch'

      if (rtb_SumI1 > rtU.T_max_allowed) {
        rtb_SumI1 = rtU.T_max_allowed;
      } else if (rtb_SumI1 < 0.0) {
        // Switch: '<S10>/Switch' incorporates:
        //   Constant: '<S2>/Constant5'

        rtb_SumI1 = 0.0;
      }

      // End of Switch: '<S10>/Switch2'
    }

    // End of Switch: '<S2>/Switch1'
    break;

   default:
    rtb_SumI1 = 0.0;
    break;
  }

  // End of MultiPortSwitch generated from: '<S1>/Multiport Switch'

  // Gain: '<S1>/Gain'
  rtb_SumI1 *= 0.25;

  // Outport: '<Root>/FrontThrustCmd'
  rtY.FrontThrustCmd = rtb_SumI1;

  // Outport: '<Root>/BackThrustCmd'
  rtY.BackThrustCmd = rtb_SumI1;

  // Outport: '<Root>/LeftThrustCmd'
  rtY.LeftThrustCmd = rtb_SumI1;

  // Outport: '<Root>/RightThrustCmd'
  rtY.RightThrustCmd = rtb_SumI1;

  // Bias: '<S3>/Bias6' incorporates:
  //   Bias: '<S2>/Bias6'
  //   Bias: '<S3>/Bias'
  //   Inport: '<Root>/T_alloc_total'
  //   Math: '<S3>/Math Function'
  //   Product: '<S3>/Product3'
  //
  //  About '<S3>/Math Function':
  //   Operator: reciprocal

  rtb_UnaryMinus = rtU.T_alloc_total / rtb_UnaryMinus / rtb_MathFunction - 9.81;

  // Sum: '<S120>/SumI1' incorporates:
  //   Bias: '<S3>/Bias6'
  //   Constant: '<S3>/Constant4'
  //   Product: '<S104>/IProd Out'
  //   Sum: '<S119>/SumI3'

  rtb_SumI1 = (rtb_UnaryMinus - rtb_Switch2_n) + rtb_Sum_e * 24.010000000000005;

  // Switch: '<S99>/Switch' incorporates:
  //   Bias: '<S3>/Bias4'
  //   RelationalOperator: '<S99>/u_GTE_up'

  if (rtb_Sum_o < rtb_MathFunction_b) {
    // Switch: '<S99>/Switch1' incorporates:
    //   Constant: '<S3>/Constant1'
    //   RelationalOperator: '<S99>/u_GT_lo'

    if (rtb_Sum_o > -9.81) {
      rtb_Diff_n = rtb_Sum_o;
    } else {
      rtb_Diff_n = -9.81;
    }

    // End of Switch: '<S99>/Switch1'
  }

  // End of Switch: '<S99>/Switch'

  // Sum: '<S99>/Diff'
  rtb_Diff_n = rtb_Sum_o - rtb_Diff_n;

  // Switch: '<S96>/Switch1' incorporates:
  //   Constant: '<S96>/Clamping_zero'
  //   Constant: '<S96>/Constant'
  //   Constant: '<S96>/Constant2'
  //   RelationalOperator: '<S96>/fix for DT propagation issue'

  if (rtb_Diff_n > 0.0) {
    tmp = 1;
  } else {
    tmp = -1;
  }

  // Switch: '<S96>/Switch2' incorporates:
  //   Constant: '<S96>/Clamping_zero'
  //   Constant: '<S96>/Constant3'
  //   Constant: '<S96>/Constant4'
  //   RelationalOperator: '<S96>/fix for DT propagation issue1'

  if (rtb_SumI1 > 0.0) {
    tmp_0 = 1;
  } else {
    tmp_0 = -1;
  }

  // Switch: '<S96>/Switch' incorporates:
  //   Constant: '<S96>/Clamping_zero'
  //   Constant: '<S96>/Constant1'
  //   Logic: '<S96>/AND3'
  //   RelationalOperator: '<S96>/Equal1'
  //   RelationalOperator: '<S96>/Relational Operator'
  //   Switch: '<S96>/Switch1'
  //   Switch: '<S96>/Switch2'

  if ((rtb_Diff_n != 0.0) && (tmp == tmp_0)) {
    rtb_Diff_n = 0.0;
  } else {
    rtb_Diff_n = rtb_SumI1;
  }

  // End of Switch: '<S96>/Switch'

  // Sum: '<S61>/SumI1' incorporates:
  //   Constant: '<S2>/Constant4'
  //   Product: '<S45>/IProd Out'
  //   Sum: '<S60>/SumI3'

  rtb_SumI1 = (rtb_UnaryMinus - rtb_Switch2) + rtb_Sum_bx * 12.005000000000003;

  // Switch: '<S40>/Switch' incorporates:
  //   RelationalOperator: '<S40>/u_GTE_up'

  if (rtb_Sqrt < rtb_MathFunction_b) {
    // Switch: '<S40>/Switch1' incorporates:
    //   Constant: '<S2>/Constant1'
    //   RelationalOperator: '<S40>/u_GT_lo'

    if (rtb_Sqrt > -9.81) {
      rtb_MathFunction_b = rtb_Sqrt;
    } else {
      rtb_MathFunction_b = -9.81;
    }

    // End of Switch: '<S40>/Switch1'
  }

  // End of Switch: '<S40>/Switch'

  // Sum: '<S40>/Diff'
  rtb_MathFunction_b = rtb_Sqrt - rtb_MathFunction_b;

  // Switch: '<S37>/Switch1' incorporates:
  //   Constant: '<S37>/Clamping_zero'
  //   Constant: '<S37>/Constant'
  //   Constant: '<S37>/Constant2'
  //   RelationalOperator: '<S37>/fix for DT propagation issue'

  if (rtb_MathFunction_b > 0.0) {
    tmp = 1;
  } else {
    tmp = -1;
  }

  // Switch: '<S37>/Switch2' incorporates:
  //   Constant: '<S37>/Clamping_zero'
  //   Constant: '<S37>/Constant3'
  //   Constant: '<S37>/Constant4'
  //   RelationalOperator: '<S37>/fix for DT propagation issue1'

  if (rtb_SumI1 > 0.0) {
    tmp_0 = 1;
  } else {
    tmp_0 = -1;
  }

  // Switch: '<S37>/Switch' incorporates:
  //   Constant: '<S37>/Clamping_zero'
  //   Constant: '<S37>/Constant1'
  //   Logic: '<S37>/AND3'
  //   RelationalOperator: '<S37>/Equal1'
  //   RelationalOperator: '<S37>/Relational Operator'
  //   Switch: '<S37>/Switch1'
  //   Switch: '<S37>/Switch2'

  if ((rtb_MathFunction_b != 0.0) && (tmp == tmp_0)) {
    rtb_SumI1 = 0.0;
  }

  // Update for DiscreteIntegrator: '<S48>/Integrator' incorporates:
  //   Switch: '<S37>/Switch'

  rtDW.Integrator_DSTATE += 0.02 * rtb_SumI1;
  if (rtDW.Integrator_DSTATE > 3.0) {
    rtDW.Integrator_DSTATE = 3.0;
  } else if (rtDW.Integrator_DSTATE < -3.0) {
    rtDW.Integrator_DSTATE = -3.0;
  }

  rtDW.Integrator_PrevResetState = static_cast<int8_T>(rtb_NOT);

  // Update for DiscreteIntegrator: '<S107>/Integrator' incorporates:
  //   DiscreteIntegrator: '<S48>/Integrator'

  rtDW.Integrator_DSTATE_a += 0.02 * rtb_Diff_n;
  if (rtDW.Integrator_DSTATE_a > 3.0) {
    rtDW.Integrator_DSTATE_a = 3.0;
  } else if (rtDW.Integrator_DSTATE_a < -10.0) {
    rtDW.Integrator_DSTATE_a = -10.0;
  }

  rtDW.Integrator_PrevResetState_b = static_cast<int8_T>(rtb_NOT);

  // End of Update for DiscreteIntegrator: '<S107>/Integrator'
}

// Model initialize function
void platformController::initialize()
{
  // InitializeConditions for DiscreteIntegrator: '<S48>/Integrator'
  rtDW.Integrator_PrevResetState = 2;

  // InitializeConditions for DiscreteIntegrator: '<S107>/Integrator'
  rtDW.Integrator_DSTATE_a = -9.81;
  rtDW.Integrator_PrevResetState_b = 2;
}

const char_T* platformController::RT_MODEL::getErrorStatus() const
{
  return (errorStatus);
}

void platformController::RT_MODEL::setErrorStatus(const char_T* const volatile
  aErrorStatus)
{
  (errorStatus = aErrorStatus);
}

// Constructor
platformController::platformController() :
  rtU(),
  rtY(),
  rtDW(),
  rtM()
{
  // Currently there is no constructor body generated.
}

// Destructor
// Currently there is no destructor body generated.
platformController::~platformController() = default;

// Real-Time Model get method
platformController::RT_MODEL * platformController::getRTM()
{
  return (&rtM);
}

//
// File trailer for generated code.
//
// [EOF]
//
