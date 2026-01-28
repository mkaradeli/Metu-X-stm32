//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: currentController.h
//
// Code generated for Simulink model 'currentController'.
//
// Model version                  : 1.19
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Wed Jan 28 20:30:42 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#ifndef currentController_h_
#define currentController_h_
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

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern struct_WlbPQVH06SMTAC0rh3ezxC currentControllerGains;// Variable: controllerGains
                                                               //  Referenced by:
                                                               //    '<S1>/Discrete-Time Integrator'
                                                               //    '<S1>/Gain'
                                                               //    '<S1>/Gain1'
                                                               //    '<S1>/Gain2'
                                                               //    '<S1>/Saturation'


// Class declaration for model currentController
class CurrentController final
{
  // public data and function members
 public:
  // Block signals and states (default storage) for system '<Root>'
  struct DW {
    real32_T DiscreteTimeIntegrator_DSTATE;// '<S1>/Discrete-Time Integrator'
    boolean_T PI_Controller_MODE;      // '<Root>/PI_Controller'
  };

  // External inputs (root inport signals with default storage)
  struct ExtU {
    real32_T current_ref;              // '<Root>/current_ref'
    real32_T current_feedback;         // '<Root>/current_feedback'
    boolean_T enabled;                 // '<Root>/enabled'
  };

  // External outputs (root outports fed by signals with default storage)
  struct ExtY {
    real32_T Duty;                     // '<Root>/Duty'
  };

  // Parameters (default storage)
  struct P {
    PI_debug debug_Y0;                 // Computed Parameter: debug_Y0
                                          //  Referenced by: '<S1>/debug'

    real32_T Y_Y0;                     // Computed Parameter: Y_Y0
                                          //  Referenced by: '<S1>/Y'

    real32_T DiscreteTimeIntegrator_gainval;
                           // Computed Parameter: DiscreteTimeIntegrator_gainval
                              //  Referenced by: '<S1>/Discrete-Time Integrator'

    real32_T DiscreteTimeIntegrator_IC;
                                // Computed Parameter: DiscreteTimeIntegrator_IC
                                   //  Referenced by: '<S1>/Discrete-Time Integrator'

  };

  // Copy Constructor
  CurrentController(CurrentController const&) = delete;

  // Assignment Operator
  CurrentController& operator= (CurrentController const&) & = delete;

  // Move Constructor
  CurrentController(CurrentController &&) = delete;

  // Move Assignment Operator
  CurrentController& operator= (CurrentController &&) = delete;

  // External inputs
  ExtU rtU;

  // External outputs
  ExtY rtY;

  // model initialize function
  void initialize();

  // model step function
  void step();

  // Constructor
  CurrentController();

  // Destructor
  ~CurrentController();

  // private data and function members
 private:
  // Block states
  DW rtDW;

  // Tunable parameters
  static P rtP;
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
//  '<Root>' : 'currentController'
//  '<S1>'   : 'currentController/PI_Controller'

#endif                                 // currentController_h_

//
// File trailer for generated code.
//
// [EOF]
//
