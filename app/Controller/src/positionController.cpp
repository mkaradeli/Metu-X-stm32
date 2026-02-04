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
// C/C++ source code generated on : Tue Feb  3 20:34:31 2026
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
                                          //    '<S4>/Gain'
                                          //    '<S4>/Rate Limiter'
                                          //    '<S4>/Saturation'
                                          //    '<S5>/Discrete-Time Integrator'
                                          //    '<S5>/Gain'
                                          //    '<S5>/Gain1'
                                          //    '<S5>/Gain2'
                                          //    '<S5>/Rate Limiter'
                                          //    '<S5>/Saturation'


controller_modes controller_mode{ controller_modes::DISABLE };// Variable: controller_mode
                                                                 //  Referenced by:
                                                                 //    '<Root>/Constant'
                                                                 //    '<Root>/Constant1'


// Model step function
void positionController::step()
{
  real32_T rtb_Saturation;
  real32_T rtb_Sum1;
  real32_T rtb_Sum_a;
  real32_T rtb_TSamp;

  // Outputs for Enabled SubSystem: '<Root>/Position Controller' incorporates:
  //   EnablePort: '<S4>/Enable'

  // RelationalOperator: '<S2>/Compare' incorporates:
  //   Constant: '<Root>/Constant1'
  //   Constant: '<S2>/Constant'

  if (controller_mode >= controller_modes::POSITION) {
    if (!rtDW.PositionController_MODE) {
      // InitializeConditions for RateLimiter: '<S4>/Rate Limiter'
      rtDW.PrevY_o = 0.0F;
      rtDW.PositionController_MODE = true;
    }

    // RateLimiter: '<S4>/Rate Limiter' incorporates:
    //   Inport: '<Root>/pos_ref'
    //   Outport: '<Root>/pos_ref_rate_limited'

    rtb_Sum_a = rtU.pos_ref - rtDW.PrevY_o;
    rtb_Sum1 = static_cast<real32_T>
      (currentControllerGains.position.RateLimiterMax * period);
    if (rtb_Sum_a > rtb_Sum1) {
      rtY.pos_ref_rate_limited = rtb_Sum1 + rtDW.PrevY_o;
    } else if (rtb_Sum_a < static_cast<real32_T>
               (currentControllerGains.position.RateLimiterMin * period)) {
      rtY.pos_ref_rate_limited = static_cast<real32_T>
        (currentControllerGains.position.RateLimiterMin * period) + rtDW.PrevY_o;
    } else {
      rtY.pos_ref_rate_limited = rtU.pos_ref;
    }

    rtDW.PrevY_o = rtY.pos_ref_rate_limited;

    // End of RateLimiter: '<S4>/Rate Limiter'

    // Gain: '<S4>/Gain' incorporates:
    //   Inport: '<Root>/pos_feedback'
    //   Outport: '<Root>/pos_ref_rate_limited'
    //   Sum: '<S4>/Sum'

    rtb_Sum_a = (rtY.pos_ref_rate_limited - rtU.pos_feedback) *
      currentControllerGains.position.Kp;

    // Saturate: '<S4>/Saturation'
    if (rtb_Sum_a > currentControllerGains.position.SatMax) {
      // Saturate: '<S4>/Saturation'
      rtDW.Saturation_l = currentControllerGains.position.SatMax;
    } else if (rtb_Sum_a < currentControllerGains.position.SatMin) {
      // Saturate: '<S4>/Saturation'
      rtDW.Saturation_l = currentControllerGains.position.SatMin;
    } else {
      // Saturate: '<S4>/Saturation'
      rtDW.Saturation_l = rtb_Sum_a;
    }

    // End of Saturate: '<S4>/Saturation'
  } else if (rtDW.PositionController_MODE) {
    // Disable for Saturate: '<S4>/Saturation' incorporates:
    //   Outport: '<S4>/SpeedDemand'

    rtDW.Saturation_l = 0.0F;
    rtDW.PositionController_MODE = false;
  }

  // End of RelationalOperator: '<S2>/Compare'
  // End of Outputs for SubSystem: '<Root>/Position Controller'

  // SampleTimeMath: '<S3>/TSamp' incorporates:
  //   Outport: '<Root>/pos_ref_rate_limited'
  //
  //  About '<S3>/TSamp':
  //   y = u * K where K = 1 / ( w * Ts )
  //
  rtb_TSamp = rtY.pos_ref_rate_limited * 1000.0F;

  // Sum: '<Root>/Sum' incorporates:
  //   Sum: '<S3>/Diff'
  //   UnitDelay: '<S3>/UD'
  //
  //  Block description for '<S3>/Diff':
  //
  //   Add in CPU
  //
  //  Block description for '<S3>/UD':
  //
  //   Store in Global RAM

  rtb_Saturation = (rtb_TSamp - rtDW.UD_DSTATE) + rtDW.Saturation_l;

  // Saturate: '<Root>/Saturation'
  if (rtb_Saturation > currentControllerGains.position.SatMax) {
    rtb_Saturation = currentControllerGains.position.SatMax;
  } else if (rtb_Saturation < currentControllerGains.position.SatMin) {
    rtb_Saturation = currentControllerGains.position.SatMin;
  }

  // End of Saturate: '<Root>/Saturation'

  // Outputs for Enabled SubSystem: '<Root>/Speed Controller' incorporates:
  //   EnablePort: '<S5>/Enable'

  // RelationalOperator: '<S1>/Compare' incorporates:
  //   Constant: '<Root>/Constant'
  //   Constant: '<S1>/Constant'

  if (controller_mode >= controller_modes::SPEED) {
    if (!rtDW.SpeedController_MODE) {
      // InitializeConditions for DiscreteIntegrator: '<S5>/Discrete-Time Integrator' 
      rtDW.DiscreteTimeIntegrator_DSTATE = 0.0F;

      // InitializeConditions for RateLimiter: '<S5>/Rate Limiter'
      rtDW.PrevY = 0.0F;
      rtDW.SpeedController_MODE = true;
    }

    // DiscreteIntegrator: '<S5>/Discrete-Time Integrator'
    rtDW.Ui = rtDW.DiscreteTimeIntegrator_DSTATE;

    // RateLimiter: '<S5>/Rate Limiter'
    rtb_Sum_a = rtb_Saturation - rtDW.PrevY;
    rtb_Sum1 = static_cast<real32_T>(currentControllerGains.speed.RateLimiterMax
      * period);
    if (rtb_Sum_a > rtb_Sum1) {
      // RateLimiter: '<S5>/Rate Limiter'
      rtDW.RateLimiter = rtb_Sum1 + rtDW.PrevY;
    } else if (rtb_Sum_a < static_cast<real32_T>
               (currentControllerGains.speed.RateLimiterMin * period)) {
      // RateLimiter: '<S5>/Rate Limiter'
      rtDW.RateLimiter = static_cast<real32_T>
        (currentControllerGains.speed.RateLimiterMin * period) + rtDW.PrevY;
    } else {
      // RateLimiter: '<S5>/Rate Limiter'
      rtDW.RateLimiter = rtb_Saturation;
    }

    rtDW.PrevY = rtDW.RateLimiter;

    // End of RateLimiter: '<S5>/Rate Limiter'

    // Sum: '<S5>/Sum' incorporates:
    //   Inport: '<Root>/speed_feedback'

    rtb_Sum_a = rtDW.RateLimiter - rtU.SpeedFeedback;

    // Gain: '<S5>/Gain'
    rtDW.Up = currentControllerGains.speed.Kp * rtb_Sum_a;

    // Sum: '<S5>/Sum1'
    rtb_Sum1 = rtDW.Up + rtDW.Ui;

    // Saturate: '<S5>/Saturation'
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

    // End of Saturate: '<S5>/Saturation'

    // Update for DiscreteIntegrator: '<S5>/Discrete-Time Integrator' incorporates:
    //   Gain: '<S5>/Gain1'
    //   Gain: '<S5>/Gain2'
    //   Outport: '<Root>/currentDemand'
    //   Sum: '<S5>/Sum2'
    //   Sum: '<S5>/Sum3'

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

    // End of Update for DiscreteIntegrator: '<S5>/Discrete-Time Integrator'
  } else if (rtDW.SpeedController_MODE) {
    // Disable for Outport: '<Root>/currentDemand' incorporates:
    //   Outport: '<S5>/Y'

    rtY.currentDemand = 0.0F;
    rtDW.SpeedController_MODE = false;
  }

  // End of RelationalOperator: '<S1>/Compare'
  // End of Outputs for SubSystem: '<Root>/Speed Controller'

  // BusCreator generated from: '<Root>/speedDebug' incorporates:
  //   Outport: '<Root>/speedDebug'

  rtY.speedDebug.Up = rtDW.Up;
  rtY.speedDebug.Ui = rtDW.Ui;
  rtY.speedDebug.ref_rate_limited = rtDW.RateLimiter;

  // Outport: '<Root>/speedDemand'
  rtY.speedDemand = rtb_Saturation;

  // Update for UnitDelay: '<S3>/UD'
  //
  //  Block description for '<S3>/UD':
  //
  //   Store in Global RAM

  rtDW.UD_DSTATE = rtb_TSamp;
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
