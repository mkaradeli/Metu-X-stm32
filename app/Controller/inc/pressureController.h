//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: pressureController.h
//
// Code generated for Simulink model 'pressureController'.
//
// Model version                  : 1.19
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Sun Feb 15 20:47:12 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: All passed
//
#ifndef pressureController_h_
#define pressureController_h_
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

// Imported (extern) block parameters
extern controller_modes controller_mode;// Variable: controller_mode
                                           //  Referenced by: '<Root>/Constant'


// Class declaration for model pressureController
class pressureController final
{
  // public data and function members
 public:
  // Block signals and states (default storage) for system '<Root>'
  struct DW {
    boolean_T Subsystem_MODE;          // '<Root>/Subsystem'
  };

  // External inputs (root inport signals with default storage)
  struct ExtU {
    real32_T P_manifold;               // '<Root>/P_manifold'
    real32_T P_nozzle_demand;          // '<Root>/P_nozzle_demand'
    real32_T position_feedback;        // '<Root>/position_feedback'
    real32_T P_nozzle;                 // '<Root>/P_nozzle'
  };

  // External outputs (root outports fed by signals with default storage)
  struct ExtY {
    real32_T position_demand;          // '<Root>/position_demand'
  };

  // Copy Constructor
  pressureController(pressureController const&) = delete;

  // Assignment Operator
  pressureController& operator= (pressureController const&) & = delete;

  // Move Constructor
  pressureController(pressureController &&) = delete;

  // Move Assignment Operator
  pressureController& operator= (pressureController &&) = delete;

  // External inputs
  ExtU rtU;

  // External outputs
  ExtY rtY;

  // model initialize function
  static void initialize();

  // model step function
  void step();

  // Constructor
  pressureController();

  // Destructor
  ~pressureController();

  // private data and function members
 private:
  // Block states
  DW rtDW;
};

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S2>/Scope' : Unused code path elimination
//  Block '<S5>/Data Type Duplicate' : Unused code path elimination
//  Block '<S5>/Data Type Propagation' : Unused code path elimination


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
//  '<Root>' : 'pressureController'
//  '<S1>'   : 'pressureController/Compare To Constant'
//  '<S2>'   : 'pressureController/Subsystem'
//  '<S3>'   : 'pressureController/Subsystem/ValveModelForController'
//  '<S4>'   : 'pressureController/Subsystem/ValveModelForPlant'
//  '<S5>'   : 'pressureController/Subsystem/ValveModelForController/Saturation Dynamic'

#endif                                 // pressureController_h_

//
// File trailer for generated code.
//
// [EOF]
//
