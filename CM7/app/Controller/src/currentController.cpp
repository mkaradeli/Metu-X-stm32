//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: currentController.cpp
//
// Code generated for Simulink model 'currentController'.
//
// Model version                  : 1.119
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Fri Mar 20 14:12:55 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#include "currentController.h"
#include "rtwtypes.h"

namespace controller
{
  // Model step function
  void current::step()
  {
    real32_T rtb_Gain2;
    real32_T u0;

    // Outputs for Enabled SubSystem: '<Root>/Subsystem3' incorporates:
    //   EnablePort: '<S2>/enabled'

    // RelationalOperator: '<S1>/Compare' incorporates:
    //   Constant: '<Root>/Constant'
    //   Constant: '<S1>/Constant'

    if (controller_mode >= controller_modes::CURRENT) {
      if (!rtDW.Subsystem3_MODE) {
        // InitializeConditions for UnitDelay: '<S3>/Delay Input'
        //
        //  Block description for '<S3>/Delay Input':
        //
        //   Store in Global RAM

        rtDW.DelayInput_DSTATE = 0.0F;

        // InitializeConditions for UnitDelay: '<S3>/Delay Output'
        //
        //  Block description for '<S3>/Delay Output':
        //
        //   Store in Global RAM

        rtDW.DelayOutput_DSTATE = 0.0F;
        rtDW.Subsystem3_MODE = true;
      }

      // Gain: '<S2>/Gain2' incorporates:
      //   Gain: '<S2>/Gain1'
      //   Inport: '<Root>/current_feedback'
      //   Inport: '<Root>/current_ref'
      //   Sum: '<S2>/Sum'

      rtb_Gain2 = (rtU.current_ref - rtU.current_feedback) * 5.1125F *
        0.0416666679F;

      // Sum: '<S3>/Sum' incorporates:
      //   Gain: '<S3>/GainPole'
      //   Gain: '<S3>/GainZero'
      //   UnitDelay: '<S3>/Delay Input'
      //   UnitDelay: '<S3>/Delay Output'
      //
      //  Block description for '<S3>/Sum':
      //
      //   Add in CPU
      //
      //  Block description for '<S3>/GainPole':
      //
      //   Multiply in CPU
      //
      //  Block description for '<S3>/GainZero':
      //
      //   Multiply in CPU
      //
      //  Block description for '<S3>/Delay Input':
      //
      //   Store in Global RAM
      //
      //  Block description for '<S3>/Delay Output':
      //
      //   Store in Global RAM

      u0 = (rtb_Gain2 - 0.522F * rtDW.DelayInput_DSTATE) + 0.9999F *
        rtDW.DelayOutput_DSTATE;

      // Saturate: '<S3>/Saturation'
      if (u0 > 1.0F) {
        // Outport: '<Root>/Duty'
        rtY.Duty = 1.0F;
      } else if (u0 < -1.0F) {
        // Outport: '<Root>/Duty'
        rtY.Duty = -1.0F;
      } else {
        // Outport: '<Root>/Duty'
        rtY.Duty = u0;
      }

      // End of Saturate: '<S3>/Saturation'

      // Update for UnitDelay: '<S3>/Delay Input'
      //
      //  Block description for '<S3>/Delay Input':
      //
      //   Store in Global RAM

      rtDW.DelayInput_DSTATE = rtb_Gain2;

      // Update for UnitDelay: '<S3>/Delay Output' incorporates:
      //   Outport: '<Root>/Duty'
      //
      //  Block description for '<S3>/Delay Output':
      //
      //   Store in Global RAM

      rtDW.DelayOutput_DSTATE = rtY.Duty;
    } else if (rtDW.Subsystem3_MODE) {
      // Disable for Outport: '<Root>/Duty' incorporates:
      //   Outport: '<S2>/Duty'

      rtY.Duty = 0.0F;
      rtDW.Subsystem3_MODE = false;
    }

    // End of RelationalOperator: '<S1>/Compare'
    // End of Outputs for SubSystem: '<Root>/Subsystem3'
  }

  // Model initialize function
  void current::initialize()
  {
    // (no initialization code required)
  }

  // Constructor
  current::current():
    rtU(),
    rtY(),
    rtDW()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  // Currently there is no destructor body generated.
  current::~current() = default;
}

//
// File trailer for generated code.
//
// [EOF]
//
