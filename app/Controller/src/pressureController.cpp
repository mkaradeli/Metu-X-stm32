//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: pressureController.cpp
//
// Code generated for Simulink model 'pressureController'.
//
// Model version                  : 1.19
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Sun Feb 15 20:47:12 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: All passed
//
#include "pressureController.h"
#include <cmath>
#include "rtwtypes.h"

// Model step function
void pressureController::step()
{
  real32_T rtb_Gain2_c;
  real32_T rtb_dP;

  // Outputs for Enabled SubSystem: '<Root>/Subsystem' incorporates:
  //   EnablePort: '<S2>/Enable'

  // RelationalOperator: '<S1>/Compare' incorporates:
  //   Constant: '<Root>/Constant'
  //   Constant: '<S1>/Constant'

  if (controller_mode >= controller_modes::PRESSURE) {
    rtDW.Subsystem_MODE = true;

    // Saturate: '<S4>/Saturation' incorporates:
    //   Inport: '<Root>/position_feedback'

    if (rtU.position_feedback > 1800.0F) {
      rtb_Gain2_c = 1800.0F;
    } else if (rtU.position_feedback < 0.0F) {
      rtb_Gain2_c = 0.0F;
    } else {
      rtb_Gain2_c = rtU.position_feedback;
    }

    // Gain: '<S4>/Gain' incorporates:
    //   Saturate: '<S4>/Saturation'

    rtb_Gain2_c *= 0.00092592591F;

    // Math: '<S4>/Square'
    rtb_Gain2_c *= rtb_Gain2_c;

    // Sum: '<S2>/Sum' incorporates:
    //   Bias: '<S4>/Bias'
    //   Bias: '<S4>/Bias1'
    //   Gain: '<S2>/Gain'
    //   Gain: '<S4>/Gain1'
    //   Gain: '<S4>/Gain2'
    //   Gain: '<S4>/Gain3'
    //   Inport: '<Root>/P_manifold'
    //   Inport: '<Root>/P_nozzle'
    //   Inport: '<Root>/P_nozzle_demand'
    //   MinMax: '<S2>/Min'
    //   Product: '<S4>/Product'
    //   Product: '<S4>/Product1'
    //   Sum: '<S2>/Sum1'
    //   Sum: '<S2>/Sum2'
    //   Sum: '<S2>/Sum3'
    //   Sum: '<S2>/Sum5'
    //   UnaryMinus: '<S4>/Unary Minus'

    rtb_dP = rtU.P_manifold - std::fmin(rtU.P_manifold - 0.07F * rtU.P_manifold,
      rtU.P_nozzle_demand - ((-(52.8534F * rtb_Gain2_c / (51.8534F * rtb_Gain2_c
      + 1.0F) * 0.9197F) + 1.0F) * rtU.P_manifold - (rtU.P_manifold -
      rtU.P_nozzle)));

    // Saturate: '<S3>/Saturation' incorporates:
    //   Inport: '<Root>/P_manifold'

    if (rtU.P_manifold > 5000.0F) {
      rtb_Gain2_c = 5000.0F;
    } else if (rtU.P_manifold < 0.0F) {
      rtb_Gain2_c = 0.0F;
    } else {
      rtb_Gain2_c = rtU.P_manifold;
    }

    // End of Saturate: '<S3>/Saturation'

    // Switch: '<S5>/Switch2' incorporates:
    //   Constant: '<S3>/Constant'
    //   RelationalOperator: '<S5>/LowerRelop1'
    //   RelationalOperator: '<S5>/UpperRelop'
    //   Switch: '<S5>/Switch'

    if (rtb_dP > rtb_Gain2_c) {
      rtb_dP = rtb_Gain2_c;
    } else if (rtb_dP < 0.0F) {
      // Switch: '<S5>/Switch' incorporates:
      //   Constant: '<S3>/Constant'

      rtb_dP = 0.0F;
    }

    // End of Switch: '<S5>/Switch2'

    // Gain: '<S3>/Gain' incorporates:
    //   Bias: '<S3>/Bias'
    //   Product: '<S3>/Product'
    //   UnaryMinus: '<S3>/Unary Minus'

    rtb_Gain2_c = (-(1.0F / rtb_Gain2_c * rtb_dP) + 1.0F) * 1.08731103F;

    // Gain: '<S3>/Gain2' incorporates:
    //   Bias: '<S3>/Bias1'
    //   Constant: '<S3>/Constant1'
    //   Gain: '<S3>/Gain1'
    //   MinMax: '<S3>/Max'
    //   Product: '<S3>/Product1'
    //   Sqrt: '<S3>/Sqrt'

    rtb_Gain2_c = std::sqrt(std::fmax(rtb_Gain2_c / (-51.8534F * rtb_Gain2_c +
      52.8534F), 0.0F)) * 1080.0F;

    // Saturate: '<S2>/Saturation'
    if (rtb_Gain2_c > 1080.0F) {
      // Outport: '<Root>/position_demand'
      rtY.position_demand = 1080.0F;
    } else {
      // Outport: '<Root>/position_demand'
      rtY.position_demand = rtb_Gain2_c;
    }

    // End of Saturate: '<S2>/Saturation'
  } else if (rtDW.Subsystem_MODE) {
    // Disable for Outport: '<Root>/position_demand' incorporates:
    //   Outport: '<S2>/theta_demand'

    rtY.position_demand = 0.0F;
    rtDW.Subsystem_MODE = false;
  }

  // End of RelationalOperator: '<S1>/Compare'
  // End of Outputs for SubSystem: '<Root>/Subsystem'
}

// Model initialize function
void pressureController::initialize()
{
  // (no initialization code required)
}

// Constructor
pressureController::pressureController():
  rtU(),
  rtY(),
  rtDW()
{
  // Currently there is no constructor body generated.
}

// Destructor
// Currently there is no destructor body generated.
pressureController::~pressureController() = default;

//
// File trailer for generated code.
//
// [EOF]
//
