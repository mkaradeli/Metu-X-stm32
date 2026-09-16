//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: actuatorController.cpp
//
// Code generated for Simulink model 'actuatorController'.
//
// Model version                  : 1.83
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Wed Sep 16 22:02:48 2026
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
#include "limits"
#include "cmath"

const real_T period{ 0.001 };

// Exported block parameters
struct_IJnI4imAtcq7GOnq1yNUdE currentControllerGains{
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
    0.4F,
    3.0F
  }
} ;                                    // Variable: controllerGains
                                          //  Referenced by:
                                          //    '<S12>/Constant'
                                          //    '<S12>/Constant1'
                                          //    '<S12>/Gain'
                                          //    '<S12>/Gain1'
                                          //    '<S13>/Discrete-Time Integrator'
                                          //    '<S13>/Gain'
                                          //    '<S13>/Gain1'
                                          //    '<S13>/Gain2'
                                          //    '<S13>/Rate Limiter'
                                          //    '<S73>/Constant1'
                                          //    '<S50>/Integral Gain'
                                          //    '<S58>/Proportional Gain'


controller_modes controller_mode{ controller_modes::DISABLE };// Variable: controller_mode
                                                                 //  Referenced by: '<S1>/controller_mode'


static real32_T look1_iflf_bingc(real32_T u0, const real32_T bp0[], const
  real32_T table[], uint32_T maxIndex);
//extern "C"
//{
//  real_T rtNaN { -std::numeric_limits<real_T>::quiet_NaN() };
//
//  real_T rtInf { std::numeric_limits<real_T>::infinity() };
//
//  real_T rtMinusInf { -std::numeric_limits<real_T>::infinity() };
//
//  real32_T rtNaNF { -std::numeric_limits<real32_T>::quiet_NaN() };
//
//  real32_T rtInfF { std::numeric_limits<real32_T>::infinity() };
//
//  real32_T rtMinusInfF { -std::numeric_limits<real32_T>::infinity() };
//}

static real32_T look1_iflf_bingc(real32_T u0, const real32_T bp0[], const
  real32_T table[], uint32_T maxIndex)
{
  real32_T frac;
  uint32_T iLeft;

  // Column-major Lookup 1-D
  // Search method: 'binary'
  // Use previous index: 'off'
  // Interpolation method: 'Linear Lagrange'
  // Extrapolation method: 'Clip'
  // Use last breakpoint for index at or above upper limit: 'off'
  // Remove protection against out-of-range input in generated code: 'off'

  // Prelookup - Index and Fraction
  // Index Search method: 'binary'
  // Extrapolation method: 'Clip'
  // Use previous index: 'off'
  // Use last breakpoint for index at or above upper limit: 'off'
  // Remove protection against out-of-range input in generated code: 'off'

  if (u0 <= bp0[0U]) {
    iLeft = 0U;
    frac = 0.0F;
  } else if (u0 < bp0[maxIndex]) {
    uint32_T bpIdx;
    uint32_T iRght;

    // Binary Search
    bpIdx = maxIndex >> 1U;
    iLeft = 0U;
    iRght = maxIndex;
    while (iRght - iLeft > 1U) {
      if (u0 < bp0[bpIdx]) {
        iRght = bpIdx;
      } else {
        iLeft = bpIdx;
      }

      bpIdx = (iRght + iLeft) >> 1U;
    }

    frac = (u0 - bp0[iLeft]) / (bp0[iLeft + 1U] - bp0[iLeft]);
  } else {
    iLeft = maxIndex - 1U;
    frac = 1.0F;
  }

  // Column-major Interpolation 1-D
  // Interpolation method: 'Linear Lagrange'
  // Use last breakpoint for index at or above upper limit: 'off'
  // Overflow mode: 'wrapping'

  return (1.0F - frac) * table[iLeft] + table[iLeft + 1U] * frac;
}

namespace controller
{
  // Model step function
  void actuatorController::step()
  {
    real_T rateLimiterRate;
    real_T rtb_Saturation3;
    real_T rtb_Sum;
    real_T tmp;
    real32_T rtb_SpeedDemand;
    real32_T rtb_Switch_i4;
    real32_T rtb_Switch_l;
    real32_T rtb_TSamp;
    real32_T tmp_0;

    // Outputs for Enabled SubSystem: '<S1>/Position Controller' incorporates:
    //   EnablePort: '<S12>/Enable'

    // RelationalOperator: '<S4>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S4>/Constant'

    if (controller_mode >= rtP.CompareToConstant1_const) {
      if (!rtDW.PositionController_MODE) {
        // InitializeConditions for UnitDelay: '<S71>/UD'
        //
        //  Block description for '<S71>/UD':
        //
        //   Store in Global RAM

        rtDW.UD_DSTATE = rtP.DiscreteDerivative_ICPrevScaled;
        rtDW.PositionController_MODE = true;
      }

      // SampleTimeMath: '<S71>/TSamp' incorporates:
      //   DiscreteIntegrator: '<S75>/Discrete-Time Integrator'
      //
      //  About '<S71>/TSamp':
      //   y = u * K where K = 1 / ( w * Ts )
      //
      rtb_TSamp = rtDW.DiscreteTimeIntegrator_DSTATE * rtP.TSamp_WtEt;

      // Sum: '<S12>/Sum1' incorporates:
      //   DiscreteIntegrator: '<S75>/Discrete-Time Integrator'
      //   Gain: '<S12>/Gain'
      //   Gain: '<S12>/Gain1'
      //   Inport: '<Root>/pos_feedback'
      //   Sum: '<S12>/Sum'
      //   Sum: '<S71>/Diff'
      //   UnitDelay: '<S71>/UD'
      //
      //  Block description for '<S71>/Diff':
      //
      //   Add in CPU
      //
      //  Block description for '<S71>/UD':
      //
      //   Store in Global RAM

      rtb_SpeedDemand = (rtb_TSamp - rtDW.UD_DSTATE) *
        currentControllerGains.position.Kff +
        (rtDW.DiscreteTimeIntegrator_DSTATE - rtU.pos_feedback) *
        currentControllerGains.position.Kp;

      // Switch: '<S72>/Switch2' incorporates:
      //   Constant: '<S12>/Constant'
      //   Inport: '<Root>/pos_feedback'
      //   RelationalOperator: '<S72>/LowerRelop1'
      //   Switch: '<S12>/Switch'

      if (rtb_SpeedDemand > currentControllerGains.position.SatMax) {
        // Switch: '<S72>/Switch2'
        rtDW.Switch2_h = currentControllerGains.position.SatMax;
      } else {
        if (rtU.pos_feedback > rtP.Switch_Threshold) {
          // Switch: '<S12>/Switch' incorporates:
          //   Constant: '<S12>/Constant1'

          rtb_Switch_l = currentControllerGains.position.SatMin;
        } else {
          // Switch: '<S12>/Switch' incorporates:
          //   Constant: '<S12>/Constant2'

          rtb_Switch_l = rtP.Constant2_Value;
        }

        // Switch: '<S72>/Switch' incorporates:
        //   RelationalOperator: '<S72>/UpperRelop'

        if (rtb_SpeedDemand < rtb_Switch_l) {
          // Switch: '<S72>/Switch2'
          rtDW.Switch2_h = rtb_Switch_l;
        } else {
          // Switch: '<S72>/Switch2'
          rtDW.Switch2_h = rtb_SpeedDemand;
        }

        // End of Switch: '<S72>/Switch'
      }

      // End of Switch: '<S72>/Switch2'

      // Update for UnitDelay: '<S71>/UD'
      //
      //  Block description for '<S71>/UD':
      //
      //   Store in Global RAM

      rtDW.UD_DSTATE = rtb_TSamp;
    } else if (rtDW.PositionController_MODE) {
      // Disable for Switch: '<S72>/Switch2' incorporates:
      //   Outport: '<S12>/SpeedDemand'

      rtDW.Switch2_h = rtP.SpeedDemand_Y0;
      rtDW.PositionController_MODE = false;
    }

    // End of RelationalOperator: '<S4>/Compare'
    // End of Outputs for SubSystem: '<S1>/Position Controller'

    // Switch: '<S1>/Switch' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S6>/Constant'
    //   Inport: '<Root>/speedDemand_ext'
    //   RelationalOperator: '<S6>/Compare'
    //   Sum: '<S1>/Sum'

    if (controller_mode == rtP.CompareToConstant3_const) {
      rtb_TSamp = rtU.speedDemand_ext;
    } else {
      rtb_TSamp = rtDW.Switch2_h;
    }

    // End of Switch: '<S1>/Switch'

    // Gain: '<S10>/Gain1'
    rtb_SpeedDemand = rtP.Gain1_Gain * rtb_TSamp;

    // Outputs for Enabled SubSystem: '<S1>/Speed Controller' incorporates:
    //   EnablePort: '<S13>/Enable'

    // RelationalOperator: '<S3>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S3>/Constant'

    if (controller_mode >= rtP.CompareToConstant_const) {
      if (!rtDW.SpeedController_MODE) {
        // InitializeConditions for DiscreteIntegrator: '<S13>/Discrete-Time Integrator' 
        rtDW.DiscreteTimeIntegrator_DSTATE_b = rtP.DiscreteTimeIntegrator_IC;

        // InitializeConditions for RateLimiter: '<S13>/Rate Limiter'
        rtDW.PrevY_h = rtP.RateLimiter_IC_o;
        rtDW.SpeedController_MODE = true;
      }

      // RateLimiter: '<S13>/Rate Limiter'
      rtb_Switch_l = rtb_SpeedDemand - rtDW.PrevY_h;
      tmp_0 = static_cast<real32_T>(currentControllerGains.speed.RateLimiterMax *
        period);
      if (rtb_Switch_l > tmp_0) {
        rtb_SpeedDemand = tmp_0 + rtDW.PrevY_h;
      } else if (rtb_Switch_l < static_cast<real32_T>
                 (currentControllerGains.speed.RateLimiterMin * period)) {
        rtb_SpeedDemand = static_cast<real32_T>
          (currentControllerGains.speed.RateLimiterMin * period) + rtDW.PrevY_h;
      }

      rtDW.PrevY_h = rtb_SpeedDemand;

      // End of RateLimiter: '<S13>/Rate Limiter'

      // Sum: '<S13>/Sum' incorporates:
      //   Gain: '<S11>/Gain1'
      //   Inport: '<Root>/speed_feedback'

      rtb_SpeedDemand -= rtP.Gain1_Gain_p * rtU.SpeedFeedback;

      // Sum: '<S13>/Sum1' incorporates:
      //   DiscreteIntegrator: '<S13>/Discrete-Time Integrator'
      //   Gain: '<S13>/Gain'

      rtb_Switch_l = currentControllerGains.speed.Kp * rtb_SpeedDemand +
        rtDW.DiscreteTimeIntegrator_DSTATE_b;

      // Switch: '<S74>/Switch2' incorporates:
      //   Constant: '<S13>/Constant'
      //   Inport: '<Root>/pos_feedback'
      //   RelationalOperator: '<S74>/LowerRelop1'
      //   Switch: '<S73>/Switch'
      //   Switch: '<S73>/Switch1'

      if (rtb_Switch_l > rtP.Constant_Value_o) {
        // Outport: '<Root>/currentDemand'
        rtY.currentDemand = static_cast<real32_T>(rtP.Constant_Value_o);
      } else {
        if (rtU.pos_feedback > rtP.Switch_Threshold_h) {
          // Switch: '<S73>/Switch' incorporates:
          //   Constant: '<S73>/Constant1'

          rtb_Switch_i4 = currentControllerGains.speed.SatMin;
        } else if (rtU.pos_feedback > rtP.Switch1_Threshold) {
          // Switch: '<S73>/Switch1' incorporates:
          //   Constant: '<S73>/Constant2'
          //   Switch: '<S73>/Switch'

          rtb_Switch_i4 = rtP.Constant2_Value_p;
        } else {
          // Switch: '<S73>/Switch' incorporates:
          //   Constant: '<S73>/Constant3'

          rtb_Switch_i4 = rtP.Constant3_Value;
        }

        // Switch: '<S74>/Switch' incorporates:
        //   Inport: '<Root>/pos_feedback'
        //   RelationalOperator: '<S74>/UpperRelop'
        //   Switch: '<S73>/Switch'
        //   Switch: '<S73>/Switch1'

        if (rtb_Switch_l < rtb_Switch_i4) {
          // Outport: '<Root>/currentDemand'
          rtY.currentDemand = rtb_Switch_i4;
        } else {
          // Outport: '<Root>/currentDemand'
          rtY.currentDemand = rtb_Switch_l;
        }

        // End of Switch: '<S74>/Switch'
      }

      // End of Switch: '<S74>/Switch2'

      // Update for DiscreteIntegrator: '<S13>/Discrete-Time Integrator' incorporates:
      //   Gain: '<S13>/Gain1'
      //   Gain: '<S13>/Gain2'
      //   Outport: '<Root>/currentDemand'
      //   Sum: '<S13>/Sum2'
      //   Sum: '<S13>/Sum3'

      rtDW.DiscreteTimeIntegrator_DSTATE_b += (currentControllerGains.speed.Ki /
        currentControllerGains.speed.Kp * (rtY.currentDemand - rtb_Switch_l) +
        currentControllerGains.speed.Ki * rtb_SpeedDemand) *
        rtP.DiscreteTimeIntegrator_gainval;
      if (rtDW.DiscreteTimeIntegrator_DSTATE_b >
          currentControllerGains.speed.SatMax) {
        rtDW.DiscreteTimeIntegrator_DSTATE_b =
          currentControllerGains.speed.SatMax;
      } else if (rtDW.DiscreteTimeIntegrator_DSTATE_b <
                 currentControllerGains.speed.SatMin) {
        rtDW.DiscreteTimeIntegrator_DSTATE_b =
          currentControllerGains.speed.SatMin;
      }

      // End of Update for DiscreteIntegrator: '<S13>/Discrete-Time Integrator'
    } else if (rtDW.SpeedController_MODE) {
      // Disable for Outport: '<Root>/currentDemand' incorporates:
      //   Outport: '<S13>/Y'

      rtY.currentDemand = rtP.Y_Y0;
      rtDW.SpeedController_MODE = false;
    }

    // End of RelationalOperator: '<S3>/Compare'
    // End of Outputs for SubSystem: '<S1>/Speed Controller'

    // Outputs for Enabled SubSystem: '<S1>/Thrust Controller' incorporates:
    //   EnablePort: '<S14>/Enable'

    // RelationalOperator: '<S8>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S8>/Constant'

    if (controller_mode >= rtP.CompareToConstant5_const) {
      // Saturate: '<S14>/Saturation1' incorporates:
      //   Inport: '<Root>/F_demand'

      if (rtU.F_demand > rtP.Saturation1_UpperSat_g) {
        tmp_0 = rtP.Saturation1_UpperSat_g;
      } else if (rtU.F_demand < rtP.Saturation1_LowerSat_h) {
        tmp_0 = rtP.Saturation1_LowerSat_h;
      } else {
        tmp_0 = rtU.F_demand;
      }

      // Saturate: '<S14>/Saturation' incorporates:
      //   Inport: '<Root>/nozzle_gain'

      if (rtU.nozzle_gain > rtP.Saturation_UpperSat_p) {
        rtb_SpeedDemand = rtP.Saturation_UpperSat_p;
      } else if (rtU.nozzle_gain < rtP.Saturation_LowerSat_k) {
        rtb_SpeedDemand = rtP.Saturation_LowerSat_k;
      } else {
        rtb_SpeedDemand = rtU.nozzle_gain;
      }

      // Product: '<S14>/Product' incorporates:
      //   Saturate: '<S14>/Saturation'
      //   Saturate: '<S14>/Saturation1'

      rtDW.Product = tmp_0 * rtb_SpeedDemand;
    }

    // End of RelationalOperator: '<S8>/Compare'
    // End of Outputs for SubSystem: '<S1>/Thrust Controller'

    // Switch: '<S1>/Switch2' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S9>/Constant'
    //   Inport: '<Root>/P_nozzle_demand'
    //   RelationalOperator: '<S9>/Compare'

    if (controller_mode == rtP.CompareToConstant6_const) {
      rtb_Switch_l = rtU.P_nozzle_demand;
    } else {
      rtb_Switch_l = rtDW.Product;
    }

    // Saturate: '<S1>/Saturation' incorporates:
    //   Switch: '<S1>/Switch2'

    if (rtb_Switch_l > rtP.Saturation_UpperSat_k) {
      rtb_Switch_l = rtP.Saturation_UpperSat_k;
    } else if (rtb_Switch_l < rtP.Saturation_LowerSat_n) {
      rtb_Switch_l = rtP.Saturation_LowerSat_n;
    }

    // End of Saturate: '<S1>/Saturation'

    // Outputs for Enabled SubSystem: '<S1>/1D Valve Lookup Controller External Table' incorporates:
    //   EnablePort: '<S2>/Enable'

    // RelationalOperator: '<S5>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S5>/Constant'

    if (controller_mode >= rtP.CompareToConstant2_const) {
      // Gain: '<S2>/Gain' incorporates:
      //   Inport: '<Root>/P_manifold'

      rtb_Switch_i4 = rtP.Gain_Gain * rtU.P_manifold;

      // Saturate: '<S2>/Saturation'
      if (rtb_Switch_i4 > rtP.Saturation_UpperSat) {
        rtb_Switch_i4 = rtP.Saturation_UpperSat;
      } else if (rtb_Switch_i4 < rtP.Saturation_LowerSat) {
        rtb_Switch_i4 = rtP.Saturation_LowerSat;
      }

      // End of Saturate: '<S2>/Saturation'

      // Switch: '<S18>/Switch2' incorporates:
      //   RelationalOperator: '<S18>/LowerRelop1'

      if (rtb_Switch_l <= rtb_Switch_i4) {
        // Switch: '<S18>/Switch' incorporates:
        //   Constant: '<S2>/Constant'
        //   RelationalOperator: '<S18>/UpperRelop'

        if (rtb_Switch_l < rtP.Constant_Value) {
          rtb_Switch_i4 = static_cast<real32_T>(rtP.Constant_Value);
        } else {
          rtb_Switch_i4 = rtb_Switch_l;
        }

        // End of Switch: '<S18>/Switch'
      }

      // End of Switch: '<S18>/Switch2'

      // Saturate: '<S2>/Saturation1' incorporates:
      //   Inport: '<Root>/P_manifold'

      if (rtU.P_manifold > rtP.Saturation1_UpperSat) {
        tmp_0 = rtP.Saturation1_UpperSat;
      } else if (rtU.P_manifold < rtP.Saturation1_LowerSat) {
        tmp_0 = rtP.Saturation1_LowerSat;
      } else {
        tmp_0 = rtU.P_manifold;
      }

      // Lookup_n-D: '<S2>/1-D Lookup Table' incorporates:
      //   Inport: '<Root>/ValveFitPressureRatios'
      //   Product: '<S2>/Divide'
      //   Saturate: '<S2>/Saturation1'

      rtb_SpeedDemand = look1_iflf_bingc(rtb_Switch_i4 / tmp_0,
        rtU.ValveFitPressureRatios, rtP.uDLookupTable_tableData, 10U);

      // Sum: '<S2>/Sum2' incorporates:
      //   Inport: '<Root>/P_nozzle'

      rtb_Switch_i4 -= rtU.P_nozzle;

      // Sum: '<S62>/Sum' incorporates:
      //   DiscreteIntegrator: '<S53>/Integrator'
      //   Gain: '<S58>/Proportional Gain'

      rtb_Sum = currentControllerGains.pressure.Kp * rtb_Switch_i4 +
        rtDW.Integrator_DSTATE;

      // Switch: '<S2>/Switch' incorporates:
      //   Constant: '<S16>/Constant'
      //   Constant: '<S2>/Constant1'
      //   Inport: '<Root>/P_nozzle'
      //   RelationalOperator: '<S16>/Compare'

      if (rtU.P_nozzle >= rtP.CompareToConstant_const_p) {
        tmp = rtb_Sum;
      } else {
        tmp = rtP.Constant1_Value;
      }

      // Sum: '<S2>/Sum' incorporates:
      //   Switch: '<S2>/Switch'

      rtb_Saturation3 = rtb_SpeedDemand + tmp;

      // Saturate: '<S2>/Saturation3'
      if (rtb_Saturation3 > rtP.Saturation3_UpperSat) {
        rtb_Saturation3 = rtP.Saturation3_UpperSat;
      } else if (rtb_Saturation3 < rtP.Saturation3_LowerSat) {
        rtb_Saturation3 = rtP.Saturation3_LowerSat;
      }

      // End of Saturate: '<S2>/Saturation3'

      // RateLimiter: '<S2>/Rate Limiter'
      rateLimiterRate = rtb_Saturation3 - rtDW.PrevY;
      tmp = rtP.RateLimiter_RisingLim * period;
      if (rateLimiterRate > tmp) {
        // RateLimiter: '<S2>/Rate Limiter'
        rtDW.RateLimiter = tmp + rtDW.PrevY;
      } else if (rateLimiterRate < rtP.RateLimiter_FallingLim * period) {
        // RateLimiter: '<S2>/Rate Limiter'
        rtDW.RateLimiter = rtP.RateLimiter_FallingLim * period + rtDW.PrevY;
      } else {
        // RateLimiter: '<S2>/Rate Limiter'
        rtDW.RateLimiter = rtb_Saturation3;
      }

      rtDW.PrevY = rtDW.RateLimiter;

      // End of RateLimiter: '<S2>/Rate Limiter'

      // Update for DiscreteIntegrator: '<S53>/Integrator' incorporates:
      //   Gain: '<S50>/Integral Gain'
      //   Gain: '<S64>/Kt'
      //   Sum: '<S2>/Sum1'
      //   Sum: '<S64>/SumI3'
      //   Sum: '<S65>/SumI1'

      rtDW.Integrator_DSTATE += (((rtDW.RateLimiter - rtb_SpeedDemand) - rtb_Sum)
        * rtP.PIDController_Kt + currentControllerGains.pressure.Ki *
        rtb_Switch_i4) * rtP.Integrator_gainval;
    }

    // End of RelationalOperator: '<S5>/Compare'
    // End of Outputs for SubSystem: '<S1>/1D Valve Lookup Controller External Table' 

    // Switch: '<S1>/Switch1' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S7>/Constant'
    //   Inport: '<Root>/pos_ref_ext'
    //   RelationalOperator: '<S7>/Compare'

    if (controller_mode == rtP.CompareToConstant4_const) {
      rtb_SpeedDemand = rtU.pos_ref_ext;
    } else {
      rtb_SpeedDemand = static_cast<real32_T>(rtDW.RateLimiter);
    }

    // End of Switch: '<S1>/Switch1'

    // Outport: '<Root>/position_demand'
    rtY.position_demand = rtb_SpeedDemand;

    // Outport: '<Root>/P_nozzle_demand1'
    rtY.P_nozzle_demand1 = rtb_Switch_l;

    // Outport: '<Root>/speedDemand'
    rtY.speedDemand = rtb_TSamp;

    // Outport: '<Root>/pos_ref_rate_limited' incorporates:
    //   DiscreteIntegrator: '<S75>/Discrete-Time Integrator'

    rtY.pos_ref_rate_limited = rtDW.DiscreteTimeIntegrator_DSTATE;

    // Outport: '<Root>/ThrustMax' incorporates:
    //   Gain: '<S1>/Gain'
    //   Inport: '<Root>/P_manifold'
    //   Inport: '<Root>/nozzle_gain'
    //   Product: '<S1>/Product'

    rtY.ThrustMax = rtP.Gain_Gain_j * rtU.P_manifold / rtU.nozzle_gain;

    // Outport: '<Root>/ThrustEstimate' incorporates:
    //   Inport: '<Root>/P_nozzle'
    //   Inport: '<Root>/nozzle_gain'
    //   Product: '<S1>/Product1'

    rtY.ThrustEstimate = rtU.P_nozzle / rtU.nozzle_gain;

    // Saturate: '<S15>/Saturation1'
    if (rtb_SpeedDemand > rtP.Saturation1_UpperSat_k) {
      rtb_SpeedDemand = rtP.Saturation1_UpperSat_k;
    } else if (rtb_SpeedDemand < rtP.Saturation1_LowerSat_k) {
      rtb_SpeedDemand = rtP.Saturation1_LowerSat_k;
    }

    // Gain: '<S75>/Gain' incorporates:
    //   DiscreteIntegrator: '<S75>/Discrete-Time Integrator'
    //   Gain: '<S75>/K'
    //   Saturate: '<S15>/Saturation1'
    //   Sum: '<S75>/Sum1'

    rtb_TSamp = (rtP.K_Gain * rtb_SpeedDemand -
                 rtDW.DiscreteTimeIntegrator_DSTATE) * rtP.Gain_Gain_h;

    // Saturate: '<S75>/Saturation'
    if (rtb_TSamp > rtP.Saturation_UpperSat_kk) {
      rtb_TSamp = rtP.Saturation_UpperSat_kk;
    } else if (rtb_TSamp < rtP.Saturation_LowerSat_kk) {
      rtb_TSamp = rtP.Saturation_LowerSat_kk;
    }

    // Update for DiscreteIntegrator: '<S75>/Discrete-Time Integrator' incorporates:
    //   Saturate: '<S75>/Saturation'

    rtDW.DiscreteTimeIntegrator_DSTATE += rtP.DiscreteTimeIntegrator_gainva_l *
      rtb_TSamp;
  }

  // Model initialize function
  void actuatorController::initialize()
  {
    // InitializeConditions for DiscreteIntegrator: '<S75>/Discrete-Time Integrator' 
    rtDW.DiscreteTimeIntegrator_DSTATE = rtP.DiscreteTimeIntegrator_IC_c;

    // SystemInitialize for Enabled SubSystem: '<S1>/Position Controller'
    // InitializeConditions for UnitDelay: '<S71>/UD'
    //
    //  Block description for '<S71>/UD':
    //
    //   Store in Global RAM

    rtDW.UD_DSTATE = rtP.DiscreteDerivative_ICPrevScaled;

    // SystemInitialize for Switch: '<S72>/Switch2' incorporates:
    //   Outport: '<S12>/SpeedDemand'

    rtDW.Switch2_h = rtP.SpeedDemand_Y0;

    // End of SystemInitialize for SubSystem: '<S1>/Position Controller'

    // SystemInitialize for Enabled SubSystem: '<S1>/Speed Controller'
    // InitializeConditions for DiscreteIntegrator: '<S13>/Discrete-Time Integrator' 
    rtDW.DiscreteTimeIntegrator_DSTATE_b = rtP.DiscreteTimeIntegrator_IC;

    // InitializeConditions for RateLimiter: '<S13>/Rate Limiter'
    rtDW.PrevY_h = rtP.RateLimiter_IC_o;

    // SystemInitialize for Outport: '<Root>/currentDemand' incorporates:
    //   Outport: '<S13>/Y'

    rtY.currentDemand = rtP.Y_Y0;

    // End of SystemInitialize for SubSystem: '<S1>/Speed Controller'

    // SystemInitialize for Enabled SubSystem: '<S1>/Thrust Controller'
    // SystemInitialize for Product: '<S14>/Product' incorporates:
    //   Outport: '<S14>/P_nozzle_demand'

    rtDW.Product = rtP.P_nozzle_demand_Y0;

    // End of SystemInitialize for SubSystem: '<S1>/Thrust Controller'

    // SystemInitialize for Enabled SubSystem: '<S1>/1D Valve Lookup Controller External Table' 
    // InitializeConditions for DiscreteIntegrator: '<S53>/Integrator'
    rtDW.Integrator_DSTATE = rtP.PIDController_InitialConditionF;

    // InitializeConditions for RateLimiter: '<S2>/Rate Limiter'
    rtDW.PrevY = rtP.RateLimiter_IC;

    // SystemInitialize for RateLimiter: '<S2>/Rate Limiter' incorporates:
    //   Outport: '<S2>/Theta'

    rtDW.RateLimiter = rtP.Theta_Y0;

    // End of SystemInitialize for SubSystem: '<S1>/1D Valve Lookup Controller External Table' 
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
