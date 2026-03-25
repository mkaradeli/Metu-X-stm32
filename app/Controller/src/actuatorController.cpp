//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: actuatorController.cpp
//
// Code generated for Simulink model 'actuatorController'.
//
// Model version                  : 1.43
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Sat Mar 21 17:05:46 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#include "actuatorController.h"
#include "rtwtypes.h"
#include <cmath>
#include "limits"
#include "cmath"

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
    0.0F
  }
} ;                                    // Variable: controllerGains
                                          //  Referenced by:
                                          //    '<S9>/Gain'
                                          //    '<S9>/Gain1'
                                          //    '<S9>/Saturation'
                                          //    '<S10>/Gain1'
                                          //    '<S11>/Discrete-Time Integrator'
                                          //    '<S11>/Gain'
                                          //    '<S11>/Gain1'
                                          //    '<S11>/Gain2'
                                          //    '<S11>/Rate Limiter'
                                          //    '<S11>/Saturation'


controller_modes controller_mode{ controller_modes::DISABLE };// Variable: controller_mode
                                                                 //  Referenced by: '<S1>/controller_mode'


extern "C"
{
  real_T rtNaN { -std::numeric_limits<real_T>::quiet_NaN() };

  real_T rtInf { std::numeric_limits<real_T>::infinity() };

  real_T rtMinusInf { -std::numeric_limits<real_T>::infinity() };

  real32_T rtNaNF { -std::numeric_limits<real32_T>::quiet_NaN() };

  real32_T rtInfF { std::numeric_limits<real32_T>::infinity() };

  real32_T rtMinusInfF { -std::numeric_limits<real32_T>::infinity() };
}

namespace controller
{
  // Model step function
  void actuatorController::step()
  {
    real32_T rtb_Product1;
    real32_T rtb_Sum1_py;
    real32_T rtb_Sum3_p;
    real32_T rtb_Sum_m;
    real32_T rtb_TSamp;
    boolean_T rtb_LogicalOperator;
    boolean_T rtb_LogicalOperator_f;
    boolean_T rtb_LogicalOperator_o;

    // Outputs for Enabled SubSystem: '<S1>/Position Controller' incorporates:
    //   EnablePort: '<S9>/Enable'

    // RelationalOperator: '<S3>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S3>/Constant'

    if (controller_mode >= controller_modes::POSITION) {
      if (!rtDW.PositionController_MODE) {
        // InitializeConditions for UnitDelay: '<S13>/UD'
        //
        //  Block description for '<S13>/UD':
        //
        //   Store in Global RAM

        rtDW.UD_DSTATE = 0.0F;
        rtDW.PositionController_MODE = true;
      }

      // SampleTimeMath: '<S13>/TSamp' incorporates:
      //   DiscreteIntegrator: '<S42>/Discrete-Time Integrator'
      //
      //  About '<S13>/TSamp':
      //   y = u * K where K = 1 / ( w * Ts )
      //
      rtb_TSamp = rtDW.DiscreteTimeIntegrator_DSTATE * 1000.0F;

      // Sum: '<S9>/Sum1' incorporates:
      //   DiscreteIntegrator: '<S42>/Discrete-Time Integrator'
      //   Gain: '<S9>/Gain'
      //   Gain: '<S9>/Gain1'
      //   Inport: '<Root>/pos_feedback'
      //   Sum: '<S13>/Diff'
      //   Sum: '<S9>/Sum'
      //   UnitDelay: '<S13>/UD'
      //
      //  Block description for '<S13>/Diff':
      //
      //   Add in CPU
      //
      //  Block description for '<S13>/UD':
      //
      //   Store in Global RAM

      rtb_Sum_m = (rtb_TSamp - rtDW.UD_DSTATE) *
        currentControllerGains.position.Kff +
        (rtDW.DiscreteTimeIntegrator_DSTATE - rtU.pos_feedback) *
        currentControllerGains.position.Kp;

      // Saturate: '<S9>/Saturation'
      if (rtb_Sum_m > currentControllerGains.position.SatMax) {
        // Saturate: '<S9>/Saturation'
        rtDW.Saturation_m = currentControllerGains.position.SatMax;
      } else if (rtb_Sum_m < currentControllerGains.position.SatMin) {
        // Saturate: '<S9>/Saturation'
        rtDW.Saturation_m = currentControllerGains.position.SatMin;
      } else {
        // Saturate: '<S9>/Saturation'
        rtDW.Saturation_m = rtb_Sum_m;
      }

      // End of Saturate: '<S9>/Saturation'

      // Update for UnitDelay: '<S13>/UD'
      //
      //  Block description for '<S13>/UD':
      //
      //   Store in Global RAM

      rtDW.UD_DSTATE = rtb_TSamp;
    } else if (rtDW.PositionController_MODE) {
      // Disable for Saturate: '<S9>/Saturation' incorporates:
      //   Outport: '<S9>/SpeedDemand'

      rtDW.Saturation_m = 0.0F;
      rtDW.PositionController_MODE = false;
    }

    // End of RelationalOperator: '<S3>/Compare'
    // End of Outputs for SubSystem: '<S1>/Position Controller'

    // Outputs for Enabled SubSystem: '<S1>/Pressure Controller' incorporates:
    //   EnablePort: '<S10>/Enable'

    // RelationalOperator: '<S4>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S4>/Constant'

    if (controller_mode >= controller_modes::PRESSURE) {
      if (!rtDW.PressureController_MODE) {
        // InitializeConditions for DiscreteIntegrator: '<S40>/Integrator'
        rtDW.Integrator_PrevResetState = 0;
        rtDW.Integrator_IC_LOADING = 1U;

        // InitializeConditions for DiscreteIntegrator: '<S26>/Integrator'
        rtDW.Integrator_PrevResetState_h = 0;
        rtDW.Integrator_IC_LOADING_m = 1U;

        // InitializeConditions for DiscreteIntegrator: '<S33>/Integrator'
        rtDW.Integrator_PrevResetState_c = 0;
        rtDW.Integrator_IC_LOADING_f = 1U;
        rtDW.PressureController_MODE = true;
      }

      // Logic: '<S34>/Logical Operator' incorporates:
      //   Constant: '<S34>/Time constant'
      //   Constant: '<S38>/Constant'
      //   RelationalOperator: '<S38>/Compare'
      //   Sum: '<S34>/Sum1'

      rtb_LogicalOperator = (0.005F - rtDW.Probe[0] <= 0.0F);

      // DiscreteIntegrator: '<S40>/Integrator' incorporates:
      //   Inport: '<Root>/P_manifold'

      if (rtDW.Integrator_IC_LOADING != 0) {
        rtDW.Integrator_DSTATE = rtU.P_manifold;
      }

      if (rtb_LogicalOperator || (rtDW.Integrator_PrevResetState != 0)) {
        rtDW.Integrator_DSTATE = rtU.P_manifold;
      }

      // Logic: '<S20>/Logical Operator' incorporates:
      //   Constant: '<S20>/Time constant'
      //   Constant: '<S24>/Constant'
      //   RelationalOperator: '<S24>/Compare'
      //   Sum: '<S20>/Sum1'

      rtb_LogicalOperator_o = (static_cast<real32_T>(0.001 - rtDW.Probe_h[0]) <=
        0.0F);

      // Saturate: '<S19>/Saturation' incorporates:
      //   Inport: '<Root>/pos_feedback'

      if (rtU.pos_feedback > 1800.0F) {
        rtb_Sum_m = 1800.0F;
      } else if (rtU.pos_feedback < 0.0F) {
        rtb_Sum_m = 0.0F;
      } else {
        rtb_Sum_m = rtU.pos_feedback;
      }

      // Gain: '<S19>/Gain' incorporates:
      //   Saturate: '<S19>/Saturation'

      rtb_TSamp = 0.00092592591F * rtb_Sum_m;

      // Math: '<S19>/Square'
      rtb_TSamp *= rtb_TSamp;

      // Product: '<S19>/Product1' incorporates:
      //   Bias: '<S19>/Bias'
      //   Bias: '<S19>/Bias1'
      //   DiscreteIntegrator: '<S40>/Integrator'
      //   Gain: '<S19>/Gain1'
      //   Gain: '<S19>/Gain2'
      //   Gain: '<S19>/Gain3'
      //   Product: '<S19>/Product'
      //   UnaryMinus: '<S19>/Unary Minus'

      rtb_Product1 = (-(52.8534F * rtb_TSamp / (51.8534F * rtb_TSamp + 1.0F) *
                        0.9197F) + 1.0F) * rtDW.Integrator_DSTATE;

      // DiscreteIntegrator: '<S26>/Integrator'
      if (rtDW.Integrator_IC_LOADING_m != 0) {
        rtDW.Integrator_DSTATE_i = rtb_Product1;
      }

      if (rtb_LogicalOperator_o || (rtDW.Integrator_PrevResetState_h != 0)) {
        rtDW.Integrator_DSTATE_i = rtb_Product1;
      }

      // Sum: '<S10>/Sum1' incorporates:
      //   DiscreteIntegrator: '<S26>/Integrator'
      //   DiscreteIntegrator: '<S40>/Integrator'
      //   Inport: '<Root>/P_nozzle'
      //   Sum: '<S10>/Sum5'

      rtb_Sum1_py = (rtDW.Integrator_DSTATE - rtDW.Integrator_DSTATE_i) -
        rtU.P_nozzle;

      // Logic: '<S27>/Logical Operator' incorporates:
      //   Constant: '<S27>/Time constant'
      //   Constant: '<S31>/Constant'
      //   RelationalOperator: '<S31>/Compare'
      //   Sum: '<S27>/Sum1'

      rtb_LogicalOperator_f = (static_cast<real32_T>(0.004 - rtDW.Probe_i[0]) <=
        0.0F);

      // DiscreteIntegrator: '<S33>/Integrator'
      if (rtDW.Integrator_IC_LOADING_f != 0) {
        rtDW.Integrator_DSTATE_d = rtb_Sum1_py;
        if (rtDW.Integrator_DSTATE_d > 250.0F) {
          rtDW.Integrator_DSTATE_d = 250.0F;
        } else if (rtDW.Integrator_DSTATE_d < -250.0F) {
          rtDW.Integrator_DSTATE_d = -250.0F;
        }
      }

      if (rtb_LogicalOperator_f || (rtDW.Integrator_PrevResetState_c != 0)) {
        rtDW.Integrator_DSTATE_d = rtb_Sum1_py;
        if (rtDW.Integrator_DSTATE_d > 250.0F) {
          rtDW.Integrator_DSTATE_d = 250.0F;
        } else if (rtDW.Integrator_DSTATE_d < -250.0F) {
          rtDW.Integrator_DSTATE_d = -250.0F;
        }
      }

      // Saturate: '<S10>/Saturation1' incorporates:
      //   Inport: '<Root>/P_nozzle_demand'

      if (rtU.P_nozzle_demand > 4000.0F) {
        rtb_TSamp = 4000.0F;
      } else if (rtU.P_nozzle_demand < 0.0F) {
        rtb_TSamp = 0.0F;
      } else {
        rtb_TSamp = rtU.P_nozzle_demand;
      }

      // End of Saturate: '<S10>/Saturation1'

      // Sum: '<S10>/Sum4' incorporates:
      //   DiscreteIntegrator: '<S33>/Integrator'

      rtb_Sum_m = rtb_TSamp + rtDW.Integrator_DSTATE_d;

      // Saturate: '<S10>/Saturation2'
      if (rtb_Sum_m > 4000.0F) {
        rtb_Sum_m = 4000.0F;
      } else if (rtb_Sum_m < 0.0F) {
        rtb_Sum_m = 0.0F;
      }

      // Sum: '<S10>/Sum3' incorporates:
      //   DiscreteIntegrator: '<S40>/Integrator'
      //   Gain: '<S10>/Gain2'
      //   MinMax: '<S10>/Min'
      //   Saturate: '<S10>/Saturation2'

      rtb_Sum3_p = rtDW.Integrator_DSTATE - std::fmin(0.93F *
        rtDW.Integrator_DSTATE, rtb_Sum_m);

      // Saturate: '<S18>/Saturation' incorporates:
      //   DiscreteIntegrator: '<S40>/Integrator'

      if (rtDW.Integrator_DSTATE > 5000.0F) {
        rtb_Sum_m = 5000.0F;
      } else if (rtDW.Integrator_DSTATE < 0.0F) {
        rtb_Sum_m = 0.0F;
      } else {
        rtb_Sum_m = rtDW.Integrator_DSTATE;
      }

      // End of Saturate: '<S18>/Saturation'

      // Switch: '<S41>/Switch2' incorporates:
      //   Constant: '<S18>/Constant'
      //   RelationalOperator: '<S41>/LowerRelop1'
      //   RelationalOperator: '<S41>/UpperRelop'
      //   Switch: '<S41>/Switch'

      if (rtb_Sum3_p > rtb_Sum_m) {
        rtb_Sum3_p = rtb_Sum_m;
      } else if (rtb_Sum3_p < 0.0F) {
        // Switch: '<S41>/Switch' incorporates:
        //   Constant: '<S18>/Constant'

        rtb_Sum3_p = 0.0F;
      }

      // End of Switch: '<S41>/Switch2'

      // Gain: '<S18>/Gain' incorporates:
      //   Bias: '<S18>/Bias'
      //   Product: '<S18>/Product'
      //   UnaryMinus: '<S18>/Unary Minus'

      rtb_Sum_m = (-(1.0F / rtb_Sum_m * rtb_Sum3_p) + 1.0F) * 1.08731103F;

      // Gain: '<S18>/Gain2' incorporates:
      //   Bias: '<S18>/Bias1'
      //   Constant: '<S18>/Constant1'
      //   Gain: '<S18>/Gain1'
      //   MinMax: '<S18>/Max'
      //   Product: '<S18>/Product1'
      //   Sqrt: '<S18>/Sqrt'

      rtb_Sum_m = std::sqrt(std::fmax(rtb_Sum_m / (-51.8534F * rtb_Sum_m +
        52.8534F), 0.0F)) * 1080.0F;

      // Saturate: '<S10>/Saturation'
      if (rtb_Sum_m > 1080.0F) {
        rtb_Sum_m = 1080.0F;
      }

      // Product: '<S10>/Product' incorporates:
      //   Constant: '<S14>/Constant'
      //   Inport: '<Root>/P_nozzle_demand'
      //   RelationalOperator: '<S14>/Compare'
      //   Saturate: '<S10>/Saturation'

      rtDW.Product = static_cast<real32_T>(rtU.P_nozzle_demand >= 5.0F) *
        rtb_Sum_m;

      // Gain: '<S10>/Gain1' incorporates:
      //   Inport: '<Root>/P_nozzle'
      //   Sum: '<S10>/Sum2'

      rtDW.Gain1 = (rtb_TSamp - rtU.P_nozzle) *
        currentControllerGains.pressure.Kp;

      // Update for DiscreteIntegrator: '<S40>/Integrator' incorporates:
      //   Constant: '<S34>/Time constant'
      //   Inport: '<Root>/P_manifold'
      //   MinMax: '<S34>/Max'
      //   Product: '<S17>/1//T'
      //   Sum: '<S17>/Sum1'

      rtDW.Integrator_IC_LOADING = 0U;
      rtDW.Integrator_DSTATE += 1.0F / std::fmax(rtDW.Probe[0], 0.005F) *
        (rtU.P_manifold - rtDW.Integrator_DSTATE) * 0.001F;
      rtDW.Integrator_PrevResetState = static_cast<int8_T>(rtb_LogicalOperator);

      // Update for DiscreteIntegrator: '<S26>/Integrator' incorporates:
      //   Constant: '<S20>/Time constant'
      //   MinMax: '<S20>/Max'
      //   Product: '<S15>/1//T'
      //   Sum: '<S15>/Sum1'

      rtDW.Integrator_IC_LOADING_m = 0U;
      rtDW.Integrator_DSTATE_i += 1.0F / static_cast<real32_T>(std::fmax(
        static_cast<real_T>(rtDW.Probe_h[0]), 0.001)) * (rtb_Product1 -
        rtDW.Integrator_DSTATE_i) * 0.001F;
      rtDW.Integrator_PrevResetState_h = static_cast<int8_T>
        (rtb_LogicalOperator_o);

      // Update for DiscreteIntegrator: '<S33>/Integrator' incorporates:
      //   Constant: '<S27>/Time constant'
      //   MinMax: '<S27>/Max'
      //   Product: '<S16>/1//T'
      //   Sum: '<S16>/Sum1'

      rtDW.Integrator_IC_LOADING_f = 0U;
      rtDW.Integrator_DSTATE_d += 1.0F / static_cast<real32_T>(std::fmax(
        static_cast<real_T>(rtDW.Probe_i[0]), 0.004)) * (rtb_Sum1_py -
        rtDW.Integrator_DSTATE_d) * 0.001F;
      if (rtDW.Integrator_DSTATE_d > 250.0F) {
        rtDW.Integrator_DSTATE_d = 250.0F;
      } else if (rtDW.Integrator_DSTATE_d < -250.0F) {
        rtDW.Integrator_DSTATE_d = -250.0F;
      }

      rtDW.Integrator_PrevResetState_c = static_cast<int8_T>
        (rtb_LogicalOperator_f);

      // End of Update for DiscreteIntegrator: '<S33>/Integrator'
    } else if (rtDW.PressureController_MODE) {
      // Disable for Product: '<S10>/Product' incorporates:
      //   Outport: '<S10>/theta_demand'

      rtDW.Product = 0.0F;

      // Disable for Gain: '<S10>/Gain1' incorporates:
      //   Outport: '<S10>/speed_demand'

      rtDW.Gain1 = 0.0F;
      rtDW.PressureController_MODE = false;
    }

    // End of RelationalOperator: '<S4>/Compare'
    // End of Outputs for SubSystem: '<S1>/Pressure Controller'

    // Switch: '<S1>/Switch' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S5>/Constant'
    //   Inport: '<Root>/speedDemand_ext'
    //   RelationalOperator: '<S5>/Compare'
    //   Sum: '<S1>/Sum'

    if (controller_mode == controller_modes::SPEED) {
      rtb_Sum1_py = rtU.speedDemand_ext;
    } else {
      rtb_Sum1_py = rtDW.Saturation_m + rtDW.Gain1;
    }

    // End of Switch: '<S1>/Switch'

    // Gain: '<S7>/Gain1'
    rtb_TSamp = 0.0174532924F * rtb_Sum1_py;

    // Outputs for Enabled SubSystem: '<S1>/Speed Controller' incorporates:
    //   EnablePort: '<S11>/Enable'

    // RelationalOperator: '<S2>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S2>/Constant'

    if (controller_mode >= controller_modes::SPEED) {
      if (!rtDW.SpeedController_MODE) {
        // InitializeConditions for DiscreteIntegrator: '<S11>/Discrete-Time Integrator' 
        rtDW.DiscreteTimeIntegrator_DSTATE_b = 0.0F;

        // InitializeConditions for RateLimiter: '<S11>/Rate Limiter'
        rtDW.PrevY = 0.0F;
        rtDW.SpeedController_MODE = true;
      }

      // RateLimiter: '<S11>/Rate Limiter'
      rtb_Product1 = rtb_TSamp - rtDW.PrevY;
      rtb_Sum_m = static_cast<real32_T>
        (currentControllerGains.speed.RateLimiterMax * period);
      if (rtb_Product1 > rtb_Sum_m) {
        rtb_Sum_m += rtDW.PrevY;
      } else if (rtb_Product1 < static_cast<real32_T>
                 (currentControllerGains.speed.RateLimiterMin * period)) {
        rtb_Sum_m = static_cast<real32_T>
          (currentControllerGains.speed.RateLimiterMin * period) + rtDW.PrevY;
      } else {
        rtb_Sum_m = rtb_TSamp;
      }

      rtDW.PrevY = rtb_Sum_m;

      // End of RateLimiter: '<S11>/Rate Limiter'

      // Sum: '<S11>/Sum' incorporates:
      //   Gain: '<S8>/Gain1'
      //   Inport: '<Root>/speed_feedback'

      rtb_Sum_m -= 0.0174532924F * rtU.SpeedFeedback;

      // Sum: '<S11>/Sum1' incorporates:
      //   DiscreteIntegrator: '<S11>/Discrete-Time Integrator'
      //   Gain: '<S11>/Gain'

      rtb_TSamp = currentControllerGains.speed.Kp * rtb_Sum_m +
        rtDW.DiscreteTimeIntegrator_DSTATE_b;

      // Saturate: '<S11>/Saturation'
      if (rtb_TSamp > currentControllerGains.speed.SatMax) {
        // Outport: '<Root>/currentDemand'
        rtY.currentDemand = currentControllerGains.speed.SatMax;
      } else if (rtb_TSamp < currentControllerGains.speed.SatMin) {
        // Outport: '<Root>/currentDemand'
        rtY.currentDemand = currentControllerGains.speed.SatMin;
      } else {
        // Outport: '<Root>/currentDemand'
        rtY.currentDemand = rtb_TSamp;
      }

      // End of Saturate: '<S11>/Saturation'

      // Update for DiscreteIntegrator: '<S11>/Discrete-Time Integrator' incorporates:
      //   Gain: '<S11>/Gain1'
      //   Gain: '<S11>/Gain2'
      //   Outport: '<Root>/currentDemand'
      //   Sum: '<S11>/Sum2'
      //   Sum: '<S11>/Sum3'

      rtDW.DiscreteTimeIntegrator_DSTATE_b += (currentControllerGains.speed.Ki /
        currentControllerGains.speed.Kp * (rtY.currentDemand - rtb_TSamp) +
        currentControllerGains.speed.Ki * rtb_Sum_m) * 0.001F;
      if (rtDW.DiscreteTimeIntegrator_DSTATE_b >
          currentControllerGains.speed.SatMax) {
        rtDW.DiscreteTimeIntegrator_DSTATE_b =
          currentControllerGains.speed.SatMax;
      } else if (rtDW.DiscreteTimeIntegrator_DSTATE_b <
                 currentControllerGains.speed.SatMin) {
        rtDW.DiscreteTimeIntegrator_DSTATE_b =
          currentControllerGains.speed.SatMin;
      }

      // End of Update for DiscreteIntegrator: '<S11>/Discrete-Time Integrator'
    } else if (rtDW.SpeedController_MODE) {
      // Disable for Outport: '<Root>/currentDemand' incorporates:
      //   Outport: '<S11>/Y'

      rtY.currentDemand = 0.0F;
      rtDW.SpeedController_MODE = false;
    }

    // End of RelationalOperator: '<S2>/Compare'
    // End of Outputs for SubSystem: '<S1>/Speed Controller'

    // Outport: '<Root>/speedDemand'
    rtY.speedDemand = rtb_Sum1_py;

    // Switch: '<S1>/Switch1' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S6>/Constant'
    //   Inport: '<Root>/pos_ref_ext'
    //   RelationalOperator: '<S6>/Compare'

    if (controller_mode == controller_modes::POSITION) {
      rtb_Sum1_py = rtU.pos_ref_ext;
    } else {
      rtb_Sum1_py = rtDW.Product;
    }

    // End of Switch: '<S1>/Switch1'

    // Outport: '<Root>/position_demand'
    rtY.position_demand = rtb_Sum1_py;

    // Outport: '<Root>/pos_ref_rate_limited' incorporates:
    //   DiscreteIntegrator: '<S42>/Discrete-Time Integrator'

    rtY.pos_ref_rate_limited = rtDW.DiscreteTimeIntegrator_DSTATE;

    // Saturate: '<S12>/Saturation1'
    if (rtb_Sum1_py > 1440.0F) {
      rtb_Sum1_py = 1440.0F;
    } else if (rtb_Sum1_py < 0.0F) {
      rtb_Sum1_py = 0.0F;
    }

    // Gain: '<S42>/Gain' incorporates:
    //   DiscreteIntegrator: '<S42>/Discrete-Time Integrator'
    //   Saturate: '<S12>/Saturation1'
    //   Sum: '<S42>/Sum1'

    rtb_Sum_m = (rtb_Sum1_py - rtDW.DiscreteTimeIntegrator_DSTATE) * 188.49556F;

    // Saturate: '<S42>/Saturation'
    if (rtb_Sum_m > 2400.0F) {
      rtb_Sum_m = 2400.0F;
    } else if (rtb_Sum_m < -2400.0F) {
      rtb_Sum_m = -2400.0F;
    }

    // Update for DiscreteIntegrator: '<S42>/Discrete-Time Integrator' incorporates:
    //   Saturate: '<S42>/Saturation'

    rtDW.DiscreteTimeIntegrator_DSTATE += 0.001F * rtb_Sum_m;
  }

  // Model initialize function
  void actuatorController::initialize()
  {
    // SystemInitialize for Enabled SubSystem: '<S1>/Pressure Controller'
    // Start for Probe: '<S34>/Probe'
    rtDW.Probe[0] = 0.001F;
    rtDW.Probe[1] = 0.0F;

    // Start for Probe: '<S20>/Probe'
    rtDW.Probe_h[0] = 0.001F;
    rtDW.Probe_h[1] = 0.0F;

    // Start for Probe: '<S27>/Probe'
    rtDW.Probe_i[0] = 0.001F;
    rtDW.Probe_i[1] = 0.0F;

    // InitializeConditions for DiscreteIntegrator: '<S40>/Integrator'
    rtDW.Integrator_IC_LOADING = 1U;

    // InitializeConditions for DiscreteIntegrator: '<S26>/Integrator'
    rtDW.Integrator_IC_LOADING_m = 1U;

    // InitializeConditions for DiscreteIntegrator: '<S33>/Integrator'
    rtDW.Integrator_IC_LOADING_f = 1U;

    // End of SystemInitialize for SubSystem: '<S1>/Pressure Controller'
  }

  const char_T* actuatorController::RT_MODEL::getErrorStatus() const
  {
    return (errorStatus);
  }

  void actuatorController::RT_MODEL::setErrorStatus(const char_T* const volatile
    aErrorStatus)
  {
    (errorStatus = aErrorStatus);
  }

  // Constructor
  actuatorController::actuatorController() :
    rtU(),
    rtY(),
    rtDW(),
    rtM()
  {
    // Currently there is no constructor body generated.
  }

  // Destructor
  // Currently there is no destructor body generated.
  actuatorController::~actuatorController() = default;

  // Real-Time Model get method
  actuatorController::RT_MODEL * actuatorController::getRTM()
  {
    return (&rtM);
  }
}

extern "C"
{
  // Test if value is infinite
  static boolean_T rtIsInf(real_T value)
  {
    return std::isinf(value);
  }

  // Test if single-precision value is infinite
  static boolean_T rtIsInfF(real32_T value)
  {
    return std::isinf(value);
  }

  // Test if value is not a number
  static boolean_T rtIsNaN(real_T value)
  {
    return std::isnan(value);
  }

  // Test if single-precision value is not a number
  static boolean_T rtIsNaNF(real32_T value)
  {
    return std::isnan(value);
  }
}

//
// File trailer for generated code.
//
// [EOF]
//
