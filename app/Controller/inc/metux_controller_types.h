/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: metux_controller_types.h
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

#ifndef metux_controller_types_h_
#define metux_controller_types_h_
#include "rtwtypes.h"
#ifndef DEFINED_TYPEDEF_FOR_struct_uPAe3g7S4gcBpKefqviYUB_
#define DEFINED_TYPEDEF_FOR_struct_uPAe3g7S4gcBpKefqviYUB_

typedef struct {
  real_T kP;
  real_T kI;
  real_T kD;
  real_T kFF;
  real_T rateLimiter;
} struct_uPAe3g7S4gcBpKefqviYUB;

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_vowNbQvkfytzerHGmKsAtG_
#define DEFINED_TYPEDEF_FOR_struct_vowNbQvkfytzerHGmKsAtG_

typedef struct {
  real_T kP;
  real_T kI;
  real_T kD;
  real_T kFF;
  real_T saturation;
  real_T saturationInt;
  real_T rateLimiter;
  real_T activeBand;
} struct_vowNbQvkfytzerHGmKsAtG;

#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM_metux_controller_T RT_MODEL_metux_controller_T;

#endif                                 /* metux_controller_types_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
