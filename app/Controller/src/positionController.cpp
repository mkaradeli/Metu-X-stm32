//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: positionController.cpp
//
// Code generated for Simulink model 'positionController'.
//
// Model version                  : 1.33
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Tue Feb 24 22:38:00 2026
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
struct_In9luHMHiNdKBncTZV4w1E currentControllerGains{
  {
    0.00784151815F,
    712.09436F,
    1.0F,
    -1.0F,
    0.000125
  },

  {
    0.624023795F,
    2.77958274F,
    15.0F,
    -15.0F,
    2.0e+15F,
    -2.0e+15F,
    0.001
  },

  {
    62.831852F,
    0.8F,
    2400.0F,
    -2400.0F,
    2400.0F,
    -2400.0F,
    0.001
  },

  {
    0.5F
  }
} ;                                    // Variable: controllerGains
                                          //  Referenced by:
                                          //    '<S6>/Gain'
                                          //    '<S6>/Gain1'
                                          //    '<S6>/Saturation'
                                          //    '<S7>/Discrete-Time Integrator'
                                          //    '<S7>/Gain'
                                          //    '<S7>/Gain1'
                                          //    '<S7>/Gain2'
                                          //    '<S7>/Rate Limiter'
                                          //    '<S7>/Saturation'


controller_modes controller_mode{ controller_modes::DISABLE };// Variable: controller_mode
                                                                 //  Referenced by: '<Root>/controller_mode'


namespace controller
{
  // Model step function
  void position::step()
  {
    real32_T rtb_Sum1_k;
    real32_T rtb_TSamp;
    real32_T u0;

    // Outputs for Enabled SubSystem: '<Root>/Position Controller' incorporates:
    //   EnablePort: '<S6>/Enable'

    // Switch: '<Root>/Switch' incorporates:
    //   Constant: '<Root>/controller_mode'
    //   Constant: '<S2>/Constant'
    //   Inport: '<Root>/speedDemandExt'
    //   Outport: '<Root>/speedDemand'
    //   RelationalOperator: '<S2>/Compare'

    if (controller_mode >= controller_modes::POSITION) {
      if (!rtDW.PositionController_MODE) {
        // InitializeConditions for UnitDelay: '<S9>/UD'
        //
        //  Block description for '<S9>/UD':
        //
        //   Store in Global RAM

        rtDW.UD_DSTATE = 0.0F;
        rtDW.PositionController_MODE = true;
      }

      // SampleTimeMath: '<S9>/TSamp' incorporates:
      //   DiscreteIntegrator: '<S10>/Discrete-Time Integrator'
      //
      //  About '<S9>/TSamp':
      //   y = u * K where K = 1 / ( w * Ts )
      //
      rtb_TSamp = rtDW.DiscreteTimeIntegrator_DSTATE * 1000.0F;

      // Sum: '<S6>/Sum1' incorporates:
      //   DiscreteIntegrator: '<S10>/Discrete-Time Integrator'
      //   Gain: '<S6>/Gain'
      //   Gain: '<S6>/Gain1'
      //   Inport: '<Root>/pos_feedback'
      //   Sum: '<S6>/Sum'
      //   Sum: '<S9>/Diff'
      //   UnitDelay: '<S9>/UD'
      //
      //  Block description for '<S9>/Diff':
      //
      //   Add in CPU
      //
      //  Block description for '<S9>/UD':
      //
      //   Store in Global RAM

      u0 = (rtb_TSamp - rtDW.UD_DSTATE) * currentControllerGains.position.Kff +
        (rtDW.DiscreteTimeIntegrator_DSTATE - rtU.pos_feedback) *
        currentControllerGains.position.Kp;

      // Saturate: '<S6>/Saturation'
      if (u0 > currentControllerGains.position.SatMax) {
        // Outport: '<Root>/speedDemand'
        rtY.speedDemand = currentControllerGains.position.SatMax;
      } else if (u0 < currentControllerGains.position.SatMin) {
        // Outport: '<Root>/speedDemand'
        rtY.speedDemand = currentControllerGains.position.SatMin;
      } else {
        // Outport: '<Root>/speedDemand'
        rtY.speedDemand = u0;
      }

      // End of Saturate: '<S6>/Saturation'

      // Update for UnitDelay: '<S9>/UD'
      //
      //  Block description for '<S9>/UD':
      //
      //   Store in Global RAM

      rtDW.UD_DSTATE = rtb_TSamp;
      u0 = rtY.speedDemand;
    } else {
      if (rtDW.PositionController_MODE) {
        // Disable for Outport: '<Root>/speedDemand' incorporates:
        //   Outport: '<S6>/SpeedDemand'

        rtY.speedDemand = 0.0F;
        rtDW.PositionController_MODE = false;
      }

      u0 = rtU.speedDemandExt;
    }

    // End of Outputs for SubSystem: '<Root>/Position Controller'

    // Gain: '<S4>/Gain1' incorporates:
    //   Switch: '<Root>/Switch'

    rtb_TSamp = 0.0174532924F * u0;

    // Outputs for Enabled SubSystem: '<Root>/Speed Controller' incorporates:
    //   EnablePort: '<S7>/Enable'

    // RelationalOperator: '<S1>/Compare' incorporates:
    //   Constant: '<Root>/controller_mode'
    //   Constant: '<S1>/Constant'

    if (controller_mode >= controller_modes::SPEED) {
      if (!rtDW.SpeedController_MODE) {
        // InitializeConditions for DiscreteIntegrator: '<S7>/Discrete-Time Integrator' 
        rtDW.DiscreteTimeIntegrator_DSTATE_b = 0.0F;

        // InitializeConditions for RateLimiter: '<S7>/Rate Limiter'
        rtDW.PrevY = 0.0F;
        rtDW.SpeedController_MODE = true;
      }

      // DiscreteIntegrator: '<S7>/Discrete-Time Integrator'
      rtDW.Ui = rtDW.DiscreteTimeIntegrator_DSTATE_b;

      // RateLimiter: '<S7>/Rate Limiter'
      rtb_Sum1_k = rtb_TSamp - rtDW.PrevY;
      u0 = static_cast<real32_T>(currentControllerGains.speed.RateLimiterMax *
        period);
      if (rtb_Sum1_k > u0) {
        // RateLimiter: '<S7>/Rate Limiter'
        rtDW.RateLimiter = u0 + rtDW.PrevY;
      } else if (rtb_Sum1_k < static_cast<real32_T>
                 (currentControllerGains.speed.RateLimiterMin * period)) {
        // RateLimiter: '<S7>/Rate Limiter'
        rtDW.RateLimiter = static_cast<real32_T>
          (currentControllerGains.speed.RateLimiterMin * period) + rtDW.PrevY;
      } else {
        // RateLimiter: '<S7>/Rate Limiter'
        rtDW.RateLimiter = rtb_TSamp;
      }

      rtDW.PrevY = rtDW.RateLimiter;

      // End of RateLimiter: '<S7>/Rate Limiter'

      // Sum: '<S7>/Sum' incorporates:
      //   Gain: '<S5>/Gain1'
      //   Inport: '<Root>/speed_feedback'

      rtb_TSamp = rtDW.RateLimiter - 0.0174532924F * rtU.SpeedFeedback;

      // Gain: '<S7>/Gain'
      rtDW.Up = currentControllerGains.speed.Kp * rtb_TSamp;

      // Sum: '<S7>/Sum1'
      rtb_Sum1_k = rtDW.Up + rtDW.Ui;

      // Saturate: '<S7>/Saturation'
      if (rtb_Sum1_k > currentControllerGains.speed.SatMax) {
        // Outport: '<Root>/currentDemand'
        rtY.currentDemand = currentControllerGains.speed.SatMax;
      } else if (rtb_Sum1_k < currentControllerGains.speed.SatMin) {
        // Outport: '<Root>/currentDemand'
        rtY.currentDemand = currentControllerGains.speed.SatMin;
      } else {
        // Outport: '<Root>/currentDemand'
        rtY.currentDemand = rtb_Sum1_k;
      }

      // End of Saturate: '<S7>/Saturation'

      // Update for DiscreteIntegrator: '<S7>/Discrete-Time Integrator' incorporates:
      //   Gain: '<S7>/Gain1'
      //   Gain: '<S7>/Gain2'
      //   Outport: '<Root>/currentDemand'
      //   Sum: '<S7>/Sum2'
      //   Sum: '<S7>/Sum3'

      rtDW.DiscreteTimeIntegrator_DSTATE_b += (currentControllerGains.speed.Ki /
        currentControllerGains.speed.Kp * (rtY.currentDemand - rtb_Sum1_k) +
        currentControllerGains.speed.Ki * rtb_TSamp) * 0.001F;
      if (rtDW.DiscreteTimeIntegrator_DSTATE_b >
          currentControllerGains.speed.SatMax) {
        rtDW.DiscreteTimeIntegrator_DSTATE_b =
          currentControllerGains.speed.SatMax;
      } else if (rtDW.DiscreteTimeIntegrator_DSTATE_b <
                 currentControllerGains.speed.SatMin) {
        rtDW.DiscreteTimeIntegrator_DSTATE_b =
          currentControllerGains.speed.SatMin;
      }

      // End of Update for DiscreteIntegrator: '<S7>/Discrete-Time Integrator'
    } else if (rtDW.SpeedController_MODE) {
      // Disable for Outport: '<Root>/currentDemand' incorporates:
      //   Outport: '<S7>/Y'

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

    // Outport: '<Root>/pos_ref_rate_limited' incorporates:
    //   DiscreteIntegrator: '<S10>/Discrete-Time Integrator'

    rtY.pos_ref_rate_limited = rtDW.DiscreteTimeIntegrator_DSTATE;

    // Saturate: '<S8>/Saturation1' incorporates:
    //   Inport: '<Root>/pos_ref'

    if (rtU.pos_ref > 1440.0F) {
      u0 = 1440.0F;
    } else if (rtU.pos_ref < 0.0F) {
      u0 = 0.0F;
    } else {
      u0 = rtU.pos_ref;
    }

    // Gain: '<S10>/Gain' incorporates:
    //   DiscreteIntegrator: '<S10>/Discrete-Time Integrator'
    //   Saturate: '<S8>/Saturation1'
    //   Sum: '<S10>/Sum1'

    u0 = (u0 - rtDW.DiscreteTimeIntegrator_DSTATE) * 188.49556F;

    // Saturate: '<S10>/Saturation'
    if (u0 > 2400.0F) {
      u0 = 2400.0F;
    } else if (u0 < -2400.0F) {
      u0 = -2400.0F;
    }

    // Update for DiscreteIntegrator: '<S10>/Discrete-Time Integrator' incorporates:
    //   Saturate: '<S10>/Saturation'

    rtDW.DiscreteTimeIntegrator_DSTATE += 0.001F * u0;
  }

  // Model initialize function
  void position::initialize()
  {
    // (no initialization code required)
  }

  // Constructor
  position::position():
    rtU(),
    rtY(),
    rtDW()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  // Currently there is no destructor body generated.
  position::~position() = default;
}

//
// File trailer for generated code.
//
// [EOF]
//
