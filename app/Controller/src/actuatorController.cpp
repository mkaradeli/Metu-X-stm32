//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: actuatorController.cpp
//
// Code generated for Simulink model 'actuatorController'.
//
// Model version                  : 1.50
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Fri Aug 14 20:13:34 2026
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


static real32_T look1_iflf_bingc(real32_T u0, const real32_T bp0[], const
  real32_T table[], uint32_T maxIndex);
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
    real32_T rtb_Divide;
    real32_T rtb_Gain1;
    real32_T rtb_TSamp;
    real32_T tmp;

    // Outputs for Enabled SubSystem: '<S1>/Position Controller' incorporates:
    //   EnablePort: '<S11>/Enable'

    // RelationalOperator: '<S4>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S4>/Constant'

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
      //   DiscreteIntegrator: '<S18>/Discrete-Time Integrator'
      //
      //  About '<S16>/TSamp':
      //   y = u * K where K = 1 / ( w * Ts )
      //
      rtb_TSamp = rtDW.DiscreteTimeIntegrator_DSTATE * 1000.0F;

      // Sum: '<S11>/Sum1' incorporates:
      //   DiscreteIntegrator: '<S18>/Discrete-Time Integrator'
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

      rtb_Gain1 = (rtb_TSamp - rtDW.UD_DSTATE) *
        currentControllerGains.position.Kff +
        (rtDW.DiscreteTimeIntegrator_DSTATE - rtU.pos_feedback) *
        currentControllerGains.position.Kp;

      // Saturate: '<S11>/Saturation'
      if (rtb_Gain1 > currentControllerGains.position.SatMax) {
        // Saturate: '<S11>/Saturation'
        rtDW.Saturation_m = currentControllerGains.position.SatMax;
      } else if (rtb_Gain1 < currentControllerGains.position.SatMin) {
        // Saturate: '<S11>/Saturation'
        rtDW.Saturation_m = currentControllerGains.position.SatMin;
      } else {
        // Saturate: '<S11>/Saturation'
        rtDW.Saturation_m = rtb_Gain1;
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

    // Gain: '<S9>/Gain1'
    rtb_Gain1 = 0.0174532924F * rtb_TSamp;

    // Outputs for Enabled SubSystem: '<S1>/Speed Controller' incorporates:
    //   EnablePort: '<S12>/Enable'

    // RelationalOperator: '<S3>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S3>/Constant'

    if (controller_mode >= controller_modes::SPEED) {
      if (!rtDW.SpeedController_MODE) {
        // InitializeConditions for DiscreteIntegrator: '<S12>/Discrete-Time Integrator' 
        rtDW.DiscreteTimeIntegrator_DSTATE_b = 0.0F;

        // InitializeConditions for RateLimiter: '<S12>/Rate Limiter'
        rtDW.PrevY = 0.0F;
        rtDW.SpeedController_MODE = true;
      }

      // RateLimiter: '<S12>/Rate Limiter'
      rtb_Divide = rtb_Gain1 - rtDW.PrevY;
      tmp = static_cast<real32_T>(currentControllerGains.speed.RateLimiterMax *
        period);
      if (rtb_Divide > tmp) {
        rtb_Gain1 = tmp + rtDW.PrevY;
      } else if (rtb_Divide < static_cast<real32_T>
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

      rtb_Divide = currentControllerGains.speed.Kp * rtb_Gain1 +
        rtDW.DiscreteTimeIntegrator_DSTATE_b;

      // Saturate: '<S12>/Saturation'
      if (rtb_Divide > currentControllerGains.speed.SatMax) {
        // Outport: '<Root>/currentDemand'
        rtY.currentDemand = currentControllerGains.speed.SatMax;
      } else if (rtb_Divide < currentControllerGains.speed.SatMin) {
        // Outport: '<Root>/currentDemand'
        rtY.currentDemand = currentControllerGains.speed.SatMin;
      } else {
        // Outport: '<Root>/currentDemand'
        rtY.currentDemand = rtb_Divide;
      }

      // End of Saturate: '<S12>/Saturation'

      // Update for DiscreteIntegrator: '<S12>/Discrete-Time Integrator' incorporates:
      //   Gain: '<S12>/Gain1'
      //   Gain: '<S12>/Gain2'
      //   Outport: '<Root>/currentDemand'
      //   Sum: '<S12>/Sum2'
      //   Sum: '<S12>/Sum3'

      rtDW.DiscreteTimeIntegrator_DSTATE_b += (currentControllerGains.speed.Ki /
        currentControllerGains.speed.Kp * (rtY.currentDemand - rtb_Divide) +
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

    // End of RelationalOperator: '<S3>/Compare'
    // End of Outputs for SubSystem: '<S1>/Speed Controller'

    // Outport: '<Root>/speedDemand'
    rtY.speedDemand = rtb_TSamp;

    // Outport: '<Root>/pos_ref_rate_limited' incorporates:
    //   DiscreteIntegrator: '<S18>/Discrete-Time Integrator'

    rtY.pos_ref_rate_limited = rtDW.DiscreteTimeIntegrator_DSTATE;

    // Outputs for Enabled SubSystem: '<S1>/1D Valve Lookup Controller External Table' incorporates:
    //   EnablePort: '<S2>/Enable'

    // RelationalOperator: '<S5>/Compare' incorporates:
    //   Constant: '<S1>/controller_mode'
    //   Constant: '<S5>/Constant'

    if (controller_mode >= controller_modes::PRESSURE) {
      // Gain: '<S2>/Gain' incorporates:
      //   Inport: '<Root>/P_manifold'

      rtb_Divide = 0.9F * rtU.P_manifold;

      // Saturate: '<S2>/Saturation'
      if (rtb_Divide > 5000.0F) {
        rtb_Divide = 5000.0F;
      } else if (rtb_Divide < 15.0F) {
        rtb_Divide = 15.0F;
      }

      // End of Saturate: '<S2>/Saturation'

      // Switch: '<S15>/Switch2' incorporates:
      //   Inport: '<Root>/P_nozzle_demand'
      //   RelationalOperator: '<S15>/LowerRelop1'

      if (!(rtU.P_nozzle_demand > rtb_Divide)) {
        // Switch: '<S15>/Switch' incorporates:
        //   Constant: '<S2>/Constant'
        //   RelationalOperator: '<S15>/UpperRelop'

        if (rtU.P_nozzle_demand < 0.0F) {
          rtb_Divide = 0.0F;
        } else {
          rtb_Divide = rtU.P_nozzle_demand;
        }

        // End of Switch: '<S15>/Switch'
      }

      // End of Switch: '<S15>/Switch2'

      // Saturate: '<S2>/Saturation1' incorporates:
      //   Inport: '<Root>/P_manifold'

      if (rtU.P_manifold > 5000.0F) {
        tmp = 5000.0F;
      } else if (rtU.P_manifold < 15.0F) {
        tmp = 15.0F;
      } else {
        tmp = rtU.P_manifold;
      }

      // Lookup_n-D: '<S2>/1-D Lookup Table' incorporates:
      //   Inport: '<Root>/ValveFitPressureRatios'
      //   Product: '<S2>/Divide'
      //   Saturate: '<S2>/Saturation1'

      rtDW.uDLookupTable = look1_iflf_bingc(rtb_Divide / tmp,
        rtU.ValveFitPressureRatios, rtConstP.uDLookupTable_tableData, 11U);
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
      rtb_TSamp = rtDW.uDLookupTable;
    }

    // End of Switch: '<S1>/Switch1'

    // Outport: '<Root>/position_demand'
    rtY.position_demand = rtb_TSamp;

    // Saturate: '<S14>/Saturation1'
    if (rtb_TSamp > 1440.0F) {
      rtb_TSamp = 1440.0F;
    } else if (rtb_TSamp < 0.0F) {
      rtb_TSamp = 0.0F;
    }

    // Gain: '<S18>/Gain' incorporates:
    //   DiscreteIntegrator: '<S18>/Discrete-Time Integrator'
    //   Saturate: '<S14>/Saturation1'
    //   Sum: '<S18>/Sum1'

    rtb_Gain1 = (rtb_TSamp - rtDW.DiscreteTimeIntegrator_DSTATE) * 188.49556F;

    // Saturate: '<S18>/Saturation'
    if (rtb_Gain1 > 2400.0F) {
      rtb_Gain1 = 2400.0F;
    } else if (rtb_Gain1 < -2400.0F) {
      rtb_Gain1 = -2400.0F;
    }

    // Update for DiscreteIntegrator: '<S18>/Discrete-Time Integrator' incorporates:
    //   Saturate: '<S18>/Saturation'

    rtDW.DiscreteTimeIntegrator_DSTATE += 0.001F * rtb_Gain1;
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

//
// File trailer for generated code.
//
// [EOF]
//
