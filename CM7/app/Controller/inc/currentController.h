//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: currentController.h
//
// Code generated for Simulink model 'currentController'.
//
// Model version                  : 1.119
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Fri Mar 20 14:12:55 2026
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


// Class declaration for model currentController
namespace controller
{
  class current final
  {
    // public data and function members
   public:
    // Block signals and states (default storage) for system '<Root>'
    struct DW {
      real32_T DelayInput_DSTATE;      // '<S3>/Delay Input'
      real32_T DelayOutput_DSTATE;     // '<S3>/Delay Output'
      boolean_T Subsystem3_MODE;       // '<Root>/Subsystem3'
    };

    // External inputs (root inport signals with default storage)
    struct ExtU {
      real32_T current_ref;            // '<Root>/current_ref'
      real32_T current_feedback;       // '<Root>/current_feedback'
    };

    // External outputs (root outports fed by signals with default storage)
    struct ExtY {
      real32_T Duty;                   // '<Root>/Duty'
    };

    // Copy Constructor
    current(current const&) = delete;

    // Assignment Operator
    current& operator= (current const&) & = delete;

    // Move Constructor
    current(current &&) = delete;

    // Move Assignment Operator
    current& operator= (current &&) = delete;

    // External inputs
    ExtU rtU;

    // External outputs
    ExtY rtY;

    // model initialize function
    static void initialize();

    // model step function
    void step();

    // Constructor
    current();

    // Destructor
    ~current();

    // private data and function members
   private:
    // Block states
    DW rtDW;
  };
}

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S3>/DataTypeProp' : Unused code path elimination
//  Block '<S2>/Gain3' : Unused code path elimination
//  Block '<S3>/Downcast' : Eliminate redundant data type conversion


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
//  '<S1>'   : 'currentController/Compare To Constant'
//  '<S2>'   : 'currentController/Subsystem3'
//  '<S3>'   : 'currentController/Subsystem3/ChatGPT'

#endif                                 // currentController_h_

//
// File trailer for generated code.
//
// [EOF]
//
