//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: actuatorController.cpp
//
// Code generated for Simulink model 'actuatorController'.
//
// Model version                  : 1.62
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Mon Aug 17 00:36:39 2026
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
    -5.0F,
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
                                          //    '<S11>/Gain'
                                          //    '<S11>/Gain1'
                                          //    '<S11>/Saturation'
                                          //    '<S12>/Discrete-Time Integrator'
                                          //    '<S12>/Gain'
                                          //    '<S12>/Gain1'
                                          //    '<S12>/Gain2'
                                          //    '<S12>/Rate Limiter'
                                          //    '<S12>/Saturation'


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
    real32_T rtb_Gain1;
    real32_T rtb_Product1_k;
    real32_T rtb_Sum1_g;
    real32_T rtb_Sum1_k;
    real32_T rtb_TSamp;
    real32_T u0;
    boolean_T rtb_LogicalOperator;
    boolean_T rtb_LogicalOperator_c;
    boolean_T rtb_LogicalOperator_k;

    // Outputs for Enabled SubSystem: '<S1>/Position Controller' incorporates:
    //   EnablePort: '<S11>/Enable'

    // RelationalOperator: '<S3>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S3>/Constant'

    if (controller_mode >= controller_modes::POSITION) {
      if (!rtDW.PositionController_MODE) {
        // InitializeConditions for UnitDelay: '<S16>/UD'
        //
        //  Block description for '<S16>/UD':
        //
        //   Store in Global RAM

        rtDW.UD_DSTATE = 0.0F;
        rtDW.PositionController_MODE = true;
      }

      // SampleTimeMath: '<S16>/TSamp' incorporates:
      //   DiscreteIntegrator: '<S45>/Discrete-Time Integrator'
      //
      //  About '<S16>/TSamp':
      //   y = u * K where K = 1 / ( w * Ts )
      //
      rtb_TSamp = rtDW.DiscreteTimeIntegrator_DSTATE * 1000.0F;

      // Sum: '<S11>/Sum1' incorporates:
      //   DiscreteIntegrator: '<S45>/Discrete-Time Integrator'
      //   Gain: '<S11>/Gain'
      //   Gain: '<S11>/Gain1'
      //   Inport: '<Root>/pos_feedback'
      //   Sum: '<S11>/Sum'
      //   Sum: '<S16>/Diff'
      //   UnitDelay: '<S16>/UD'
      //
      //  Block description for '<S16>/Diff':
      //
      //   Add in CPU
      //
      //  Block description for '<S16>/UD':
      //
      //   Store in Global RAM

      u0 = (rtb_TSamp - rtDW.UD_DSTATE) * currentControllerGains.position.Kff +
        (rtDW.DiscreteTimeIntegrator_DSTATE - rtU.pos_feedback) *
        currentControllerGains.position.Kp;

      // Saturate: '<S11>/Saturation'
      if (u0 > currentControllerGains.position.SatMax) {
        // Saturate: '<S11>/Saturation'
        rtDW.Saturation_m = currentControllerGains.position.SatMax;
      } else if (u0 < currentControllerGains.position.SatMin) {
        // Saturate: '<S11>/Saturation'
        rtDW.Saturation_m = currentControllerGains.position.SatMin;
      } else {
        // Saturate: '<S11>/Saturation'
        rtDW.Saturation_m = u0;
      }

      // End of Saturate: '<S11>/Saturation'

      // Update for UnitDelay: '<S16>/UD'
      //
      //  Block description for '<S16>/UD':
      //
      //   Store in Global RAM

      rtDW.UD_DSTATE = rtb_TSamp;
    } else if (rtDW.PositionController_MODE) {
      // Disable for Saturate: '<S11>/Saturation' incorporates:
      //   Outport: '<S11>/SpeedDemand'

      rtDW.Saturation_m = 0.0F;
      rtDW.PositionController_MODE = false;
    }

    // End of RelationalOperator: '<S3>/Compare'
    // End of Outputs for SubSystem: '<S1>/Position Controller'

    // Switch: '<S1>/Switch' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S5>/Constant'
    //   Inport: '<Root>/speedDemand_ext'
    //   RelationalOperator: '<S5>/Compare'
    //   Sum: '<S1>/Sum'

    if (controller_mode == controller_modes::SPEED) {
      rtb_TSamp = rtU.speedDemand_ext;
    } else {
      rtb_TSamp = rtDW.Saturation_m;
    }

    // End of Switch: '<S1>/Switch'

    // Gain: '<S9>/Gain1'
    rtb_Gain1 = 0.0174532924F * rtb_TSamp;

    // Outputs for Enabled SubSystem: '<S1>/Speed Controller' incorporates:
    //   EnablePort: '<S12>/Enable'

    // RelationalOperator: '<S2>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S2>/Constant'

    if (controller_mode >= controller_modes::SPEED) {
      if (!rtDW.SpeedController_MODE) {
        // InitializeConditions for DiscreteIntegrator: '<S12>/Discrete-Time Integrator' 
        rtDW.DiscreteTimeIntegrator_DSTATE_b = 0.0F;

        // InitializeConditions for RateLimiter: '<S12>/Rate Limiter'
        rtDW.PrevY = 0.0F;
        rtDW.SpeedController_MODE = true;
      }

      // RateLimiter: '<S12>/Rate Limiter'
      rtb_Sum1_k = rtb_Gain1 - rtDW.PrevY;
      u0 = static_cast<real32_T>(currentControllerGains.speed.RateLimiterMax *
        period);
      if (rtb_Sum1_k > u0) {
        rtb_Gain1 = u0 + rtDW.PrevY;
      } else if (rtb_Sum1_k < static_cast<real32_T>
                 (currentControllerGains.speed.RateLimiterMin * period)) {
        rtb_Gain1 = static_cast<real32_T>
          (currentControllerGains.speed.RateLimiterMin * period) + rtDW.PrevY;
      }

      rtDW.PrevY = rtb_Gain1;

      // End of RateLimiter: '<S12>/Rate Limiter'

      // Sum: '<S12>/Sum' incorporates:
      //   Gain: '<S10>/Gain1'
      //   Inport: '<Root>/speed_feedback'

      rtb_Gain1 -= 0.0174532924F * rtU.SpeedFeedback;

      // Sum: '<S12>/Sum1' incorporates:
      //   DiscreteIntegrator: '<S12>/Discrete-Time Integrator'
      //   Gain: '<S12>/Gain'

      rtb_Sum1_k = currentControllerGains.speed.Kp * rtb_Gain1 +
        rtDW.DiscreteTimeIntegrator_DSTATE_b;

      // Saturate: '<S12>/Saturation'
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

      // End of Saturate: '<S12>/Saturation'

      // Update for DiscreteIntegrator: '<S12>/Discrete-Time Integrator' incorporates:
      //   Gain: '<S12>/Gain1'
      //   Gain: '<S12>/Gain2'
      //   Outport: '<Root>/currentDemand'
      //   Sum: '<S12>/Sum2'
      //   Sum: '<S12>/Sum3'

      rtDW.DiscreteTimeIntegrator_DSTATE_b += (currentControllerGains.speed.Ki /
        currentControllerGains.speed.Kp * (rtY.currentDemand - rtb_Sum1_k) +
        currentControllerGains.speed.Ki * rtb_Gain1) * 0.001F;
      if (rtDW.DiscreteTimeIntegrator_DSTATE_b >
          currentControllerGains.speed.SatMax) {
        rtDW.DiscreteTimeIntegrator_DSTATE_b =
          currentControllerGains.speed.SatMax;
      } else if (rtDW.DiscreteTimeIntegrator_DSTATE_b <
                 currentControllerGains.speed.SatMin) {
        rtDW.DiscreteTimeIntegrator_DSTATE_b =
          currentControllerGains.speed.SatMin;
      }

      // End of Update for DiscreteIntegrator: '<S12>/Discrete-Time Integrator'
    } else if (rtDW.SpeedController_MODE) {
      // Disable for Outport: '<Root>/currentDemand' incorporates:
      //   Outport: '<S12>/Y'

      rtY.currentDemand = 0.0F;
      rtDW.SpeedController_MODE = false;
    }

    // End of RelationalOperator: '<S2>/Compare'
    // End of Outputs for SubSystem: '<S1>/Speed Controller'

    // Outport: '<Root>/speedDemand'
    rtY.speedDemand = rtb_TSamp;

    // Outport: '<Root>/pos_ref_rate_limited' incorporates:
    //   DiscreteIntegrator: '<S45>/Discrete-Time Integrator'

    rtY.pos_ref_rate_limited = rtDW.DiscreteTimeIntegrator_DSTATE;

    // Outputs for Enabled SubSystem: '<S1>/Thrust Controller' incorporates:
    //   EnablePort: '<S14>/Enable'

    // RelationalOperator: '<S7>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S7>/Constant'

    if (controller_mode >= controller_modes::FORCE) {
      // Saturate: '<S14>/Saturation1' incorporates:
      //   Inport: '<Root>/F_demand'

      if (rtU.F_demand <= 0.0F) {
        u0 = 0.0F;
      } else {
        u0 = rtU.F_demand;
      }

      // Saturate: '<S14>/Saturation' incorporates:
      //   Inport: '<Root>/nozzle_gain'

      if (rtU.nozzle_gain <= 0.0F) {
        rtb_TSamp = 0.0F;
      } else {
        rtb_TSamp = rtU.nozzle_gain;
      }

      // Outport: '<Root>/P_nozzle_demand1' incorporates:
      //   Product: '<S14>/Product'
      //   Saturate: '<S14>/Saturation'
      //   Saturate: '<S14>/Saturation1'

      rtY.P_nozzle_demand1 = u0 * rtb_TSamp;
    }

    // End of RelationalOperator: '<S7>/Compare'
    // End of Outputs for SubSystem: '<S1>/Thrust Controller'

    // Switch: '<S1>/Switch2' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S8>/Constant'
    //   Inport: '<Root>/P_nozzle_demand'
    //   Outport: '<Root>/P_nozzle_demand1'
    //   RelationalOperator: '<S8>/Compare'

    if (controller_mode == controller_modes::PRESSURE) {
      rtb_Sum1_k = rtU.P_nozzle_demand;
    } else {
      rtb_Sum1_k = rtY.P_nozzle_demand1;
    }

    // End of Switch: '<S1>/Switch2'

    // Outputs for Enabled SubSystem: '<S1>/Subsystem' incorporates:
    //   EnablePort: '<S13>/Enable'

    // RelationalOperator: '<S4>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S4>/Constant'

    if (controller_mode >= controller_modes::PRESSURE) {
      if (!rtDW.Subsystem_MODE) {
        // InitializeConditions for DiscreteIntegrator: '<S43>/Integrator'
        rtDW.Integrator_PrevResetState = 0;
        rtDW.Integrator_IC_LOADING = 1U;

        // InitializeConditions for DiscreteIntegrator: '<S29>/Integrator'
        rtDW.Integrator_PrevResetState_o = 0;
        rtDW.Integrator_IC_LOADING_b = 1U;

        // InitializeConditions for DiscreteIntegrator: '<S36>/Integrator'
        rtDW.Integrator_PrevResetState_h = 0;
        rtDW.Integrator_IC_LOADING_bm = 1U;
        rtDW.Subsystem_MODE = true;
      }

      // Logic: '<S37>/Logical Operator' incorporates:
      //   Constant: '<S37>/Time constant'
      //   Constant: '<S41>/Constant'
      //   RelationalOperator: '<S41>/Compare'
      //   Sum: '<S37>/Sum1'

      rtb_LogicalOperator = (0.005F - rtDW.Probe[0] <= 0.0F);

      // DiscreteIntegrator: '<S43>/Integrator' incorporates:
      //   Inport: '<Root>/P_manifold'

      if (rtDW.Integrator_IC_LOADING != 0) {
        rtDW.Integrator_DSTATE = rtU.P_manifold;
      }

      if (rtb_LogicalOperator || (rtDW.Integrator_PrevResetState != 0)) {
        rtDW.Integrator_DSTATE = rtU.P_manifold;
      }

      // Logic: '<S23>/Logical Operator' incorporates:
      //   Constant: '<S23>/Time constant'
      //   Constant: '<S27>/Constant'
      //   RelationalOperator: '<S27>/Compare'
      //   Sum: '<S23>/Sum1'

      rtb_LogicalOperator_c = (static_cast<real32_T>(0.001 - rtDW.Probe_e[0]) <=
        0.0F);

      // Saturate: '<S22>/Saturation' incorporates:
      //   Inport: '<Root>/pos_feedback'

      if (rtU.pos_feedback > 1800.0F) {
        u0 = 1800.0F;
      } else if (rtU.pos_feedback < 0.0F) {
        u0 = 0.0F;
      } else {
        u0 = rtU.pos_feedback;
      }

      // Gain: '<S22>/Gain' incorporates:
      //   Saturate: '<S22>/Saturation'

      rtb_Gain1 = 0.00092592591F * u0;

      // Math: '<S22>/Square'
      rtb_Gain1 *= rtb_Gain1;

      // Product: '<S22>/Product1' incorporates:
      //   Bias: '<S22>/Bias'
      //   Bias: '<S22>/Bias1'
      //   DiscreteIntegrator: '<S43>/Integrator'
      //   Gain: '<S22>/Gain1'
      //   Gain: '<S22>/Gain2'
      //   Gain: '<S22>/Gain3'
      //   Product: '<S22>/Product'
      //   UnaryMinus: '<S22>/Unary Minus'

      rtb_Product1_k = (-(52.8534F * rtb_Gain1 / (51.8534F * rtb_Gain1 + 1.0F) *
                          0.9197F) + 1.0F) * rtDW.Integrator_DSTATE;

      // DiscreteIntegrator: '<S29>/Integrator'
      if (rtDW.Integrator_IC_LOADING_b != 0) {
        rtDW.Integrator_DSTATE_l = rtb_Product1_k;
      }

      if (rtb_LogicalOperator_c || (rtDW.Integrator_PrevResetState_o != 0)) {
        rtDW.Integrator_DSTATE_l = rtb_Product1_k;
      }

      // Sum: '<S13>/Sum1' incorporates:
      //   DiscreteIntegrator: '<S29>/Integrator'
      //   DiscreteIntegrator: '<S43>/Integrator'
      //   Inport: '<Root>/P_nozzle'
      //   Sum: '<S13>/Sum5'

      rtb_Sum1_g = (rtDW.Integrator_DSTATE - rtDW.Integrator_DSTATE_l) -
        rtU.P_nozzle;

      // Logic: '<S30>/Logical Operator' incorporates:
      //   Constant: '<S30>/Time constant'
      //   Constant: '<S34>/Constant'
      //   RelationalOperator: '<S34>/Compare'
      //   Sum: '<S30>/Sum1'

      rtb_LogicalOperator_k = (static_cast<real32_T>(0.004 - rtDW.Probe_k[0]) <=
        0.0F);

      // DiscreteIntegrator: '<S36>/Integrator'
      if (rtDW.Integrator_IC_LOADING_bm != 0) {
        rtDW.Integrator_DSTATE_k = rtb_Sum1_g;
        if (rtDW.Integrator_DSTATE_k > 250.0F) {
          rtDW.Integrator_DSTATE_k = 250.0F;
        } else if (rtDW.Integrator_DSTATE_k < -250.0F) {
          rtDW.Integrator_DSTATE_k = -250.0F;
        }
      }

      if (rtb_LogicalOperator_k || (rtDW.Integrator_PrevResetState_h != 0)) {
        rtDW.Integrator_DSTATE_k = rtb_Sum1_g;
        if (rtDW.Integrator_DSTATE_k > 250.0F) {
          rtDW.Integrator_DSTATE_k = 250.0F;
        } else if (rtDW.Integrator_DSTATE_k < -250.0F) {
          rtDW.Integrator_DSTATE_k = -250.0F;
        }
      }

      // Saturate: '<S13>/Saturation1'
      if (rtb_Sum1_k > 4000.0F) {
        u0 = 4000.0F;
      } else if (rtb_Sum1_k < 0.0F) {
        u0 = 0.0F;
      } else {
        u0 = rtb_Sum1_k;
      }

      // Sum: '<S13>/Sum4' incorporates:
      //   DiscreteIntegrator: '<S36>/Integrator'
      //   Saturate: '<S13>/Saturation1'

      u0 += rtDW.Integrator_DSTATE_k;

      // Saturate: '<S13>/Saturation2'
      if (u0 > 4000.0F) {
        u0 = 4000.0F;
      } else if (u0 < 0.0F) {
        u0 = 0.0F;
      }

      // Sum: '<S13>/Sum3' incorporates:
      //   DiscreteIntegrator: '<S43>/Integrator'
      //   Gain: '<S13>/Gain2'
      //   MinMax: '<S13>/Min'
      //   Saturate: '<S13>/Saturation2'

      rtb_TSamp = rtDW.Integrator_DSTATE - std::fmin(0.93F *
        rtDW.Integrator_DSTATE, u0);

      // Saturate: '<S21>/Saturation' incorporates:
      //   DiscreteIntegrator: '<S43>/Integrator'

      if (rtDW.Integrator_DSTATE > 5000.0F) {
        rtb_Gain1 = 5000.0F;
      } else if (rtDW.Integrator_DSTATE < 0.0F) {
        rtb_Gain1 = 0.0F;
      } else {
        rtb_Gain1 = rtDW.Integrator_DSTATE;
      }

      // End of Saturate: '<S21>/Saturation'

      // Switch: '<S44>/Switch2' incorporates:
      //   Constant: '<S21>/Constant'
      //   RelationalOperator: '<S44>/LowerRelop1'
      //   RelationalOperator: '<S44>/UpperRelop'
      //   Switch: '<S44>/Switch'

      if (rtb_TSamp > rtb_Gain1) {
        rtb_TSamp = rtb_Gain1;
      } else if (rtb_TSamp < 0.0F) {
        // Switch: '<S44>/Switch' incorporates:
        //   Constant: '<S21>/Constant'

        rtb_TSamp = 0.0F;
      }

      // End of Switch: '<S44>/Switch2'

      // Gain: '<S21>/Gain' incorporates:
      //   Bias: '<S21>/Bias'
      //   Product: '<S21>/Product'
      //   UnaryMinus: '<S21>/Unary Minus'

      rtb_Gain1 = (-(1.0F / rtb_Gain1 * rtb_TSamp) + 1.0F) * 1.08731103F;

      // Gain: '<S21>/Gain2' incorporates:
      //   Bias: '<S21>/Bias1'
      //   Constant: '<S21>/Constant1'
      //   Gain: '<S21>/Gain1'
      //   MinMax: '<S21>/Max'
      //   Product: '<S21>/Product1'
      //   Sqrt: '<S21>/Sqrt'

      u0 = std::sqrt(std::fmax(rtb_Gain1 / (-51.8534F * rtb_Gain1 + 52.8534F),
        0.0F)) * 1080.0F;

      // Saturate: '<S13>/Saturation'
      if (u0 > 1080.0F) {
        u0 = 1080.0F;
      }

      // Product: '<S13>/Product' incorporates:
      //   Constant: '<S17>/Constant'
      //   RelationalOperator: '<S17>/Compare'
      //   Saturate: '<S13>/Saturation'

      rtDW.Product_n = static_cast<real32_T>(rtb_Sum1_k >= 5.0F) * u0;

      // Update for DiscreteIntegrator: '<S43>/Integrator' incorporates:
      //   Constant: '<S37>/Time constant'
      //   Inport: '<Root>/P_manifold'
      //   MinMax: '<S37>/Max'
      //   Product: '<S20>/1//T'
      //   Sum: '<S20>/Sum1'

      rtDW.Integrator_IC_LOADING = 0U;
      rtDW.Integrator_DSTATE += 1.0F / std::fmax(rtDW.Probe[0], 0.005F) *
        (rtU.P_manifold - rtDW.Integrator_DSTATE) * 0.001F;
      rtDW.Integrator_PrevResetState = static_cast<int8_T>(rtb_LogicalOperator);

      // Update for DiscreteIntegrator: '<S29>/Integrator' incorporates:
      //   Constant: '<S23>/Time constant'
      //   MinMax: '<S23>/Max'
      //   Product: '<S18>/1//T'
      //   Sum: '<S18>/Sum1'

      rtDW.Integrator_IC_LOADING_b = 0U;
      rtDW.Integrator_DSTATE_l += 1.0F / static_cast<real32_T>(std::fmax(
        static_cast<real_T>(rtDW.Probe_e[0]), 0.001)) * (rtb_Product1_k -
        rtDW.Integrator_DSTATE_l) * 0.001F;
      rtDW.Integrator_PrevResetState_o = static_cast<int8_T>
        (rtb_LogicalOperator_c);

      // Update for DiscreteIntegrator: '<S36>/Integrator' incorporates:
      //   Constant: '<S30>/Time constant'
      //   MinMax: '<S30>/Max'
      //   Product: '<S19>/1//T'
      //   Sum: '<S19>/Sum1'

      rtDW.Integrator_IC_LOADING_bm = 0U;
      rtDW.Integrator_DSTATE_k += 1.0F / static_cast<real32_T>(std::fmax(
        static_cast<real_T>(rtDW.Probe_k[0]), 0.004)) * (rtb_Sum1_g -
        rtDW.Integrator_DSTATE_k) * 0.001F;
      if (rtDW.Integrator_DSTATE_k > 250.0F) {
        rtDW.Integrator_DSTATE_k = 250.0F;
      } else if (rtDW.Integrator_DSTATE_k < -250.0F) {
        rtDW.Integrator_DSTATE_k = -250.0F;
      }

      rtDW.Integrator_PrevResetState_h = static_cast<int8_T>
        (rtb_LogicalOperator_k);

      // End of Update for DiscreteIntegrator: '<S36>/Integrator'
    } else if (rtDW.Subsystem_MODE) {
      // Disable for Product: '<S13>/Product' incorporates:
      //   Outport: '<S13>/theta_demand'

      rtDW.Product_n = 0.0F;
      rtDW.Subsystem_MODE = false;
    }

    // End of RelationalOperator: '<S4>/Compare'
    // End of Outputs for SubSystem: '<S1>/Subsystem'

    // Switch: '<S1>/Switch1' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S6>/Constant'
    //   Inport: '<Root>/pos_ref_ext'
    //   RelationalOperator: '<S6>/Compare'

    if (controller_mode == controller_modes::POSITION) {
      rtb_TSamp = rtU.pos_ref_ext;
    } else {
      rtb_TSamp = rtDW.Product_n;
    }

    // End of Switch: '<S1>/Switch1'

    // Outport: '<Root>/position_demand'
    rtY.position_demand = rtb_TSamp;

    // Outport: '<Root>/ThrustMax' incorporates:
    //   Gain: '<S1>/Gain'
    //   Inport: '<Root>/P_manifold'
    //   Inport: '<Root>/nozzle_gain'
    //   Product: '<S1>/Product'

    rtY.ThrustMax = 0.8F * rtU.P_manifold / rtU.nozzle_gain;

    // Outport: '<Root>/ThrustEstimate' incorporates:
    //   Inport: '<Root>/P_nozzle'
    //   Inport: '<Root>/nozzle_gain'
    //   Product: '<S1>/Product1'

    rtY.ThrustEstimate = rtU.P_nozzle / rtU.nozzle_gain;

    // Saturate: '<S15>/Saturation1'
    if (rtb_TSamp > 1440.0F) {
      rtb_TSamp = 1440.0F;
    } else if (rtb_TSamp < 0.0F) {
      rtb_TSamp = 0.0F;
    }

    // Gain: '<S45>/Gain' incorporates:
    //   DiscreteIntegrator: '<S45>/Discrete-Time Integrator'
    //   Saturate: '<S15>/Saturation1'
    //   Sum: '<S45>/Sum1'

    u0 = (rtb_TSamp - rtDW.DiscreteTimeIntegrator_DSTATE) * 188.49556F;

    // Saturate: '<S45>/Saturation'
    if (u0 > 2400.0F) {
      u0 = 2400.0F;
    } else if (u0 < -2400.0F) {
      u0 = -2400.0F;
    }

    // Update for DiscreteIntegrator: '<S45>/Discrete-Time Integrator' incorporates:
    //   Saturate: '<S45>/Saturation'

    rtDW.DiscreteTimeIntegrator_DSTATE += 0.001F * u0;
  }

  // Model initialize function
  void actuatorController::initialize()
  {
    // SystemInitialize for Enabled SubSystem: '<S1>/Subsystem'
    // Start for Probe: '<S37>/Probe'
    rtDW.Probe[0] = 0.001F;
    rtDW.Probe[1] = 0.0F;

    // Start for Probe: '<S23>/Probe'
    rtDW.Probe_e[0] = 0.001F;
    rtDW.Probe_e[1] = 0.0F;

    // Start for Probe: '<S30>/Probe'
    rtDW.Probe_k[0] = 0.001F;
    rtDW.Probe_k[1] = 0.0F;

    // InitializeConditions for DiscreteIntegrator: '<S43>/Integrator'
    rtDW.Integrator_IC_LOADING = 1U;

    // InitializeConditions for DiscreteIntegrator: '<S29>/Integrator'
    rtDW.Integrator_IC_LOADING_b = 1U;

    // InitializeConditions for DiscreteIntegrator: '<S36>/Integrator'
    rtDW.Integrator_IC_LOADING_bm = 1U;

    // End of SystemInitialize for SubSystem: '<S1>/Subsystem'
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
