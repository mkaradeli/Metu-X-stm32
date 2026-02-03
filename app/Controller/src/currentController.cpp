//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: currentController.cpp
//
// Code generated for Simulink model 'currentController'.
//
// Model version                  : 1.77
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Tue Feb  3 13:11:00 2026
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

// Model step function
void CurrentController::step()
{
  real32_T rtb_Gain2;
  real32_T rtb_Saturation;
  real32_T rtb_Saturation_g;
  real32_T rtb_Sum;

  // Outputs for Enabled SubSystem: '<Root>/Subsystem3' incorporates:
  //   EnablePort: '<S1>/enabled'

  // Inport: '<Root>/enabled'
  if (rtU.enabled) {
    if (!rtDW.Subsystem3_MODE) {
      // InitializeConditions for UnitDelay: '<S2>/Delay Input'
      //
      //  Block description for '<S2>/Delay Input':
      //
      //   Store in Global RAM

      rtDW.DelayInput_DSTATE = rtP.ChatGPT_ICPrevInput;

      // InitializeConditions for UnitDelay: '<S2>/Delay Output'
      //
      //  Block description for '<S2>/Delay Output':
      //
      //   Store in Global RAM

      rtDW.DelayOutput_DSTATE = rtP.ChatGPT_ICPrevOutput;

      // InitializeConditions for UnitDelay: '<S3>/Delay Output'
      //
      //  Block description for '<S3>/Delay Output':
      //
      //   Store in Global RAM

      rtDW.DelayOutput_DSTATE_l = rtP.GORKEM_ICPrevOutput;

      // InitializeConditions for UnitDelay: '<S3>/Delay Input'
      //
      //  Block description for '<S3>/Delay Input':
      //
      //   Store in Global RAM

      rtDW.DelayInput_DSTATE_p = rtP.GORKEM_ICPrevInput;
      rtDW.Subsystem3_MODE = true;
    }

    // Sum: '<S1>/Sum' incorporates:
    //   Inport: '<Root>/current_feedback'
    //   Inport: '<Root>/current_ref'

    rtb_Sum = rtU.current_ref - rtU.current_feedback;

    // Gain: '<S1>/Gain2' incorporates:
    //   Gain: '<S1>/Gain5'

    rtb_Gain2 = rtP.Gain5_Gain * rtb_Sum * rtP.Gain2_Gain;

    // Sum: '<S2>/Sum' incorporates:
    //   Gain: '<S2>/GainPole'
    //   Gain: '<S2>/GainZero'
    //   UnitDelay: '<S2>/Delay Input'
    //   UnitDelay: '<S2>/Delay Output'
    //
    //  Block description for '<S2>/Sum':
    //
    //   Add in CPU
    //
    //  Block description for '<S2>/GainPole':
    //
    //   Multiply in CPU
    //
    //  Block description for '<S2>/GainZero':
    //
    //   Multiply in CPU
    //
    //  Block description for '<S2>/Delay Input':
    //
    //   Store in Global RAM
    //
    //  Block description for '<S2>/Delay Output':
    //
    //   Store in Global RAM

    rtb_Saturation = (rtb_Gain2 - rtP.ChatGPT_ZeroZ * rtDW.DelayInput_DSTATE) +
      rtP.ChatGPT_PoleZ * rtDW.DelayOutput_DSTATE;

    // Saturate: '<S2>/Saturation'
    if (rtb_Saturation > rtP.Saturation_UpperSat) {
      rtb_Saturation = rtP.Saturation_UpperSat;
    } else if (rtb_Saturation < rtP.Saturation_LowerSat) {
      rtb_Saturation = rtP.Saturation_LowerSat;
    }

    // End of Saturate: '<S2>/Saturation'

    // Gain: '<S1>/Gain4' incorporates:
    //   Gain: '<S1>/Gain1'

    rtb_Sum = rtP.Gain1_Gain * rtb_Sum * rtP.Gain4_Gain;

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

    rtb_Saturation_g = (rtb_Sum - rtP.GORKEM_ZeroZ * rtDW.DelayInput_DSTATE_p) +
      rtP.GORKEM_PoleZ * rtDW.DelayOutput_DSTATE_l;

    // Saturate: '<S3>/Saturation'
    if (rtb_Saturation_g > rtP.Saturation_UpperSat_l) {
      rtb_Saturation_g = rtP.Saturation_UpperSat_l;
    } else if (rtb_Saturation_g < rtP.Saturation_LowerSat_j) {
      rtb_Saturation_g = rtP.Saturation_LowerSat_j;
    }

    // End of Saturate: '<S3>/Saturation'

    // ManualSwitch: '<S1>/Manual Switch'
    if (rtP.ManualSwitch_CurrentSetting == 1) {
      // Outport: '<Root>/Duty'
      rtY.Duty = rtb_Saturation_g;
    } else {
      // Outport: '<Root>/Duty'
      rtY.Duty = rtb_Saturation;
    }

    // End of ManualSwitch: '<S1>/Manual Switch'

    // Update for UnitDelay: '<S2>/Delay Input'
    //
    //  Block description for '<S2>/Delay Input':
    //
    //   Store in Global RAM

    rtDW.DelayInput_DSTATE = rtb_Gain2;

    // Update for UnitDelay: '<S2>/Delay Output'
    //
    //  Block description for '<S2>/Delay Output':
    //
    //   Store in Global RAM

    rtDW.DelayOutput_DSTATE = rtb_Saturation;

    // Update for UnitDelay: '<S3>/Delay Output'
    //
    //  Block description for '<S3>/Delay Output':
    //
    //   Store in Global RAM

    rtDW.DelayOutput_DSTATE_l = rtb_Saturation_g;

    // Update for UnitDelay: '<S3>/Delay Input'
    //
    //  Block description for '<S3>/Delay Input':
    //
    //   Store in Global RAM

    rtDW.DelayInput_DSTATE_p = rtb_Sum;
  } else if (rtDW.Subsystem3_MODE) {
    // Disable for Outport: '<Root>/Duty' incorporates:
    //   Outport: '<S1>/Duty'

    rtY.Duty = rtP.Duty_Y0;
    rtDW.Subsystem3_MODE = false;
  }

  // End of Inport: '<Root>/enabled'
  // End of Outputs for SubSystem: '<Root>/Subsystem3'
}

// Model initialize function
void CurrentController::initialize()
{
  // SystemInitialize for Enabled SubSystem: '<Root>/Subsystem3'
  // InitializeConditions for UnitDelay: '<S2>/Delay Input'
  //
  //  Block description for '<S2>/Delay Input':
  //
  //   Store in Global RAM

  rtDW.DelayInput_DSTATE = rtP.ChatGPT_ICPrevInput;

  // InitializeConditions for UnitDelay: '<S2>/Delay Output'
  //
  //  Block description for '<S2>/Delay Output':
  //
  //   Store in Global RAM

  rtDW.DelayOutput_DSTATE = rtP.ChatGPT_ICPrevOutput;

  // InitializeConditions for UnitDelay: '<S3>/Delay Output'
  //
  //  Block description for '<S3>/Delay Output':
  //
  //   Store in Global RAM

  rtDW.DelayOutput_DSTATE_l = rtP.GORKEM_ICPrevOutput;

  // InitializeConditions for UnitDelay: '<S3>/Delay Input'
  //
  //  Block description for '<S3>/Delay Input':
  //
  //   Store in Global RAM

  rtDW.DelayInput_DSTATE_p = rtP.GORKEM_ICPrevInput;

  // SystemInitialize for Outport: '<Root>/Duty' incorporates:
  //   Outport: '<S1>/Duty'

  rtY.Duty = rtP.Duty_Y0;

  // End of SystemInitialize for SubSystem: '<Root>/Subsystem3'
}

// Constructor
CurrentController::CurrentController():
  rtU(),
  rtY(),
  rtDW()
{
  // Currently there is no constructor body generated.
}

// Destructor
// Currently there is no destructor body generated.
CurrentController::~CurrentController() = default;

//
// File trailer for generated code.
//
// [EOF]
//
