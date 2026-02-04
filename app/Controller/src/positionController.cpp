//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: positionController.cpp
//
// Code generated for Simulink model 'positionController'.
//
// Model version                  : 1.26
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Wed Feb  4 23:12:10 2026
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
struct_bcCVSDe9ijc6YF0EEo7cx currentControllerGains{
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
    15.0F,
    -15.0F,
    200000.0F,
    -200000.0F,
    0.001
  },

  {
    62.831852F,
    0.0F,
    2400.0F,
    -2400.0F,
    2400.0F,
    -2400.0F,
    0.001
  }
} ;                                    // Variable: controllerGains
                                          //  Referenced by:
                                          //    '<S3>/Gain'
                                          //    '<S3>/Gain1'
                                          //    '<S3>/Saturation'
                                          //    '<S4>/Discrete-Time Integrator'
                                          //    '<S4>/Gain'
                                          //    '<S4>/Gain1'
                                          //    '<S4>/Gain2'
                                          //    '<S4>/Rate Limiter'
                                          //    '<S4>/Saturation'
                                          //    '<S5>/Rate Limiter'


controller_modes controller_mode{ controller_modes::DISABLE };// Variable: controller_mode
                                                                 //  Referenced by: '<Root>/controller_mode'


// Model step function
void positionController::step()
{
  real32_T rtb_RateLimiter;
  real32_T rtb_Sum1;
  real32_T rtb_Sum_a;
  real32_T tmp;

  // Saturate: '<S5>/Saturation1' incorporates:
  //   Inport: '<Root>/pos_ref'

  if (rtU.pos_ref > 1440.0F) {
    rtb_Sum_a = 1440.0F;
  } else if (rtU.pos_ref < 0.0F) {
    rtb_Sum_a = 0.0F;
  } else {
    rtb_Sum_a = rtU.pos_ref;
  }

  // End of Saturate: '<S5>/Saturation1'

  // RateLimiter: '<S5>/Rate Limiter'
  rtb_Sum1 = rtb_Sum_a - rtDW.PrevY;
  tmp = static_cast<real32_T>(currentControllerGains.position.RateLimiterMax *
    period);
  if (rtb_Sum1 > tmp) {
    rtb_RateLimiter = tmp + rtDW.PrevY;
  } else if (rtb_Sum1 < static_cast<real32_T>
             (currentControllerGains.position.RateLimiterMin * period)) {
    rtb_RateLimiter = static_cast<real32_T>
      (currentControllerGains.position.RateLimiterMin * period) + rtDW.PrevY;
  } else {
    rtb_RateLimiter = rtb_Sum_a;
  }

  rtDW.PrevY = rtb_RateLimiter;

  // End of RateLimiter: '<S5>/Rate Limiter'

  // Outputs for Enabled SubSystem: '<Root>/Position Controller' incorporates:
  //   EnablePort: '<S3>/Enable'

  // RelationalOperator: '<S2>/Compare' incorporates:
  //   Constant: '<Root>/controller_mode'
  //   Constant: '<S2>/Constant'

  if (controller_mode >= controller_modes::POSITION) {
    if (!rtDW.PositionController_MODE) {
      // InitializeConditions for UnitDelay: '<S6>/UD'
      //
      //  Block description for '<S6>/UD':
      //
      //   Store in Global RAM

      rtDW.UD_DSTATE = 0.0F;
      rtDW.PositionController_MODE = true;
    }

    // SampleTimeMath: '<S6>/TSamp'
    //
    //  About '<S6>/TSamp':
    //   y = u * K where K = 1 / ( w * Ts )
    //
    rtb_Sum_a = rtb_RateLimiter * 1000.0F;

    // Sum: '<S3>/Sum1' incorporates:
    //   Gain: '<S3>/Gain'
    //   Gain: '<S3>/Gain1'
    //   Inport: '<Root>/pos_feedback'
    //   Sum: '<S3>/Sum'
    //   Sum: '<S6>/Diff'
    //   UnitDelay: '<S6>/UD'
    //
    //  Block description for '<S6>/Diff':
    //
    //   Add in CPU
    //
    //  Block description for '<S6>/UD':
    //
    //   Store in Global RAM

    rtb_Sum1 = (rtb_Sum_a - rtDW.UD_DSTATE) *
      currentControllerGains.position.Kff + (rtb_RateLimiter - rtU.pos_feedback)
      * currentControllerGains.position.Kp;

    // Saturate: '<S3>/Saturation'
    if (rtb_Sum1 > currentControllerGains.position.SatMax) {
      // Outport: '<Root>/speedDemand'
      rtY.speedDemand = currentControllerGains.position.SatMax;
    } else if (rtb_Sum1 < currentControllerGains.position.SatMin) {
      // Outport: '<Root>/speedDemand'
      rtY.speedDemand = currentControllerGains.position.SatMin;
    } else {
      // Outport: '<Root>/speedDemand'
      rtY.speedDemand = rtb_Sum1;
    }

    // End of Saturate: '<S3>/Saturation'

    // Update for UnitDelay: '<S6>/UD'
    //
    //  Block description for '<S6>/UD':
    //
    //   Store in Global RAM

    rtDW.UD_DSTATE = rtb_Sum_a;
  } else if (rtDW.PositionController_MODE) {
    // Disable for Outport: '<Root>/speedDemand' incorporates:
    //   Outport: '<S3>/SpeedDemand'

    rtY.speedDemand = 0.0F;
    rtDW.PositionController_MODE = false;
  }

  // End of RelationalOperator: '<S2>/Compare'
  // End of Outputs for SubSystem: '<Root>/Position Controller'

  // Outputs for Enabled SubSystem: '<Root>/Speed Controller' incorporates:
  //   EnablePort: '<S4>/Enable'

  // RelationalOperator: '<S1>/Compare' incorporates:
  //   Constant: '<Root>/controller_mode'
  //   Constant: '<S1>/Constant'

  if (controller_mode >= controller_modes::SPEED) {
    if (!rtDW.SpeedController_MODE) {
      // InitializeConditions for DiscreteIntegrator: '<S4>/Discrete-Time Integrator' 
      rtDW.DiscreteTimeIntegrator_DSTATE = 0.0F;

      // InitializeConditions for RateLimiter: '<S4>/Rate Limiter'
      rtDW.PrevY_i = 0.0F;
      rtDW.SpeedController_MODE = true;
    }

    // DiscreteIntegrator: '<S4>/Discrete-Time Integrator'
    rtDW.Ui = rtDW.DiscreteTimeIntegrator_DSTATE;

    // RateLimiter: '<S4>/Rate Limiter' incorporates:
    //   Outport: '<Root>/speedDemand'

    rtb_Sum1 = rtY.speedDemand - rtDW.PrevY_i;
    tmp = static_cast<real32_T>(currentControllerGains.speed.RateLimiterMax *
      period);
    if (rtb_Sum1 > tmp) {
      // RateLimiter: '<S4>/Rate Limiter'
      rtDW.RateLimiter = tmp + rtDW.PrevY_i;
    } else if (rtb_Sum1 < static_cast<real32_T>
               (currentControllerGains.speed.RateLimiterMin * period)) {
      // RateLimiter: '<S4>/Rate Limiter'
      rtDW.RateLimiter = static_cast<real32_T>
        (currentControllerGains.speed.RateLimiterMin * period) + rtDW.PrevY_i;
    } else {
      // RateLimiter: '<S4>/Rate Limiter'
      rtDW.RateLimiter = rtY.speedDemand;
    }

    rtDW.PrevY_i = rtDW.RateLimiter;

    // End of RateLimiter: '<S4>/Rate Limiter'

    // Sum: '<S4>/Sum' incorporates:
    //   Inport: '<Root>/speed_feedback'

    rtb_Sum_a = rtDW.RateLimiter - rtU.SpeedFeedback;

    // Gain: '<S4>/Gain'
    rtDW.Up = currentControllerGains.speed.Kp * rtb_Sum_a;

    // Sum: '<S4>/Sum1'
    rtb_Sum1 = rtDW.Up + rtDW.Ui;

    // Saturate: '<S4>/Saturation'
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

    // End of Saturate: '<S4>/Saturation'

    // Update for DiscreteIntegrator: '<S4>/Discrete-Time Integrator' incorporates:
    //   Gain: '<S4>/Gain1'
    //   Gain: '<S4>/Gain2'
    //   Outport: '<Root>/currentDemand'
    //   Sum: '<S4>/Sum2'
    //   Sum: '<S4>/Sum3'

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

    // End of Update for DiscreteIntegrator: '<S4>/Discrete-Time Integrator'
  } else if (rtDW.SpeedController_MODE) {
    // Disable for Outport: '<Root>/currentDemand' incorporates:
    //   Outport: '<S4>/Y'

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

  // Outport: '<Root>/pos_ref_rate_limited'
  rtY.pos_ref_rate_limited = rtb_RateLimiter;
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
