/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: metux_controller.c
 *
 * Code generated for Simulink model 'metux_controller'.
 *
 * Model version                  : 5.16
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Thu Nov 20 12:07:27 2025
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "metux_controller.h"
#include "rtwtypes.h"
#include "metux_controller_types.h"
#include <string.h>

/* Named constants for MATLAB Function: '<S3>/TPE_KALMAN_spd' */
#define metux_controller_Ts            (0.001)
#define metux_controller_period        (0.001)

/* Exported block parameters */
struct_vowNbQvkfytzerHGmKsAtG position_controller = {
  15.0,
  0.0,
  0.0,
  1.0,
  2520.0,
  2280.0,
  2280.0,
  20.0
} ;                                    /* Variable: position_controller
                                        * Referenced by:
                                        *   '<S14>/Gain'
                                        *   '<S14>/Gain1'
                                        *   '<S14>/Rate Limiter'
                                        *   '<S14>/Saturation1'
                                        */

struct_uPAe3g7S4gcBpKefqviYUB velocity_controller = {
  0.00059999999325,
  0.0078,
  0.0,
  0.000702,
  100000.0
} ;                                    /* Variable: velocity_controller
                                        * Referenced by:
                                        *   '<S8>/Gain'
                                        *   '<S8>/Gain1'
                                        *   '<S8>/Gain2'
                                        *   '<S16>/Gain'
                                        *   '<S16>/Gain1'
                                        *   '<S16>/Gain2'
                                        */

real_T enableKalmanForPositionEst = 1.0;/* Variable: enableKalmanForPositionEst
                                         * Referenced by: '<S1>/Constant3'
                                         */
real_T enableKalmanForVelocityEst = 1.0;/* Variable: enableKalmanForVelocityEst
                                         * Referenced by: '<S1>/Constant2'
                                         */

/* Block signals (default storage) */
B_metux_controller_T metux_controller_B;

/* Block states (default storage) */
DW_metux_controller_T metux_controller_DW;

/* External inputs (root inport signals with default storage) */
ExtU_metux_controller_T metux_controller_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_metux_controller_T metux_controller_Y;

/* Real-time model */
static RT_MODEL_metux_controller_T metux_controller_M_;
RT_MODEL_metux_controller_T *const metux_controller_M = &metux_controller_M_;

/* Model step function */
void metux_controller_step(void)
{
  real_T K1;
  real_T K2;
  real_T P11;
  real_T P12;
  real_T P21;
  real_T dummy;
  real_T qk_idx_0;
  real_T qk_idx_1;
  real_T rtb_RateLimiter1;
  real_T rtb_Switch1_n;
  real_T rtb_TSamp;
  real_T rtb_error_c;
  real_T rtb_yFF;
  real_T rtb_yFF_p;
  boolean_T rtb_OR1;

  /* RateLimiter: '<S1>/Rate Limiter1' incorporates:
   *  Inport: '<Root>/Motor Angle'
   */
  rtb_TSamp = metux_controller_U.MotorAngle - metux_controller_DW.PrevY;
  if (rtb_TSamp > 2.88) {
    rtb_RateLimiter1 = metux_controller_DW.PrevY + 2.88;
  } else if (rtb_TSamp < -2.88) {
    rtb_RateLimiter1 = metux_controller_DW.PrevY - 2.88;
  } else {
    rtb_RateLimiter1 = metux_controller_U.MotorAngle;
  }

  metux_controller_DW.PrevY = rtb_RateLimiter1;

  /* End of RateLimiter: '<S1>/Rate Limiter1' */

  /* MATLAB Function: '<S3>/TPE_KALMAN_spd' incorporates:
   *  Delay: '<S3>/Resettable Delay_qk'
   *  UnitDelay: '<S3>/Unit Delay_Pk'
   */
  rtb_TSamp = metux_controller_DW.ResettableDelay_qk_DSTATE[1] *
    metux_controller_Ts + metux_controller_DW.ResettableDelay_qk_DSTATE[0];
  P12 = metux_controller_Ts * metux_controller_DW.UnitDelay_Pk_DSTATE[3];
  dummy = P12 + metux_controller_DW.UnitDelay_Pk_DSTATE[1];
  P11 = ((metux_controller_Ts * metux_controller_DW.UnitDelay_Pk_DSTATE[2] +
          metux_controller_DW.UnitDelay_Pk_DSTATE[0]) + metux_controller_Ts *
         dummy) + 5.3333333333333345E-6;
  P21 = dummy + 0.008;
  P12 = (P12 + metux_controller_DW.UnitDelay_Pk_DSTATE[2]) + 0.008;
  dummy = 1.0 / (P11 + 0.0064);
  K1 = P11 * dummy;
  K2 = P21 * dummy;
  dummy = rtb_RateLimiter1 - rtb_TSamp;
  qk_idx_0 = K1 * dummy + rtb_TSamp;
  qk_idx_1 = K2 * dummy + metux_controller_DW.ResettableDelay_qk_DSTATE[1];

  /* Outputs for Enabled SubSystem: '<S1>/motor position controller' incorporates:
   *  EnablePort: '<S5>/Enable'
   */
  /* Inport: '<Root>/Activate' */
  if (metux_controller_U.Activate) {
    metux_controller_DW.motorpositioncontroller_MODE = true;

    /* Saturate: '<S14>/Saturation' incorporates:
     *  Inport: '<Root>/motor_pos_demand_ext'
     */
    if (metux_controller_U.motor_pos_demand_ext > 1440.0) {
      dummy = 1440.0;
    } else if (metux_controller_U.motor_pos_demand_ext < -10.0) {
      dummy = -10.0;
    } else {
      dummy = metux_controller_U.motor_pos_demand_ext;
    }

    /* End of Saturate: '<S14>/Saturation' */

    /* RateLimiter: '<S14>/Rate Limiter' */
    rtb_TSamp = dummy - metux_controller_DW.PrevY_d;
    rtb_Switch1_n = position_controller.rateLimiter * metux_controller_period;
    if (rtb_TSamp > rtb_Switch1_n) {
      dummy = rtb_Switch1_n + metux_controller_DW.PrevY_d;
    } else if (rtb_TSamp < -position_controller.rateLimiter *
               metux_controller_period) {
      dummy = -position_controller.rateLimiter * metux_controller_period +
        metux_controller_DW.PrevY_d;
    }

    metux_controller_DW.PrevY_d = dummy;

    /* End of RateLimiter: '<S14>/Rate Limiter' */

    /* SampleTimeMath: '<S15>/TSamp'
     *
     * About '<S15>/TSamp':
     *  y = u * K where K = 1 / ( w * Ts )
     *   */
    rtb_TSamp = dummy * 1000.0;

    /* Switch: '<S1>/Switch2' incorporates:
     *  Constant: '<S1>/Constant3'
     *  MATLAB Function: '<S3>/TPE_KALMAN_spd'
     */
    if (enableKalmanForPositionEst > 0.0) {
      rtb_Switch1_n = qk_idx_0;
    } else {
      rtb_Switch1_n = rtb_RateLimiter1;
    }

    /* Sum: '<S14>/Sum1' incorporates:
     *  Gain: '<S14>/Gain'
     *  Gain: '<S14>/Gain1'
     *  Sum: '<S14>/Sum'
     *  Sum: '<S15>/Diff'
     *  Switch: '<S1>/Switch2'
     *  UnitDelay: '<S15>/UD'
     *
     * Block description for '<S15>/Diff':
     *
     *  Add in CPU
     *
     * Block description for '<S15>/UD':
     *
     *  Store in Global RAM
     */
    rtb_yFF_p = (rtb_TSamp - metux_controller_DW.UD_DSTATE) *
      position_controller.kFF + (dummy - rtb_Switch1_n) * position_controller.kP;

    /* Saturate: '<S14>/Saturation1' */
    if (rtb_yFF_p > position_controller.saturation) {
      /* Saturate: '<S14>/Saturation1' */
      metux_controller_B.yOut_j = position_controller.saturation;
    } else if (rtb_yFF_p < -position_controller.saturation) {
      /* Saturate: '<S14>/Saturation1' */
      metux_controller_B.yOut_j = -position_controller.saturation;
    } else {
      /* Saturate: '<S14>/Saturation1' */
      metux_controller_B.yOut_j = rtb_yFF_p;
    }

    /* End of Saturate: '<S14>/Saturation1' */

    /* Update for UnitDelay: '<S15>/UD'
     *
     * Block description for '<S15>/UD':
     *
     *  Store in Global RAM
     */
    metux_controller_DW.UD_DSTATE = rtb_TSamp;
  } else if (metux_controller_DW.motorpositioncontroller_MODE) {
    /* Disable for Saturate: '<S14>/Saturation1' incorporates:
     *  Outport: '<S5>/motor_speed_demand'
     */
    metux_controller_B.yOut_j = 0.0;
    metux_controller_DW.motorpositioncontroller_MODE = false;
  }

  /* End of Outputs for SubSystem: '<S1>/motor position controller' */

  /* Switch: '<S1>/Switch4' incorporates:
   *  Inport: '<Root>/Controller Mode'
   *  Inport: '<Root>/motor_speed_demand_ext'
   */
  if (metux_controller_U.ControllerMode >= 4) {
    dummy = metux_controller_B.yOut_j;
  } else {
    dummy = metux_controller_U.motor_speed_demand_ext;
  }

  /* End of Switch: '<S1>/Switch4' */

  /* Switch: '<S1>/Switch1' incorporates:
   *  Constant: '<S1>/Constant2'
   *  Delay: '<S1>/Delay One Step1'
   *  Gain: '<S1>/Gain1'
   *  MATLAB Function: '<S3>/TPE_KALMAN_spd'
   *  Sum: '<S1>/Sum2'
   */
  if (enableKalmanForVelocityEst > 0.0) {
    rtb_TSamp = qk_idx_1;
  } else {
    rtb_TSamp = (rtb_RateLimiter1 - metux_controller_DW.DelayOneStep1_DSTATE[0])
      * 500.0;
  }

  /* End of Switch: '<S1>/Switch1' */

  /* Outputs for Enabled SubSystem: '<S1>/motor speed controller' incorporates:
   *  EnablePort: '<S6>/Enable'
   */
  /* Inport: '<Root>/Activate' */
  if (metux_controller_U.Activate) {
    metux_controller_DW.motorspeedcontroller_MODE = true;

    /* Switch: '<S21>/Switch1' incorporates:
     *  Constant: '<S18>/Constant'
     *  Constant: '<S19>/Constant'
     *  Constant: '<S21>/Constant'
     *  Constant: '<S21>/Constant1'
     *  Delay: '<S21>/Delay'
     *  Logic: '<S16>/OR'
     *  RelationalOperator: '<S18>/Compare'
     *  RelationalOperator: '<S19>/Compare'
     *  Sum: '<S21>/Sum'
     */
    if ((rtb_TSamp >= 5.0) || (rtb_TSamp <= -5.0)) {
      rtb_Switch1_n = 0.0;
    } else {
      rtb_Switch1_n = metux_controller_DW.Delay_DSTATE + 1.0;
    }

    /* End of Switch: '<S21>/Switch1' */

    /* Logic: '<S16>/OR1' incorporates:
     *  Constant: '<S20>/Constant'
     *  Delay: '<S16>/Delay One Step'
     *  RelationalOperator: '<S20>/Compare'
     */
    rtb_OR1 = ((rtb_Switch1_n >= 50.0) &&
               metux_controller_DW.DelayOneStep_DSTATE);

    /* DiscreteIntegrator: '<S16>/Discrete-Time Integrator' */
    if (rtb_OR1 && (metux_controller_DW.DiscreteTimeIntegrator_PrevRese <= 0)) {
      metux_controller_DW.DiscreteTimeIntegrator_DSTATE = 0.0;
    }

    /* Saturate: '<S16>/Saturation' */
    if (dummy > 2880.0) {
      rtb_error_c = 2880.0;
    } else if (dummy < -2880.0) {
      rtb_error_c = -2880.0;
    } else {
      rtb_error_c = dummy;
    }

    /* End of Saturate: '<S16>/Saturation' */

    /* Gain: '<S16>/Gain' */
    rtb_yFF = velocity_controller.kFF * rtb_error_c;

    /* Sum: '<S16>/Sum' */
    rtb_error_c -= rtb_TSamp;

    /* Sum: '<S16>/Sum1' incorporates:
     *  DiscreteIntegrator: '<S16>/Discrete-Time Integrator'
     *  Gain: '<S16>/Gain1'
     */
    rtb_yFF_p = (velocity_controller.kP * rtb_error_c + rtb_yFF) +
      metux_controller_DW.DiscreteTimeIntegrator_DSTATE;

    /* Saturate: '<S16>/Saturation1' */
    if (rtb_yFF_p > 1.0) {
      /* Saturate: '<S16>/Saturation1' */
      metux_controller_B.yOut = 1.0;
    } else if (rtb_yFF_p < -1.0) {
      /* Saturate: '<S16>/Saturation1' */
      metux_controller_B.yOut = -1.0;
    } else {
      /* Saturate: '<S16>/Saturation1' */
      metux_controller_B.yOut = rtb_yFF_p;
    }

    /* End of Saturate: '<S16>/Saturation1' */

    /* Update for Delay: '<S21>/Delay' */
    metux_controller_DW.Delay_DSTATE = rtb_Switch1_n;

    /* Update for Delay: '<S16>/Delay One Step' incorporates:
     *  Constant: '<S17>/Constant'
     *  DiscreteIntegrator: '<S16>/Discrete-Time Integrator'
     *  Product: '<S16>/Product'
     *  RelationalOperator: '<S17>/Compare'
     */
    metux_controller_DW.DelayOneStep_DSTATE = (dummy *
      metux_controller_DW.DiscreteTimeIntegrator_DSTATE < 0.0);

    /* Update for DiscreteIntegrator: '<S16>/Discrete-Time Integrator' incorporates:
     *  Gain: '<S16>/Gain2'
     */
    metux_controller_DW.DiscreteTimeIntegrator_DSTATE += velocity_controller.kI *
      rtb_error_c * 0.001;
    if (metux_controller_DW.DiscreteTimeIntegrator_DSTATE > 1.0) {
      metux_controller_DW.DiscreteTimeIntegrator_DSTATE = 1.0;
    } else if (metux_controller_DW.DiscreteTimeIntegrator_DSTATE < -1.0) {
      metux_controller_DW.DiscreteTimeIntegrator_DSTATE = -1.0;
    }

    metux_controller_DW.DiscreteTimeIntegrator_PrevRese = (int8_T)rtb_OR1;

    /* End of Update for DiscreteIntegrator: '<S16>/Discrete-Time Integrator' */
  } else if (metux_controller_DW.motorspeedcontroller_MODE) {
    /* Disable for Saturate: '<S16>/Saturation1' incorporates:
     *  Outport: '<S6>/CurrentDemand'
     */
    metux_controller_B.yOut = 0.0;
    metux_controller_DW.motorspeedcontroller_MODE = false;
  }

  /* End of Outputs for SubSystem: '<S1>/motor speed controller' */

  /* Switch: '<S1>/Switch3' incorporates:
   *  Inport: '<Root>/Controller Mode'
   *  Inport: '<Root>/current_demand_ext'
   */
  if (metux_controller_U.ControllerMode >= 3) {
    rtb_Switch1_n = metux_controller_B.yOut;
  } else {
    rtb_Switch1_n = metux_controller_U.current_demand_ext;
  }

  /* End of Switch: '<S1>/Switch3' */

  /* Outputs for Enabled SubSystem: '<S1>/motor current controller' incorporates:
   *  EnablePort: '<S4>/Enable'
   */
  /* Inport: '<Root>/Activate' */
  if (metux_controller_U.Activate) {
    metux_controller_DW.motorcurrentcontroller_MODE = true;

    /* Switch: '<S13>/Switch1' incorporates:
     *  Constant: '<S10>/Constant'
     *  Constant: '<S11>/Constant'
     *  Constant: '<S13>/Constant'
     *  Constant: '<S13>/Constant1'
     *  Delay: '<S13>/Delay'
     *  Inport: '<Root>/current_measured'
     *  Logic: '<S8>/OR'
     *  RelationalOperator: '<S10>/Compare'
     *  RelationalOperator: '<S11>/Compare'
     *  Sum: '<S13>/Sum'
     */
    if ((metux_controller_U.current_measured >= 5.0) ||
        (metux_controller_U.current_measured <= -5.0)) {
      rtb_yFF = 0.0;
    } else {
      rtb_yFF = metux_controller_DW.Delay_DSTATE_p + 1.0;
    }

    /* End of Switch: '<S13>/Switch1' */

    /* Logic: '<S8>/OR1' incorporates:
     *  Constant: '<S12>/Constant'
     *  Delay: '<S8>/Delay One Step'
     *  RelationalOperator: '<S12>/Compare'
     */
    rtb_OR1 = ((rtb_yFF >= 50.0) && metux_controller_DW.DelayOneStep_DSTATE_l);

    /* DiscreteIntegrator: '<S8>/Discrete-Time Integrator' */
    if (rtb_OR1 && (metux_controller_DW.DiscreteTimeIntegrator_PrevRe_k <= 0)) {
      metux_controller_DW.DiscreteTimeIntegrator_DSTATE_f = 0.0;
    }

    /* Saturate: '<S8>/Saturation' */
    if (rtb_Switch1_n > 2880.0) {
      rtb_error_c = 2880.0;
    } else if (rtb_Switch1_n < -2880.0) {
      rtb_error_c = -2880.0;
    } else {
      rtb_error_c = rtb_Switch1_n;
    }

    /* End of Saturate: '<S8>/Saturation' */

    /* Gain: '<S8>/Gain' */
    rtb_yFF_p = velocity_controller.kFF * rtb_error_c;

    /* Sum: '<S8>/Sum' incorporates:
     *  Inport: '<Root>/current_measured'
     */
    rtb_error_c -= metux_controller_U.current_measured;

    /* Sum: '<S8>/Sum1' incorporates:
     *  DiscreteIntegrator: '<S8>/Discrete-Time Integrator'
     *  Gain: '<S8>/Gain1'
     */
    rtb_yFF_p = (velocity_controller.kP * rtb_error_c + rtb_yFF_p) +
      metux_controller_DW.DiscreteTimeIntegrator_DSTATE_f;

    /* Saturate: '<S8>/Saturation1' */
    if (rtb_yFF_p > 1.0) {
      /* Saturate: '<S8>/Saturation1' */
      metux_controller_B.yOut_g = 1.0;
    } else if (rtb_yFF_p < -1.0) {
      /* Saturate: '<S8>/Saturation1' */
      metux_controller_B.yOut_g = -1.0;
    } else {
      /* Saturate: '<S8>/Saturation1' */
      metux_controller_B.yOut_g = rtb_yFF_p;
    }

    /* End of Saturate: '<S8>/Saturation1' */

    /* Update for Delay: '<S13>/Delay' */
    metux_controller_DW.Delay_DSTATE_p = rtb_yFF;

    /* Update for Delay: '<S8>/Delay One Step' incorporates:
     *  Constant: '<S9>/Constant'
     *  DiscreteIntegrator: '<S8>/Discrete-Time Integrator'
     *  Product: '<S8>/Product'
     *  RelationalOperator: '<S9>/Compare'
     */
    metux_controller_DW.DelayOneStep_DSTATE_l = (rtb_Switch1_n *
      metux_controller_DW.DiscreteTimeIntegrator_DSTATE_f < 0.0);

    /* Update for DiscreteIntegrator: '<S8>/Discrete-Time Integrator' incorporates:
     *  Gain: '<S8>/Gain2'
     */
    metux_controller_DW.DiscreteTimeIntegrator_DSTATE_f +=
      velocity_controller.kI * rtb_error_c * 0.001;
    if (metux_controller_DW.DiscreteTimeIntegrator_DSTATE_f > 1.0) {
      metux_controller_DW.DiscreteTimeIntegrator_DSTATE_f = 1.0;
    } else if (metux_controller_DW.DiscreteTimeIntegrator_DSTATE_f < -1.0) {
      metux_controller_DW.DiscreteTimeIntegrator_DSTATE_f = -1.0;
    }

    metux_controller_DW.DiscreteTimeIntegrator_PrevRe_k = (int8_T)rtb_OR1;

    /* End of Update for DiscreteIntegrator: '<S8>/Discrete-Time Integrator' */
  } else if (metux_controller_DW.motorcurrentcontroller_MODE) {
    /* Disable for Saturate: '<S8>/Saturation1' incorporates:
     *  Outport: '<S4>/Duty'
     */
    metux_controller_B.yOut_g = 0.0;
    metux_controller_DW.motorcurrentcontroller_MODE = false;
  }

  /* End of Outputs for SubSystem: '<S1>/motor current controller' */

  /* Outputs for Enabled SubSystem: '<S1>/E-STOP Override' incorporates:
   *  EnablePort: '<S2>/Enable'
   */
  /* Logic: '<S1>/NOT' incorporates:
   *  Inport: '<Root>/E-STOP'
   */
  if (!metux_controller_U.ESTOP) {
    metux_controller_DW.ESTOPOverride_MODE = true;

    /* Outport: '<Root>/Motor Duty' incorporates:
     *  Inport: '<Root>/Activate'
     *  Inport: '<Root>/Controller Mode'
     *  Inport: '<Root>/MotorsEnabled'
     *  Inport: '<Root>/current_demand_ext'
     *  Product: '<S1>/Product'
     *  Product: '<S2>/Product'
     *  Switch: '<S1>/Switch6'
     */
    metux_controller_Y.MotorDuty = metux_controller_U.MotorsEnabled ?
      metux_controller_U.Activate ? metux_controller_U.ControllerMode >= 2 ?
      metux_controller_B.yOut_g : metux_controller_U.current_demand_ext : 0.0 :
      0.0;
  } else if (metux_controller_DW.ESTOPOverride_MODE) {
    /* Disable for Outport: '<Root>/Motor Duty' incorporates:
     *  Outport: '<S2>/Duty Out'
     */
    metux_controller_Y.MotorDuty = 0.0;
    metux_controller_DW.ESTOPOverride_MODE = false;
  }

  /* End of Logic: '<S1>/NOT' */
  /* End of Outputs for SubSystem: '<S1>/E-STOP Override' */

  /* Outport: '<Root>/current_demand' */
  metux_controller_Y.current_demand = rtb_Switch1_n;

  /* Outport: '<Root>/motor_speed' */
  metux_controller_Y.motor_speed = rtb_TSamp;

  /* Outport: '<Root>/motor_speed_demand' */
  metux_controller_Y.motor_speed_demand = dummy;

  /* Outport: '<Root>/motor_position_kalman' incorporates:
   *  MATLAB Function: '<S3>/TPE_KALMAN_spd'
   */
  metux_controller_Y.motor_position_kalman = qk_idx_0;

  /* Outport: '<Root>/motor_position_demand' incorporates:
   *  Inport: '<Root>/motor_pos_demand_ext'
   */
  metux_controller_Y.motor_position_demand =
    metux_controller_U.motor_pos_demand_ext;

  /* MATLAB Function: '<S3>/TPE_KALMAN_spd' incorporates:
   *  UnitDelay: '<S3>/Unit Delay_Pk'
   */
  rtb_Switch1_n = metux_controller_DW.UnitDelay_Pk_DSTATE[3];

  /* Update for UnitDelay: '<S3>/Unit Delay_Pk' incorporates:
   *  MATLAB Function: '<S3>/TPE_KALMAN_spd'
   */
  metux_controller_DW.UnitDelay_Pk_DSTATE[0] = P11 - K1 * P11;
  metux_controller_DW.UnitDelay_Pk_DSTATE[2] = P12 - K1 * P12;
  metux_controller_DW.UnitDelay_Pk_DSTATE[1] = P21 - K2 * P11;
  metux_controller_DW.UnitDelay_Pk_DSTATE[3] = (rtb_Switch1_n + 16.0) - K2 * P12;

  /* Update for Delay: '<S3>/Resettable Delay_qk' incorporates:
   *  MATLAB Function: '<S3>/TPE_KALMAN_spd'
   */
  metux_controller_DW.ResettableDelay_qk_DSTATE[0] = qk_idx_0;
  metux_controller_DW.ResettableDelay_qk_DSTATE[1] = qk_idx_1;

  /* Update for Delay: '<S1>/Delay One Step1' */
  metux_controller_DW.DelayOneStep1_DSTATE[0] =
    metux_controller_DW.DelayOneStep1_DSTATE[1];
  metux_controller_DW.DelayOneStep1_DSTATE[1] = rtb_RateLimiter1;
}

/* Model initialize function */
void metux_controller_initialize(void)
{
  /* Registration code */

  /* initialize error status */
  rtmSetErrorStatus(metux_controller_M, (NULL));

  /* block I/O */
  (void) memset(((void *) &metux_controller_B), 0,
                sizeof(B_metux_controller_T));

  /* states (dwork) */
  (void) memset((void *)&metux_controller_DW, 0,
                sizeof(DW_metux_controller_T));

  /* external inputs */
  (void)memset(&metux_controller_U, 0, sizeof(ExtU_metux_controller_T));

  /* external outputs */
  (void)memset(&metux_controller_Y, 0, sizeof(ExtY_metux_controller_T));

  /* InitializeConditions for UnitDelay: '<S3>/Unit Delay_Pk' */
  metux_controller_DW.UnitDelay_Pk_DSTATE[0] = 0.155;
  metux_controller_DW.UnitDelay_Pk_DSTATE[1] = 65.0;
  metux_controller_DW.UnitDelay_Pk_DSTATE[2] = 65.0;
  metux_controller_DW.UnitDelay_Pk_DSTATE[3] = 57000.0;

  /* InitializeConditions for RateLimiter: '<S1>/Rate Limiter1' */
  metux_controller_DW.PrevY = 0.0;

  /* SystemInitialize for Enabled SubSystem: '<S1>/motor position controller' */
  /* InitializeConditions for RateLimiter: '<S14>/Rate Limiter' */
  metux_controller_DW.PrevY_d = 0.0;

  /* End of SystemInitialize for SubSystem: '<S1>/motor position controller' */

  /* SystemInitialize for Enabled SubSystem: '<S1>/motor speed controller' */
  /* InitializeConditions for DiscreteIntegrator: '<S16>/Discrete-Time Integrator' */
  metux_controller_DW.DiscreteTimeIntegrator_PrevRese = 2;

  /* End of SystemInitialize for SubSystem: '<S1>/motor speed controller' */

  /* SystemInitialize for Enabled SubSystem: '<S1>/motor current controller' */
  /* InitializeConditions for DiscreteIntegrator: '<S8>/Discrete-Time Integrator' */
  metux_controller_DW.DiscreteTimeIntegrator_PrevRe_k = 2;

  /* End of SystemInitialize for SubSystem: '<S1>/motor current controller' */

  /* ConstCode for Outport: '<Root>/mass_estimation' incorporates:
   *  Constant: '<S1>/Constant5'
   */
  metux_controller_Y.mass_estimation = 0.0;

  /* ConstCode for Outport: '<Root>/pressure_demand' incorporates:
   *  Constant: '<S1>/Constant4'
   */
  metux_controller_Y.pressure_demand = 0.0;

  /* ConstCode for Outport: '<Root>/force_feedback' incorporates:
   *  Constant: '<S1>/Constant1'
   */
  metux_controller_Y.force_feedback = 0.0;

  /* ConstCode for Outport: '<Root>/thrust_demand' incorporates:
   *  Constant: '<S1>/Constant'
   */
  metux_controller_Y.thrust_demand = 0.0;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
