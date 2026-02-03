//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: positionController.cpp
//
// Code generated for Simulink model 'positionController'.
//
// Model version                  : 1.21
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Tue Feb  3 13:11:18 2026
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

const real_T period{ 0.001 };

// Exported block parameters
struct_Wl9576dlIcRv51Sfrsst3G currentControllerGains{
  {
    0.00784151815F,
    712.09436F,
    1.0F,
    -1.0F,
    0.000125
  },

  {
    0.00563351857F,
    0.0252739377F,
    10.0F,
    -10.0F,
    200000.0F,
    -200000.0F,
    0.001
  },

  {
    62.831852F,
    3000.0F,
    -3000.0F,
    3000.0F,
    -3000.0F,
    0.001
  }
} ;                                    // Variable: controllerGains
                                          //  Referenced by:
                                          //    '<Root>/Saturation'
                                          //    '<S2>/Gain'
                                          //    '<S2>/Rate Limiter'
                                          //    '<S2>/Saturation'
                                          //    '<S3>/Discrete-Time Integrator'
                                          //    '<S3>/Gain'
                                          //    '<S3>/Gain1'
                                          //    '<S3>/Gain2'
                                          //    '<S3>/Rate Limiter'
                                          //    '<S3>/Saturation'


// Model step function
void positionController::step()
{
  real32_T rtb_Saturation;
  real32_T rtb_Saturation1;
  real32_T rtb_Sum1;
  real32_T rtb_Sum_a;

  // Saturate: '<Root>/Saturation1' incorporates:
  //   Inport: '<Root>/pos_ref'

  if (rtU.pos_ref > 1440.0F) {
    rtb_Saturation1 = 1440.0F;
  } else if (rtU.pos_ref < 0.0F) {
    rtb_Saturation1 = 0.0F;
  } else {
    rtb_Saturation1 = rtU.pos_ref;
  }

  // End of Saturate: '<Root>/Saturation1'

  // Outputs for Enabled SubSystem: '<Root>/Position Controller' incorporates:
  //   EnablePort: '<S2>/Enable'

  // Inport: '<Root>/position_enable'
  if (rtU.position_enable) {
    if (!rtDW.PositionController_MODE) {
      // InitializeConditions for RateLimiter: '<S2>/Rate Limiter'
      rtDW.PrevY_o = 0.0F;
      rtDW.PositionController_MODE = true;
    }

    // RateLimiter: '<S2>/Rate Limiter' incorporates:
    //   Outport: '<Root>/pos_ref_rate_limited'

    rtb_Sum_a = rtb_Saturation1 - rtDW.PrevY_o;
    rtb_Sum1 = static_cast<real32_T>
      (currentControllerGains.position.RateLimiterMax * period);
    if (rtb_Sum_a > rtb_Sum1) {
      rtY.pos_ref_rate_limited = rtb_Sum1 + rtDW.PrevY_o;
    } else if (rtb_Sum_a < static_cast<real32_T>
               (currentControllerGains.position.RateLimiterMin * period)) {
      rtY.pos_ref_rate_limited = static_cast<real32_T>
        (currentControllerGains.position.RateLimiterMin * period) + rtDW.PrevY_o;
    } else {
      rtY.pos_ref_rate_limited = rtb_Saturation1;
    }

    rtDW.PrevY_o = rtY.pos_ref_rate_limited;

    // End of RateLimiter: '<S2>/Rate Limiter'

    // Gain: '<S2>/Gain' incorporates:
    //   Inport: '<Root>/pos_feedback'
    //   Outport: '<Root>/pos_ref_rate_limited'
    //   Sum: '<S2>/Sum'

    rtb_Saturation1 = (rtY.pos_ref_rate_limited - rtU.pos_feedback) *
      currentControllerGains.position.Kp;

    // Saturate: '<S2>/Saturation'
    if (rtb_Saturation1 > currentControllerGains.position.SatMax) {
      // Saturate: '<S2>/Saturation'
      rtDW.Saturation_l = currentControllerGains.position.SatMax;
    } else if (rtb_Saturation1 < currentControllerGains.position.SatMin) {
      // Saturate: '<S2>/Saturation'
      rtDW.Saturation_l = currentControllerGains.position.SatMin;
    } else {
      // Saturate: '<S2>/Saturation'
      rtDW.Saturation_l = rtb_Saturation1;
    }

    // End of Saturate: '<S2>/Saturation'
  } else if (rtDW.PositionController_MODE) {
    // Disable for Saturate: '<S2>/Saturation' incorporates:
    //   Outport: '<S2>/SpeedDemand'

    rtDW.Saturation_l = 0.0F;
    rtDW.PositionController_MODE = false;
  }

  // End of Inport: '<Root>/position_enable'
  // End of Outputs for SubSystem: '<Root>/Position Controller'

  // SampleTimeMath: '<S1>/TSamp' incorporates:
  //   Outport: '<Root>/pos_ref_rate_limited'
  //
  //  About '<S1>/TSamp':
  //   y = u * K where K = 1 / ( w * Ts )
  //
  rtb_Saturation1 = rtY.pos_ref_rate_limited * 1000.0F;

  // Sum: '<Root>/Sum' incorporates:
  //   Sum: '<S1>/Diff'
  //   UnitDelay: '<S1>/UD'
  //
  //  Block description for '<S1>/Diff':
  //
  //   Add in CPU
  //
  //  Block description for '<S1>/UD':
  //
  //   Store in Global RAM

  rtb_Saturation = (rtb_Saturation1 - rtDW.UD_DSTATE) + rtDW.Saturation_l;

  // Saturate: '<Root>/Saturation'
  if (rtb_Saturation > currentControllerGains.position.SatMax) {
    rtb_Saturation = currentControllerGains.position.SatMax;
  } else if (rtb_Saturation < currentControllerGains.position.SatMin) {
    rtb_Saturation = currentControllerGains.position.SatMin;
  }

  // End of Saturate: '<Root>/Saturation'

  // Outputs for Enabled SubSystem: '<Root>/Speed Controller' incorporates:
  //   EnablePort: '<S3>/Enable'

  // Inport: '<Root>/speed_enable'
  if (rtU.speed_enable) {
    if (!rtDW.SpeedController_MODE) {
      // InitializeConditions for DiscreteIntegrator: '<S3>/Discrete-Time Integrator' 
      rtDW.DiscreteTimeIntegrator_DSTATE = 0.0F;

      // InitializeConditions for RateLimiter: '<S3>/Rate Limiter'
      rtDW.PrevY = 0.0F;
      rtDW.SpeedController_MODE = true;
    }

    // DiscreteIntegrator: '<S3>/Discrete-Time Integrator'
    rtDW.Ui = rtDW.DiscreteTimeIntegrator_DSTATE;

    // RateLimiter: '<S3>/Rate Limiter'
    rtb_Sum_a = rtb_Saturation - rtDW.PrevY;
    rtb_Sum1 = static_cast<real32_T>(currentControllerGains.speed.RateLimiterMax
      * period);
    if (rtb_Sum_a > rtb_Sum1) {
      // RateLimiter: '<S3>/Rate Limiter'
      rtDW.RateLimiter = rtb_Sum1 + rtDW.PrevY;
    } else if (rtb_Sum_a < static_cast<real32_T>
               (currentControllerGains.speed.RateLimiterMin * period)) {
      // RateLimiter: '<S3>/Rate Limiter'
      rtDW.RateLimiter = static_cast<real32_T>
        (currentControllerGains.speed.RateLimiterMin * period) + rtDW.PrevY;
    } else {
      // RateLimiter: '<S3>/Rate Limiter'
      rtDW.RateLimiter = rtb_Saturation;
    }

    rtDW.PrevY = rtDW.RateLimiter;

    // End of RateLimiter: '<S3>/Rate Limiter'

    // Sum: '<S3>/Sum' incorporates:
    //   Inport: '<Root>/speed_feedback'

    rtb_Sum_a = rtDW.RateLimiter - rtU.SpeedFeedback;

    // Gain: '<S3>/Gain'
    rtDW.Up = currentControllerGains.speed.Kp * rtb_Sum_a;

    // Sum: '<S3>/Sum1'
    rtb_Sum1 = rtDW.Up + rtDW.Ui;

    // Saturate: '<S3>/Saturation'
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

    // End of Saturate: '<S3>/Saturation'

    // Update for DiscreteIntegrator: '<S3>/Discrete-Time Integrator' incorporates:
    //   Gain: '<S3>/Gain1'
    //   Gain: '<S3>/Gain2'
    //   Outport: '<Root>/currentDemand'
    //   Sum: '<S3>/Sum2'
    //   Sum: '<S3>/Sum3'

    rtDW.DiscreteTimeIntegrator_DSTATE += (currentControllerGains.speed.Ki /
      currentControllerGains.speed.Kp * (rtY.currentDemand - rtb_Sum1) +
      currentControllerGains.speed.Ki * rtb_Sum_a) * 0.001F;
    if (rtDW.DiscreteTimeIntegrator_DSTATE > currentControllerGains.speed.SatMax)
    {
      rtDW.DiscreteTimeIntegrator_DSTATE = currentControllerGains.speed.SatMax;
    } else if (rtDW.DiscreteTimeIntegrator_DSTATE <
               currentControllerGains.speed.SatMin) {
      rtDW.DiscreteTimeIntegrator_DSTATE = currentControllerGains.speed.SatMin;
    }

    // End of Update for DiscreteIntegrator: '<S3>/Discrete-Time Integrator'
  } else if (rtDW.SpeedController_MODE) {
    // Disable for Outport: '<Root>/currentDemand' incorporates:
    //   Outport: '<S3>/Y'

    rtY.currentDemand = 0.0F;
    rtDW.SpeedController_MODE = false;
  }

  // End of Inport: '<Root>/speed_enable'
  // End of Outputs for SubSystem: '<Root>/Speed Controller'

  // BusCreator generated from: '<Root>/speedDebug' incorporates:
  //   Outport: '<Root>/speedDebug'

  rtY.speedDebug.Up = rtDW.Up;
  rtY.speedDebug.Ui = rtDW.Ui;
  rtY.speedDebug.ref_rate_limited = rtDW.RateLimiter;

  // Outport: '<Root>/speedDemand'
  rtY.speedDemand = rtb_Saturation;

  // Update for UnitDelay: '<S1>/UD'
  //
  //  Block description for '<S1>/UD':
  //
  //   Store in Global RAM

  rtDW.UD_DSTATE = rtb_Saturation1;
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
