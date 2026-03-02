//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: pressureController.h
//
// Code generated for Simulink model 'pressureController'.
//
// Model version                  : 1.23
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Thu Feb 26 20:06:22 2026
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

#ifndef DEFINED_TYPEDEF_FOR_struct_nX7ssOP8Pea1vyXt0UgmRE_
#define DEFINED_TYPEDEF_FOR_struct_nX7ssOP8Pea1vyXt0UgmRE_

struct struct_nX7ssOP8Pea1vyXt0UgmRE
{
  real32_T Kp;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_In9luHMHiNdKBncTZV4w1E_
#define DEFINED_TYPEDEF_FOR_struct_In9luHMHiNdKBncTZV4w1E_

struct struct_In9luHMHiNdKBncTZV4w1E
{
  struct_lIrpsBx2XGeWflfk3xVByG current;
  struct_efjc54AtCP4u6rYRnJfUyF speed;
  struct_UOhA0kLqPt9jH3DECgPk1E position;
  struct_nX7ssOP8Pea1vyXt0UgmRE pressure;
};

#endif

// Imported (extern) block parameters
extern struct_In9luHMHiNdKBncTZV4w1E currentControllerGains;// Variable: controllerGains
                                                               //  Referenced by: '<S2>/Gain1'

extern controller_modes controller_mode;// Variable: controller_mode
                                           //  Referenced by: '<Root>/Constant'


// Class declaration for model pressureController
namespace controller
{
  class pressure final
  {
    // public data and function members
   public:
    // Block signals and states (default storage) for system '<Root>'
    struct DW {
      boolean_T Subsystem_MODE;        // '<Root>/Subsystem'
    };

    // External inputs (root inport signals with default storage)
    struct ExtU {
      real32_T P_manifold;             // '<Root>/P_manifold'
      real32_T P_nozzle_demand;        // '<Root>/P_nozzle_demand'
      real32_T P_nozzle;               // '<Root>/P_nozzle'
    };

    // External outputs (root outports fed by signals with default storage)
    struct ExtY {
      real32_T position_demand;        // '<Root>/position_demand'
    };

    // Copy Constructor
    pressure(pressure const&) = delete;

    // Assignment Operator
    pressure& operator= (pressure const&) & = delete;

    // Move Constructor
    pressure(pressure &&) = delete;

    // Move Assignment Operator
    pressure& operator= (pressure &&) = delete;

    // External inputs
    ExtU rtU;

    // External outputs
    ExtY rtY;

    // model initialize function
    static void initialize();

    // model step function
    void step();

    // Constructor
    pressure();

    // Destructor
    ~pressure();

    // private data and function members
   private:
    // Block states
    DW rtDW;
  };
}

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S4>/Data Type Duplicate' : Unused code path elimination
//  Block '<S4>/Data Type Propagation' : Unused code path elimination


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
//  '<S4>'   : 'pressureController/Subsystem/ValveModelForController/Saturation Dynamic'

#endif                                 // pressureController_h_

//
// File trailer for generated code.
//
// [EOF]
//
