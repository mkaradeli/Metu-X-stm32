//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: actuatorController.cpp
//
// Code generated for Simulink model 'actuatorController'.
//
// Model version                  : 1.63
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Mon Aug 17 17:30:41 2026
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
    0.15F
  }
} ;                                    // Variable: controllerGains
                                          //  Referenced by:
                                          //    '<S2>/Gain1'
                                          //    '<S12>/Gain'
                                          //    '<S12>/Gain1'
                                          //    '<S12>/Saturation'
                                          //    '<S13>/Discrete-Time Integrator'
                                          //    '<S13>/Gain'
                                          //    '<S13>/Gain1'
                                          //    '<S13>/Gain2'
                                          //    '<S13>/Rate Limiter'
                                          //    '<S13>/Saturation'


controller_modes controller_mode{ controller_modes::DISABLE };// Variable: controller_mode
                                                                 //  Referenced by: '<S1>/controller_mode'


static real32_T look1_iflf_bingc(real32_T u0, const real32_T bp0[], const
  real32_T table[], uint32_T maxIndex);
extern "C"
{
  real_T rtNaN { -std::numeric_limits<real_T>::quiet_NaN() };

  real_T rtInf { std::numeric_limits<real_T>::infinity() };

  real_T rtMinusInf { -std::numeric_limits<real_T>::infinity() };

  real32_T rtNaNF { -std::numeric_limits<real32_T>::quiet_NaN() };

  real32_T rtInfF { std::numeric_limits<real32_T>::infinity() };

  real32_T rtMinusInfF { -std::numeric_limits<real32_T>::infinity() };
}

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
    real_T u0_0;
    real32_T rtb_Gain1;
    real32_T rtb_Sum_m;
    real32_T rtb_TSamp;
    real32_T u0;

    // Outputs for Enabled SubSystem: '<S1>/Position Controller' incorporates:
    //   EnablePort: '<S12>/Enable'

    // RelationalOperator: '<S4>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S4>/Constant'

    if (controller_mode >= controller_modes::POSITION) {
      if (!rtDW.PositionController_MODE) {
        // InitializeConditions for UnitDelay: '<S18>/UD'
        //
        //  Block description for '<S18>/UD':
        //
        //   Store in Global RAM

        rtDW.UD_DSTATE = 0.0F;
        rtDW.PositionController_MODE = true;
      }

      // SampleTimeMath: '<S18>/TSamp' incorporates:
      //   DiscreteIntegrator: '<S19>/Discrete-Time Integrator'
      //
      //  About '<S18>/TSamp':
      //   y = u * K where K = 1 / ( w * Ts )
      //
      rtb_TSamp = rtDW.DiscreteTimeIntegrator_DSTATE * 1000.0F;

      // Sum: '<S12>/Sum1' incorporates:
      //   DiscreteIntegrator: '<S19>/Discrete-Time Integrator'
      //   Gain: '<S12>/Gain'
      //   Gain: '<S12>/Gain1'
      //   Inport: '<Root>/pos_feedback'
      //   Sum: '<S12>/Sum'
      //   Sum: '<S18>/Diff'
      //   UnitDelay: '<S18>/UD'
      //
      //  Block description for '<S18>/Diff':
      //
      //   Add in CPU
      //
      //  Block description for '<S18>/UD':
      //
      //   Store in Global RAM

      u0 = (rtb_TSamp - rtDW.UD_DSTATE) * currentControllerGains.position.Kff +
        (rtDW.DiscreteTimeIntegrator_DSTATE - rtU.pos_feedback) *
        currentControllerGains.position.Kp;

      // Saturate: '<S12>/Saturation'
      if (u0 > currentControllerGains.position.SatMax) {
        // Saturate: '<S12>/Saturation'
        rtDW.Saturation_m = currentControllerGains.position.SatMax;
      } else if (u0 < currentControllerGains.position.SatMin) {
        // Saturate: '<S12>/Saturation'
        rtDW.Saturation_m = currentControllerGains.position.SatMin;
      } else {
        // Saturate: '<S12>/Saturation'
        rtDW.Saturation_m = u0;
      }

      // End of Saturate: '<S12>/Saturation'

      // Update for UnitDelay: '<S18>/UD'
      //
      //  Block description for '<S18>/UD':
      //
      //   Store in Global RAM

      rtDW.UD_DSTATE = rtb_TSamp;
    } else if (rtDW.PositionController_MODE) {
      // Disable for Saturate: '<S12>/Saturation' incorporates:
      //   Outport: '<S12>/SpeedDemand'

      rtDW.Saturation_m = 0.0F;
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

    if (controller_mode == controller_modes::SPEED) {
      rtb_TSamp = rtU.speedDemand_ext;
    } else {
      rtb_TSamp = rtDW.Saturation_m;
    }

    // End of Switch: '<S1>/Switch'

    // Gain: '<S10>/Gain1'
    rtb_Gain1 = 0.0174532924F * rtb_TSamp;

    // Outputs for Enabled SubSystem: '<S1>/Speed Controller' incorporates:
    //   EnablePort: '<S13>/Enable'

    // RelationalOperator: '<S3>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S3>/Constant'

    if (controller_mode >= controller_modes::SPEED) {
      if (!rtDW.SpeedController_MODE) {
        // InitializeConditions for DiscreteIntegrator: '<S13>/Discrete-Time Integrator' 
        rtDW.DiscreteTimeIntegrator_DSTATE_b = 0.0F;

        // InitializeConditions for RateLimiter: '<S13>/Rate Limiter'
        rtDW.PrevY = 0.0F;
        rtDW.SpeedController_MODE = true;
      }

      // RateLimiter: '<S13>/Rate Limiter'
      rtb_Sum_m = rtb_Gain1 - rtDW.PrevY;
      u0 = static_cast<real32_T>(currentControllerGains.speed.RateLimiterMax *
        period);
      if (rtb_Sum_m > u0) {
        rtb_Sum_m = u0 + rtDW.PrevY;
      } else if (rtb_Sum_m < static_cast<real32_T>
                 (currentControllerGains.speed.RateLimiterMin * period)) {
        rtb_Sum_m = static_cast<real32_T>
          (currentControllerGains.speed.RateLimiterMin * period) + rtDW.PrevY;
      } else {
        rtb_Sum_m = rtb_Gain1;
      }

      rtDW.PrevY = rtb_Sum_m;

      // End of RateLimiter: '<S13>/Rate Limiter'

      // Sum: '<S13>/Sum' incorporates:
      //   Gain: '<S11>/Gain1'
      //   Inport: '<Root>/speed_feedback'

      rtb_Sum_m -= 0.0174532924F * rtU.SpeedFeedback;

      // Sum: '<S13>/Sum1' incorporates:
      //   DiscreteIntegrator: '<S13>/Discrete-Time Integrator'
      //   Gain: '<S13>/Gain'

      rtb_Gain1 = currentControllerGains.speed.Kp * rtb_Sum_m +
        rtDW.DiscreteTimeIntegrator_DSTATE_b;

      // Saturate: '<S13>/Saturation'
      if (rtb_Gain1 > currentControllerGains.speed.SatMax) {
        // Outport: '<Root>/currentDemand'
        rtY.currentDemand = currentControllerGains.speed.SatMax;
      } else if (rtb_Gain1 < currentControllerGains.speed.SatMin) {
        // Outport: '<Root>/currentDemand'
        rtY.currentDemand = currentControllerGains.speed.SatMin;
      } else {
        // Outport: '<Root>/currentDemand'
        rtY.currentDemand = rtb_Gain1;
      }

      // End of Saturate: '<S13>/Saturation'

      // Update for DiscreteIntegrator: '<S13>/Discrete-Time Integrator' incorporates:
      //   Gain: '<S13>/Gain1'
      //   Gain: '<S13>/Gain2'
      //   Outport: '<Root>/currentDemand'
      //   Sum: '<S13>/Sum2'
      //   Sum: '<S13>/Sum3'

      rtDW.DiscreteTimeIntegrator_DSTATE_b += (currentControllerGains.speed.Ki /
        currentControllerGains.speed.Kp * (rtY.currentDemand - rtb_Gain1) +
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

      // End of Update for DiscreteIntegrator: '<S13>/Discrete-Time Integrator'
    } else if (rtDW.SpeedController_MODE) {
      // Disable for Outport: '<Root>/currentDemand' incorporates:
      //   Outport: '<S13>/Y'

      rtY.currentDemand = 0.0F;
      rtDW.SpeedController_MODE = false;
    }

    // End of RelationalOperator: '<S3>/Compare'
    // End of Outputs for SubSystem: '<S1>/Speed Controller'

    // Outport: '<Root>/speedDemand'
    rtY.speedDemand = rtb_TSamp;

    // Outport: '<Root>/pos_ref_rate_limited' incorporates:
    //   DiscreteIntegrator: '<S19>/Discrete-Time Integrator'

    rtY.pos_ref_rate_limited = rtDW.DiscreteTimeIntegrator_DSTATE;

    // Outputs for Enabled SubSystem: '<S1>/Thrust Controller' incorporates:
    //   EnablePort: '<S14>/Enable'

    // RelationalOperator: '<S8>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S8>/Constant'

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

    // End of RelationalOperator: '<S8>/Compare'
    // End of Outputs for SubSystem: '<S1>/Thrust Controller'

    // Switch: '<S1>/Switch2' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S9>/Constant'
    //   Inport: '<Root>/P_nozzle_demand'
    //   Outport: '<Root>/P_nozzle_demand1'
    //   RelationalOperator: '<S9>/Compare'

    if (controller_mode == controller_modes::PRESSURE) {
      rtb_TSamp = rtU.P_nozzle_demand;
    } else {
      rtb_TSamp = rtY.P_nozzle_demand1;
    }

    // End of Switch: '<S1>/Switch2'

    // Outputs for Enabled SubSystem: '<S1>/1D Valve Lookup Controller External Table' incorporates:
    //   EnablePort: '<S2>/Enable'

    // RelationalOperator: '<S5>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S5>/Constant'

    if (controller_mode >= controller_modes::PRESSURE) {
      // Gain: '<S2>/Gain' incorporates:
      //   Inport: '<Root>/P_manifold'

      rtb_Gain1 = 0.9F * rtU.P_manifold;

      // Saturate: '<S2>/Saturation'
      if (rtb_Gain1 > 5000.0F) {
        rtb_Gain1 = 5000.0F;
      } else if (rtb_Gain1 < 15.0F) {
        rtb_Gain1 = 15.0F;
      }

      // End of Saturate: '<S2>/Saturation'

      // Switch: '<S17>/Switch2' incorporates:
      //   Constant: '<S2>/Constant'
      //   RelationalOperator: '<S17>/LowerRelop1'
      //   RelationalOperator: '<S17>/UpperRelop'
      //   Switch: '<S17>/Switch'

      if (rtb_TSamp > rtb_Gain1) {
        rtb_TSamp = rtb_Gain1;
      } else if (rtb_TSamp < 0.0F) {
        // Switch: '<S17>/Switch' incorporates:
        //   Constant: '<S2>/Constant'

        rtb_TSamp = 0.0F;
      }

      // End of Switch: '<S17>/Switch2'

      // Saturate: '<S2>/Saturation1' incorporates:
      //   Inport: '<Root>/P_manifold'

      if (rtU.P_manifold > 5000.0F) {
        u0 = 5000.0F;
      } else if (rtU.P_manifold < 15.0F) {
        u0 = 15.0F;
      } else {
        u0 = rtU.P_manifold;
      }

      // Lookup_n-D: '<S2>/1-D Lookup Table' incorporates:
      //   Inport: '<Root>/ValveFitPressureRatios'
      //   Product: '<S2>/Divide'
      //   Saturate: '<S2>/Saturation1'

      rtb_Gain1 = look1_iflf_bingc(rtb_TSamp / u0, rtU.ValveFitPressureRatios,
        rtConstP.uDLookupTable_tableData, 10U);

      // Switch: '<S2>/Switch' incorporates:
      //   Constant: '<S16>/Constant'
      //   Constant: '<S2>/Constant1'
      //   Gain: '<S2>/Gain1'
      //   Inport: '<Root>/P_nozzle'
      //   RelationalOperator: '<S16>/Compare'
      //   Sum: '<S2>/Sum2'

      if (rtU.P_nozzle >= -500.0F) {
        u0_0 = (rtb_TSamp - rtU.P_nozzle) * currentControllerGains.pressure.Kp;
      } else {
        u0_0 = 0.0;
      }

      // Sum: '<S2>/Sum' incorporates:
      //   Switch: '<S2>/Switch'

      u0_0 += rtb_Gain1;

      // Saturate: '<S2>/Saturation3'
      if (u0_0 > 1200.0) {
        // Saturate: '<S2>/Saturation3'
        rtDW.Saturation3 = 1200.0;
      } else if (u0_0 < 0.0) {
        // Saturate: '<S2>/Saturation3'
        rtDW.Saturation3 = 0.0;
      } else {
        // Saturate: '<S2>/Saturation3'
        rtDW.Saturation3 = u0_0;
      }

      // End of Saturate: '<S2>/Saturation3'
    }

    // End of RelationalOperator: '<S5>/Compare'
    // End of Outputs for SubSystem: '<S1>/1D Valve Lookup Controller External Table' 

    // Switch: '<S1>/Switch1' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S7>/Constant'
    //   Inport: '<Root>/pos_ref_ext'
    //   RelationalOperator: '<S7>/Compare'

    if (controller_mode == controller_modes::POSITION) {
      rtb_TSamp = rtU.pos_ref_ext;
    } else {
      rtb_TSamp = static_cast<real32_T>(rtDW.Saturation3);
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

    // Gain: '<S19>/Gain' incorporates:
    //   DiscreteIntegrator: '<S19>/Discrete-Time Integrator'
    //   Saturate: '<S15>/Saturation1'
    //   Sum: '<S19>/Sum1'

    u0 = (rtb_TSamp - rtDW.DiscreteTimeIntegrator_DSTATE) * 188.49556F;

    // Saturate: '<S19>/Saturation'
    if (u0 > 2400.0F) {
      u0 = 2400.0F;
    } else if (u0 < -2400.0F) {
      u0 = -2400.0F;
    }

    // Update for DiscreteIntegrator: '<S19>/Discrete-Time Integrator' incorporates:
    //   Saturate: '<S19>/Saturation'

    rtDW.DiscreteTimeIntegrator_DSTATE += 0.001F * u0;
  }

  // Model initialize function
  void actuatorController::initialize()
  {
    // (no initialization code required)
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
