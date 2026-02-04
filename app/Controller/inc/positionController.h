//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: positionController.h
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
#ifndef positionController_h_
#define positionController_h_
#include <cmath>
#include "rtwtypes.h"
#ifndef DEFINED_TYPEDEF_FOR_controller_modes_
#define DEFINED_TYPEDEF_FOR_controller_modes_

enum class controller_modes
  : int32_T {
  DISABLE = 0,                         // Default value
  DUTY,
  CURRENT,
  SPEED,
  POSITION,
  PRESSURE,
  FORCE
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_PI_debug_
#define DEFINED_TYPEDEF_FOR_PI_debug_

struct PI_debug
{
  real32_T Up;
  real32_T Ui;
  real32_T ref_rate_limited;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_lIrpsBx2XGeWflfk3xVByG_
#define DEFINED_TYPEDEF_FOR_struct_lIrpsBx2XGeWflfk3xVByG_

struct struct_lIrpsBx2XGeWflfk3xVByG
{
  real32_T Kp;
  real32_T Ki;
  real32_T SatMax;
  real32_T SatMin;
  real_T Ts;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_efjc54AtCP4u6rYRnJfUyF_
#define DEFINED_TYPEDEF_FOR_struct_efjc54AtCP4u6rYRnJfUyF_

struct struct_efjc54AtCP4u6rYRnJfUyF
{
  real32_T Kp;
  real32_T Ki;
  real32_T SatMax;
  real32_T SatMin;
  real32_T RateLimiterMax;
  real32_T RateLimiterMin;
  real_T Ts;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_UOhA0kLqPt9jH3DECgPk1E_
#define DEFINED_TYPEDEF_FOR_struct_UOhA0kLqPt9jH3DECgPk1E_

struct struct_UOhA0kLqPt9jH3DECgPk1E
{
  real32_T Kp;
  real32_T Kff;
  real32_T SatMax;
  real32_T SatMin;
  real32_T RateLimiterMax;
  real32_T RateLimiterMin;
  real_T Ts;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_bcCVSDe9ijc6YF0EEo7cx_
#define DEFINED_TYPEDEF_FOR_struct_bcCVSDe9ijc6YF0EEo7cx_

struct struct_bcCVSDe9ijc6YF0EEo7cx
{
  struct_lIrpsBx2XGeWflfk3xVByG current;
  struct_efjc54AtCP4u6rYRnJfUyF speed;
  struct_UOhA0kLqPt9jH3DECgPk1E position;
};

#endif

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern struct_bcCVSDe9ijc6YF0EEo7cx currentControllerGains;// Variable: controllerGains
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

extern controller_modes controller_mode;// Variable: controller_mode
                                           //  Referenced by: '<Root>/controller_mode'


// Class declaration for model positionController
class positionController final
{
  // public data and function members
 public:
  // Block signals and states (default storage) for system '<Root>'
  struct DW {
    real32_T Ui;                       // '<S4>/Discrete-Time Integrator'
    real32_T RateLimiter;              // '<S4>/Rate Limiter'
    real32_T Up;                       // '<S4>/Gain'
    real32_T DiscreteTimeIntegrator_DSTATE;// '<S4>/Discrete-Time Integrator'
    real32_T UD_DSTATE;                // '<S6>/UD'
    real32_T PrevY;                    // '<S5>/Rate Limiter'
    real32_T PrevY_i;                  // '<S4>/Rate Limiter'
    boolean_T SpeedController_MODE;    // '<Root>/Speed Controller'
    boolean_T PositionController_MODE; // '<Root>/Position Controller'
  };

  // External inputs (root inport signals with default storage)
  struct ExtU {
    real32_T pos_ref;                  // '<Root>/pos_ref'
    real32_T pos_feedback;             // '<Root>/pos_feedback'
    real32_T SpeedFeedback;            // '<Root>/speed_feedback'
  };

  // External outputs (root outports fed by signals with default storage)
  struct ExtY {
    real32_T currentDemand;            // '<Root>/currentDemand'
    PI_debug speedDebug;               // '<Root>/speedDebug'
    real32_T speedDemand;              // '<Root>/speedDemand'
    real32_T pos_ref_rate_limited;     // '<Root>/pos_ref_rate_limited'
  };

  // Copy Constructor
  positionController(positionController const&) = delete;

  // Assignment Operator
  positionController& operator= (positionController const&) & = delete;

  // Move Constructor
  positionController(positionController &&) = delete;

  // Move Assignment Operator
  positionController& operator= (positionController &&) = delete;

  // External inputs
  ExtU rtU;

  // External outputs
  ExtY rtY;

  // model initialize function
  static void initialize();

  // model step function
  void step();

  // Constructor
  positionController();

  // Destructor
  ~positionController();

  // private data and function members
 private:
  // Block states
  DW rtDW;
};

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S6>/Data Type Duplicate' : Unused code path elimination
//  Block '<Root>/Scope' : Unused code path elimination


//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Use the MATLAB hilite_system command to trace the generated code back
//  to the model.  For example,
//
//  hilite_system('<S3>')    - opens system 3
//  hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'positionController'
//  '<S1>'   : 'positionController/Compare To Constant'
//  '<S2>'   : 'positionController/Compare To Constant1'
//  '<S3>'   : 'positionController/Position Controller'
//  '<S4>'   : 'positionController/Speed Controller'
//  '<S5>'   : 'positionController/reference conditioning'
//  '<S6>'   : 'positionController/Position Controller/Discrete Derivative'

#endif                                 // positionController_h_

//
// File trailer for generated code.
//
// [EOF]
//
