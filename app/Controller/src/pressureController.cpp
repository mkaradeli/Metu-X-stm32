//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: pressureController.cpp
//
// Code generated for Simulink model 'pressureController'.
//
// Model version                  : 1.23
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Thu Feb 26 20:06:22 2026
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

namespace controller
{
  // Model step function
  void pressure::step()
  {
    real32_T rtb_Min;
    real32_T rtb_Saturation;

    // Outputs for Enabled SubSystem: '<Root>/Subsystem' incorporates:
    //   EnablePort: '<S2>/Enable'

    // RelationalOperator: '<S1>/Compare' incorporates:
    //   Constant: '<Root>/Constant'
    //   Constant: '<S1>/Constant'

    if (controller_mode >= controller_modes::PRESSURE) {
      rtDW.Subsystem_MODE = true;

      // MinMax: '<S2>/Min' incorporates:
      //   Gain: '<S2>/Gain'
      //   Inport: '<Root>/P_manifold'
      //   Inport: '<Root>/P_nozzle_demand'
      //   Sum: '<S2>/Sum1'

      rtb_Min = std::fmin(rtU.P_manifold - 0.07F * rtU.P_manifold,
                          rtU.P_nozzle_demand);

      // Saturate: '<S3>/Saturation' incorporates:
      //   Inport: '<Root>/P_manifold'

      if (rtU.P_manifold > 5000.0F) {
        rtb_Saturation = 5000.0F;
      } else if (rtU.P_manifold < 0.0F) {
        rtb_Saturation = 0.0F;
      } else {
        rtb_Saturation = rtU.P_manifold;
      }

      // End of Saturate: '<S3>/Saturation'

      // Switch: '<S4>/Switch2' incorporates:
      //   Constant: '<S3>/Constant'
      //   RelationalOperator: '<S4>/LowerRelop1'
      //   RelationalOperator: '<S4>/UpperRelop'
      //   Switch: '<S4>/Switch'

      if (rtb_Min > rtb_Saturation) {
        rtb_Min = rtb_Saturation;
      } else if (rtb_Min < 0.0F) {
        // Switch: '<S4>/Switch' incorporates:
        //   Constant: '<S3>/Constant'

        rtb_Min = 0.0F;
      }

      // End of Switch: '<S4>/Switch2'

      // Gain: '<S3>/Gain' incorporates:
      //   Bias: '<S3>/Bias'
      //   Product: '<S3>/Product'
      //   UnaryMinus: '<S3>/Unary Minus'

      rtb_Min = (-(1.0F / rtb_Saturation * rtb_Min) + 1.0F) * 1.08731103F;

      // Sum: '<S2>/Sum' incorporates:
      //   Bias: '<S3>/Bias1'
      //   Constant: '<S3>/Constant1'
      //   Gain: '<S2>/Gain1'
      //   Gain: '<S3>/Gain1'
      //   Gain: '<S3>/Gain2'
      //   Inport: '<Root>/P_nozzle'
      //   Inport: '<Root>/P_nozzle_demand'
      //   MinMax: '<S3>/Max'
      //   Product: '<S3>/Product1'
      //   Sqrt: '<S3>/Sqrt'
      //   Sum: '<S2>/Sum2'

      rtb_Min = std::sqrt(std::fmax(rtb_Min / (-51.8534F * rtb_Min + 52.8534F),
        0.0F)) * 1080.0F + (rtU.P_nozzle - rtU.P_nozzle_demand) *
        currentControllerGains.pressure.Kp;

      // Saturate: '<S2>/Saturation'
      if (rtb_Min > 1080.0F) {
        // Outport: '<Root>/position_demand'
        rtY.position_demand = 1080.0F;
      } else if (rtb_Min < 0.0F) {
        // Outport: '<Root>/position_demand'
        rtY.position_demand = 0.0F;
      } else {
        // Outport: '<Root>/position_demand'
        rtY.position_demand = rtb_Min;
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
  void pressure::initialize()
  {
    // (no initialization code required)
  }

  // Constructor
  pressure::pressure():
    rtU(),
    rtY(),
    rtDW()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  // Currently there is no destructor body generated.
  pressure::~pressure() = default;
}

//
// File trailer for generated code.
//
// [EOF]
//
