//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: positionController.cpp
//
// Code generated for Simulink model 'positionController'.
//
// Model version                  : 1.8
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Wed Jan 28 20:38:14 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#include "positionController.h"
#include "rtwtypes.h"

// Model step function
void positionController::step()
{
  real32_T rtb_Gain1;
  real32_T rtb_Sum1;
  real32_T rtb_Up;

  // Outputs for Enabled SubSystem: '<Root>/Position Controller' incorporates:
  //   EnablePort: '<S1>/Enable'

  // Inport: '<Root>/position_enable'
  if (rtU.position_enable) {
    rtDW.PositionController_MODE = true;

    // Gain: '<S1>/Gain' incorporates:
    //   Inport: '<Root>/pos_feedback'
    //   Inport: '<Root>/pos_ref'
    //   Sum: '<S1>/Sum'

    rtb_Up = (rtU.pos_ref - rtU.pos_feedback) *
      currentControllerGains.position.Kp;

    // Saturate: '<S1>/Saturation'
    if (rtb_Up > currentControllerGains.position.SatMax) {
      // Outport: '<Root>/speedDemand'
      rtY.speedDemand = currentControllerGains.position.SatMax;
    } else if (rtb_Up < currentControllerGains.position.SatMin) {
      // Outport: '<Root>/speedDemand'
      rtY.speedDemand = currentControllerGains.position.SatMin;
    } else {
      // Outport: '<Root>/speedDemand'
      rtY.speedDemand = rtb_Up;
    }

    // End of Saturate: '<S1>/Saturation'
  } else if (rtDW.PositionController_MODE) {
    // Disable for Outport: '<Root>/speedDemand' incorporates:
    //   Outport: '<S1>/SpeedDemand'

    rtY.speedDemand = 0.0F;
    rtDW.PositionController_MODE = false;
  }

  // End of Inport: '<Root>/position_enable'
  // End of Outputs for SubSystem: '<Root>/Position Controller'

  // Outputs for Enabled SubSystem: '<Root>/Speed Controller' incorporates:
  //   EnablePort: '<S2>/Enable'

  // Inport: '<Root>/speed_enable'
  if (rtU.speed_enable) {
    if (!rtDW.SpeedController_MODE) {
      // InitializeConditions for DiscreteIntegrator: '<S2>/Discrete-Time Integrator' 
      rtDW.DiscreteTimeIntegrator_DSTATE = 0.0F;
      rtDW.SpeedController_MODE = true;
    }

    // Sum: '<S2>/Sum' incorporates:
    //   Inport: '<Root>/speed_feedback'
    //   Outport: '<Root>/speedDemand'

    rtb_Up = rtY.speedDemand - rtU.SpeedFeedback;

    // Gain: '<S2>/Gain1'
    rtb_Gain1 = currentControllerGains.speed.Ki * rtb_Up;

    // Gain: '<S2>/Gain'
    rtb_Up *= currentControllerGains.speed.Kp;

    // Sum: '<S2>/Sum1' incorporates:
    //   DiscreteIntegrator: '<S2>/Discrete-Time Integrator'

    rtb_Sum1 = rtb_Up + rtDW.DiscreteTimeIntegrator_DSTATE;

    // Saturate: '<S2>/Saturation'
    if (rtb_Sum1 > currentControllerGains.speed.SatMax) {
      // Outport: '<Root>/currentDemand'
      rtY.currentDemand = currentControllerGains.speed.SatMax;
    } else if (rtb_Sum1 < currentControllerGains.speed.SatMin) {
      // Outport: '<Root>/currentDemand'
      rtY.currentDemand = currentControllerGains.speed.SatMin;
    } else {
      // Outport: '<Root>/currentDemand'
      rtY.currentDemand = rtb_Sum1;
    }

    // End of Saturate: '<S2>/Saturation'

    // BusCreator: '<S2>/Bus Creator' incorporates:
    //   DiscreteIntegrator: '<S2>/Discrete-Time Integrator'
    //   Outport: '<Root>/speedDebug'

    rtY.speedDebug.Up = rtb_Up;
    rtY.speedDebug.Ui = rtDW.DiscreteTimeIntegrator_DSTATE;

    // Update for DiscreteIntegrator: '<S2>/Discrete-Time Integrator' incorporates:
    //   Gain: '<S2>/Gain2'
    //   Outport: '<Root>/currentDemand'
    //   Sum: '<S2>/Sum2'
    //   Sum: '<S2>/Sum3'

    rtDW.DiscreteTimeIntegrator_DSTATE += (currentControllerGains.speed.Ki /
      currentControllerGains.speed.Kp * (rtY.currentDemand - rtb_Sum1) +
      rtb_Gain1) * 0.001F;
    if (rtDW.DiscreteTimeIntegrator_DSTATE > currentControllerGains.speed.SatMax)
    {
      rtDW.DiscreteTimeIntegrator_DSTATE = currentControllerGains.speed.SatMax;
    } else if (rtDW.DiscreteTimeIntegrator_DSTATE <
               currentControllerGains.speed.SatMin) {
      rtDW.DiscreteTimeIntegrator_DSTATE = currentControllerGains.speed.SatMin;
    }

    // End of Update for DiscreteIntegrator: '<S2>/Discrete-Time Integrator'
  } else if (rtDW.SpeedController_MODE) {
    // Disable for Outport: '<Root>/currentDemand' incorporates:
    //   Outport: '<S2>/Y'

    rtY.currentDemand = 0.0F;
    rtDW.SpeedController_MODE = false;
  }

  // End of Inport: '<Root>/speed_enable'
  // End of Outputs for SubSystem: '<Root>/Speed Controller'
}

// Model initialize function
void positionController::initialize()
{
  // (no initialization code required)
}

// Constructor
positionController::positionController():
  rtU(),
  rtY(),
  rtDW()
{
  // Currently there is no constructor body generated.
}

// Destructor
// Currently there is no destructor body generated.
positionController::~positionController() = default;

//
// File trailer for generated code.
//
// [EOF]
//
