//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: positionController.h
//
// Code generated for Simulink model 'positionController'.
//
// Model version                  : 1.8
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Wed Jan 28 20:38:14 2026
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
#ifndef DEFINED_TYPEDEF_FOR_PI_debug_
#define DEFINED_TYPEDEF_FOR_PI_debug_

struct PI_debug
{
  real32_T Up;
  real32_T Ui;
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

#ifndef DEFINED_TYPEDEF_FOR_struct_FVhgMCieLhQVP3W4i9T0PB_
#define DEFINED_TYPEDEF_FOR_struct_FVhgMCieLhQVP3W4i9T0PB_

struct struct_FVhgMCieLhQVP3W4i9T0PB
{
  real32_T Kp;
  real32_T SatMax;
  real32_T SatMin;
  real_T Ts;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_WlbPQVH06SMTAC0rh3ezxC_
#define DEFINED_TYPEDEF_FOR_struct_WlbPQVH06SMTAC0rh3ezxC_

struct struct_WlbPQVH06SMTAC0rh3ezxC
{
  struct_lIrpsBx2XGeWflfk3xVByG current;
  struct_lIrpsBx2XGeWflfk3xVByG speed;
  struct_FVhgMCieLhQVP3W4i9T0PB position;
};

#endif

// Imported (extern) block parameters
extern struct_WlbPQVH06SMTAC0rh3ezxC currentControllerGains;// Variable: controllerGains
                                                               //  Referenced by:
                                                               //    '<S1>/Gain'
                                                               //    '<S1>/Saturation'
                                                               //    '<S2>/Discrete-Time Integrator'
                                                               //    '<S2>/Gain'
                                                               //    '<S2>/Gain1'
                                                               //    '<S2>/Gain2'
                                                               //    '<S2>/Saturation'


// Class declaration for model positionController
class positionController final
{
  // public data and function members
 public:
  // Block signals and states (default storage) for system '<Root>'
  struct DW {
    real32_T DiscreteTimeIntegrator_DSTATE;// '<S2>/Discrete-Time Integrator'
    boolean_T SpeedController_MODE;    // '<Root>/Speed Controller'
    boolean_T PositionController_MODE; // '<Root>/Position Controller'
  };

  // External inputs (root inport signals with default storage)
  struct ExtU {
    real32_T pos_ref;                  // '<Root>/pos_ref'
    real32_T pos_feedback;             // '<Root>/pos_feedback'
    boolean_T position_enable;         // '<Root>/position_enable'
    real32_T SpeedFeedback;            // '<Root>/speed_feedback'
    boolean_T speed_enable;            // '<Root>/speed_enable'
  };

  // External outputs (root outports fed by signals with default storage)
  struct ExtY {
    real32_T currentDemand;            // '<Root>/currentDemand'
    PI_debug speedDebug;               // '<Root>/speedDebug'
    real32_T speedDemand;              // '<Root>/speedDemand'
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
//  '<S1>'   : 'positionController/Position Controller'
//  '<S2>'   : 'positionController/Speed Controller'

#endif                                 // positionController_h_

//
// File trailer for generated code.
//
// [EOF]
//
