/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: metux_controller.h
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

#ifndef metux_controller_h_
#define metux_controller_h_
#ifndef metux_controller_COMMON_INCLUDES_
#define metux_controller_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* metux_controller_COMMON_INCLUDES_ */

#include "metux_controller_types.h"
#include <stddef.h>
#include <string.h>

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Block signals (default storage) */
typedef struct {
  real_T yOut;                         /* '<S16>/Saturation1' */
  real_T yOut_j;                       /* '<S14>/Saturation1' */
  real_T yOut_g;                       /* '<S8>/Saturation1' */
} B_metux_controller_T;

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real_T UnitDelay_Pk_DSTATE[4];       /* '<S3>/Unit Delay_Pk' */
  real_T ResettableDelay_qk_DSTATE[2]; /* '<S3>/Resettable Delay_qk' */
  real_T DelayOneStep1_DSTATE[2];      /* '<S1>/Delay One Step1' */
  real_T Delay_DSTATE;                 /* '<S21>/Delay' */
  real_T DiscreteTimeIntegrator_DSTATE;/* '<S16>/Discrete-Time Integrator' */
  real_T UD_DSTATE;                    /* '<S15>/UD' */
  real_T Delay_DSTATE_p;               /* '<S13>/Delay' */
  real_T DiscreteTimeIntegrator_DSTATE_f;/* '<S8>/Discrete-Time Integrator' */
  real_T PrevY;                        /* '<S1>/Rate Limiter1' */
  real_T PrevY_d;                      /* '<S14>/Rate Limiter' */
  boolean_T DelayOneStep_DSTATE;       /* '<S16>/Delay One Step' */
  boolean_T DelayOneStep_DSTATE_l;     /* '<S8>/Delay One Step' */
  int8_T DiscreteTimeIntegrator_PrevRese;/* '<S16>/Discrete-Time Integrator' */
  int8_T DiscreteTimeIntegrator_PrevRe_k;/* '<S8>/Discrete-Time Integrator' */
  boolean_T motorspeedcontroller_MODE; /* '<S1>/motor speed controller' */
  boolean_T motorpositioncontroller_MODE;/* '<S1>/motor position controller' */
  boolean_T motorcurrentcontroller_MODE;/* '<S1>/motor current controller' */
  boolean_T ESTOPOverride_MODE;        /* '<S1>/E-STOP Override' */
} DW_metux_controller_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  boolean_T MotorsEnabled;             /* '<Root>/MotorsEnabled' */
  boolean_T ESTOP;                     /* '<Root>/E-STOP' */
  uint8_T ControllerMode;              /* '<Root>/Controller Mode' */
  boolean_T Activate;                  /* '<Root>/Activate' */
  real_T current_measured;             /* '<Root>/current_measured' */
  real_T MotorAngle;                   /* '<Root>/Motor Angle' */
  real_T pressure_manifold;            /* '<Root>/pressure_manifold' */
  real_T pressure_nozzle;              /* '<Root>/pressure_nozzle' */
  real_T current_demand_ext;           /* '<Root>/current_demand_ext' */
  real_T motor_speed_demand_ext;       /* '<Root>/motor_speed_demand_ext' */
  real_T motor_pos_demand_ext;         /* '<Root>/motor_pos_demand_ext' */
  real_T pressure_demand_ext;          /* '<Root>/pressure_demand_ext' */
  real_T thrust_demand_ext;            /* '<Root>/thrust_demand_ext' */
} ExtU_metux_controller_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T MotorDuty;                    /* '<Root>/Motor Duty' */
  real_T current_demand;               /* '<Root>/current_demand' */
  real_T motor_speed;                  /* '<Root>/motor_speed' */
  real_T motor_speed_demand;           /* '<Root>/motor_speed_demand' */
  real_T motor_position_kalman;        /* '<Root>/motor_position_kalman' */
  real_T motor_position_demand;        /* '<Root>/motor_position_demand' */
  real_T mass_estimation;              /* '<Root>/mass_estimation' */
  real_T pressure_demand;              /* '<Root>/pressure_demand' */
  real_T force_feedback;               /* '<Root>/force_feedback' */
  real_T thrust_demand;                /* '<Root>/thrust_demand' */
} ExtY_metux_controller_T;

/* Real-time Model Data Structure */
struct tag_RTM_metux_controller_T {
  const char_T * volatile errorStatus;
};

/* Block signals (default storage) */
extern B_metux_controller_T metux_controller_B;

/* Block states (default storage) */
extern DW_metux_controller_T metux_controller_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_metux_controller_T metux_controller_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_metux_controller_T metux_controller_Y;

/*
 * Exported Global Parameters
 *
 * Note: Exported global parameters are tunable parameters with an exported
 * global storage class designation.  Code generation will declare the memory for
 * these parameters and exports their symbols.
 *
 */
extern struct_vowNbQvkfytzerHGmKsAtG position_controller;/* Variable: position_controller
                                                          * Referenced by:
                                                          *   '<S14>/Gain'
                                                          *   '<S14>/Gain1'
                                                          *   '<S14>/Rate Limiter'
                                                          *   '<S14>/Saturation1'
                                                          */
extern struct_uPAe3g7S4gcBpKefqviYUB velocity_controller;/* Variable: velocity_controller
                                                          * Referenced by:
                                                          *   '<S8>/Gain'
                                                          *   '<S8>/Gain1'
                                                          *   '<S8>/Gain2'
                                                          *   '<S16>/Gain'
                                                          *   '<S16>/Gain1'
                                                          *   '<S16>/Gain2'
                                                          */
extern real_T enableKalmanForPositionEst;/* Variable: enableKalmanForPositionEst
                                          * Referenced by: '<S1>/Constant3'
                                          */
extern real_T enableKalmanForVelocityEst;/* Variable: enableKalmanForVelocityEst
                                          * Referenced by: '<S1>/Constant2'
                                          */

/* Model entry point functions */
extern void metux_controller_initialize(void);
extern void metux_controller_step(void);

/* Real-time Model object */
extern RT_MODEL_metux_controller_T *const metux_controller_M;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S15>/Data Type Duplicate' : Unused code path elimination
 * Block '<S1>/Cast To Boolean1' : Eliminate redundant data type conversion
 * Block '<S1>/Cast To Boolean2' : Eliminate redundant data type conversion
 * Block '<S1>/Cast To Boolean3' : Eliminate redundant data type conversion
 * Block '<S1>/Cast To Boolean4' : Eliminate redundant data type conversion
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'metux_controller'
 * '<S1>'   : 'metux_controller/metux_controller'
 * '<S2>'   : 'metux_controller/metux_controller/E-STOP Override'
 * '<S3>'   : 'metux_controller/metux_controller/Kalman Filter1'
 * '<S4>'   : 'metux_controller/metux_controller/motor current controller'
 * '<S5>'   : 'metux_controller/metux_controller/motor position controller'
 * '<S6>'   : 'metux_controller/metux_controller/motor speed controller'
 * '<S7>'   : 'metux_controller/metux_controller/Kalman Filter1/TPE_KALMAN_spd'
 * '<S8>'   : 'metux_controller/metux_controller/motor current controller/motor velocity controller'
 * '<S9>'   : 'metux_controller/metux_controller/motor current controller/motor velocity controller/Compare To Constant'
 * '<S10>'  : 'metux_controller/metux_controller/motor current controller/motor velocity controller/Compare To Constant1'
 * '<S11>'  : 'metux_controller/metux_controller/motor current controller/motor velocity controller/Compare To Constant2'
 * '<S12>'  : 'metux_controller/metux_controller/motor current controller/motor velocity controller/Compare To Constant3'
 * '<S13>'  : 'metux_controller/metux_controller/motor current controller/motor velocity controller/clock'
 * '<S14>'  : 'metux_controller/metux_controller/motor position controller/motor position controller'
 * '<S15>'  : 'metux_controller/metux_controller/motor position controller/motor position controller/Discrete Derivative'
 * '<S16>'  : 'metux_controller/metux_controller/motor speed controller/motor velocity controller'
 * '<S17>'  : 'metux_controller/metux_controller/motor speed controller/motor velocity controller/Compare To Constant'
 * '<S18>'  : 'metux_controller/metux_controller/motor speed controller/motor velocity controller/Compare To Constant1'
 * '<S19>'  : 'metux_controller/metux_controller/motor speed controller/motor velocity controller/Compare To Constant2'
 * '<S20>'  : 'metux_controller/metux_controller/motor speed controller/motor velocity controller/Compare To Constant3'
 * '<S21>'  : 'metux_controller/metux_controller/motor speed controller/motor velocity controller/clock'
 */
#endif                                 /* metux_controller_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
