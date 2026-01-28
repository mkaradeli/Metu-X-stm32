//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: currentController.cpp
//
// Code generated for Simulink model 'currentController'.
//
// Model version                  : 1.19
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Wed Jan 28 20:30:42 2026
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

// Exported block parameters
struct_WlbPQVH06SMTAC0rh3ezxC currentControllerGains{
  {
    0.00784151815F,
    712.09436F,
    1.0F,
    -1.0F,
    0.000125
  },

  {
    0.00375567912F,
    0.0252739377F,
    10.0F,
    -10.0F,
    0.001
  },

  {
    62.831852F,
    3000.0F,
    -3000.0F,
    0.001
  }
} ;                                    // Variable: controllerGains
                                          //  Referenced by:
                                          //    '<S1>/Discrete-Time Integrator'
                                          //    '<S1>/Gain'
                                          //    '<S1>/Gain1'
                                          //    '<S1>/Gain2'
                                          //    '<S1>/Saturation'


// Model step function
void CurrentController::step()
{
  real32_T rtb_Sum;
  real32_T rtb_Sum1;

  // Outputs for Enabled SubSystem: '<Root>/PI_Controller' incorporates:
  //   EnablePort: '<S1>/Enable'

  // Inport: '<Root>/enabled'
  if (rtU.enabled) {
    if (!rtDW.PI_Controller_MODE) {
      // InitializeConditions for DiscreteIntegrator: '<S1>/Discrete-Time Integrator' 
      rtDW.DiscreteTimeIntegrator_DSTATE = rtP.DiscreteTimeIntegrator_IC;
      rtDW.PI_Controller_MODE = true;
    }

    // Sum: '<S1>/Sum' incorporates:
    //   Inport: '<Root>/current_feedback'
    //   Inport: '<Root>/current_ref'

    rtb_Sum = rtU.current_ref - rtU.current_feedback;

    // Sum: '<S1>/Sum1' incorporates:
    //   DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
    //   Gain: '<S1>/Gain'

    rtb_Sum1 = currentControllerGains.current.Kp * rtb_Sum +
      rtDW.DiscreteTimeIntegrator_DSTATE;

    // Saturate: '<S1>/Saturation'
    if (rtb_Sum1 > currentControllerGains.current.SatMax) {
      // Outport: '<Root>/Duty'
      rtY.Duty = currentControllerGains.current.SatMax;
    } else if (rtb_Sum1 < currentControllerGains.current.SatMin) {
      // Outport: '<Root>/Duty'
      rtY.Duty = currentControllerGains.current.SatMin;
    } else {
      // Outport: '<Root>/Duty'
      rtY.Duty = rtb_Sum1;
    }

    // End of Saturate: '<S1>/Saturation'

    // Update for DiscreteIntegrator: '<S1>/Discrete-Time Integrator' incorporates:
    //   Gain: '<S1>/Gain1'
    //   Gain: '<S1>/Gain2'
    //   Outport: '<Root>/Duty'
    //   Sum: '<S1>/Sum2'
    //   Sum: '<S1>/Sum3'

    rtDW.DiscreteTimeIntegrator_DSTATE += (currentControllerGains.current.Ki /
      currentControllerGains.current.Kp * (rtY.Duty - rtb_Sum1) +
      currentControllerGains.current.Ki * rtb_Sum) *
      rtP.DiscreteTimeIntegrator_gainval;
    if (rtDW.DiscreteTimeIntegrator_DSTATE >
        currentControllerGains.current.SatMax) {
      rtDW.DiscreteTimeIntegrator_DSTATE = currentControllerGains.current.SatMax;
    } else if (rtDW.DiscreteTimeIntegrator_DSTATE <
               currentControllerGains.current.SatMin) {
      rtDW.DiscreteTimeIntegrator_DSTATE = currentControllerGains.current.SatMin;
    }

    // End of Update for DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
  } else if (rtDW.PI_Controller_MODE) {
    // Disable for Outport: '<Root>/Duty' incorporates:
    //   Outport: '<S1>/Y'

    rtY.Duty = rtP.Y_Y0;
    rtDW.PI_Controller_MODE = false;
  }

  // End of Inport: '<Root>/enabled'
  // End of Outputs for SubSystem: '<Root>/PI_Controller'
}

// Model initialize function
void CurrentController::initialize()
{
  // SystemInitialize for Enabled SubSystem: '<Root>/PI_Controller'
  // InitializeConditions for DiscreteIntegrator: '<S1>/Discrete-Time Integrator' 
  rtDW.DiscreteTimeIntegrator_DSTATE = rtP.DiscreteTimeIntegrator_IC;

  // SystemInitialize for Outport: '<Root>/Duty' incorporates:
  //   Outport: '<S1>/Y'

  rtY.Duty = rtP.Y_Y0;

  // End of SystemInitialize for SubSystem: '<Root>/PI_Controller'
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
