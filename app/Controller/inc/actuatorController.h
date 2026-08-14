//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: actuatorController.h
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
#ifndef actuatorController_h_
#define actuatorController_h_
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

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern struct_In9luHMHiNdKBncTZV4w1E currentControllerGains;// Variable: controllerGains
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

extern controller_modes controller_mode;// Variable: controller_mode
                                           //  Referenced by: '<S1>/controller_mode'


// Class declaration for model actuatorController
namespace controller
{
  class actuatorController final
  {
    // public data and function members
   public:
    // Block signals and states (default storage) for system '<Root>'
    struct DW {
      real32_T Saturation_m;           // '<S11>/Saturation'
      real32_T uDLookupTable;          // '<S2>/1-D Lookup Table'
      real32_T DiscreteTimeIntegrator_DSTATE;// '<S18>/Discrete-Time Integrator' 
      real32_T DiscreteTimeIntegrator_DSTATE_b;// '<S12>/Discrete-Time Integrator' 
      real32_T UD_DSTATE;              // '<S16>/UD'
      real32_T PrevY;                  // '<S12>/Rate Limiter'
      boolean_T SpeedController_MODE;  // '<S1>/Speed Controller'
      boolean_T PositionController_MODE;// '<S1>/Position Controller'
    };

    // Constant parameters (default storage)
    struct ConstP {
      // Computed Parameter: uDLookupTable_tableData
      //  Referenced by: '<S2>/1-D Lookup Table'

      real32_T uDLookupTable_tableData[12];
    };

    // External inputs (root inport signals with default storage)
    struct ExtU {
      real32_T P_manifold;             // '<Root>/P_manifold'
      real32_T P_nozzle_demand;        // '<Root>/P_nozzle_demand'
      real32_T P_nozzle;               // '<Root>/P_nozzle'
      real32_T SpeedFeedback;          // '<Root>/speed_feedback'
      real32_T pos_ref_ext;            // '<Root>/pos_ref_ext'
      real32_T pos_feedback;           // '<Root>/pos_feedback'
      real32_T speedDemand_ext;        // '<Root>/speedDemand_ext'
      real32_T ValveFitPressureRatios[12];// '<Root>/ValveFitPressureRatios'
    };

    // External outputs (root outports fed by signals with default storage)
    struct ExtY {
      real32_T currentDemand;          // '<Root>/currentDemand'
      real32_T speedDemand;            // '<Root>/speedDemand'
      real32_T position_demand;        // '<Root>/position_demand'
      real32_T pos_ref_rate_limited;   // '<Root>/pos_ref_rate_limited'
    };

    // Real-time Model Data Structure
    struct RT_MODEL {
      const char_T * volatile errorStatus;
      const char_T* getErrorStatus() const;
      void setErrorStatus(const char_T* const volatile aErrorStatus);
    };

    // Copy Constructor
    actuatorController(actuatorController const&) = delete;

    // Assignment Operator
    actuatorController& operator= (actuatorController const&) & = delete;

    // Move Constructor
    actuatorController(actuatorController &&) = delete;

    // Move Assignment Operator
    actuatorController& operator= (actuatorController &&) = delete;

    // Real-Time Model get method
    actuatorController::RT_MODEL * getRTM();

    // External inputs
    ExtU rtU;

    // External outputs
    ExtY rtY;

    // model initialize function
    static void initialize();

    // model step function
    void step();

    // Constructor
    actuatorController();

    // Destructor
    ~actuatorController();

    // private data and function members
   private:
    // Block states
    DW rtDW;

    // Real-Time Model
    RT_MODEL rtM;
  };
}

// Constant parameters (default storage)
extern const controller::actuatorController::ConstP rtConstP;

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S15>/Data Type Duplicate' : Unused code path elimination
//  Block '<S15>/Data Type Propagation' : Unused code path elimination
//  Block '<S16>/Data Type Duplicate' : Unused code path elimination
//  Block '<S11>/Scope' : Unused code path elimination
//  Block '<S1>/Scope' : Unused code path elimination
//  Block '<S13>/1-D Lookup Table' : Unused code path elimination
//  Block '<S13>/Constant' : Unused code path elimination
//  Block '<S13>/Divide' : Unused code path elimination
//  Block '<S13>/Gain' : Unused code path elimination
//  Block '<S13>/Saturation' : Unused code path elimination
//  Block '<S17>/Data Type Duplicate' : Unused code path elimination
//  Block '<S17>/Data Type Propagation' : Unused code path elimination
//  Block '<S17>/LowerRelop1' : Unused code path elimination
//  Block '<S17>/Switch' : Unused code path elimination
//  Block '<S17>/Switch2' : Unused code path elimination
//  Block '<S17>/UpperRelop' : Unused code path elimination
//  Block '<S13>/Saturation1' : Unused code path elimination
//  Block '<S14>/Scope' : Unused code path elimination
//  Block '<S18>/K' : Eliminated nontunable gain of 1


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
//  '<Root>' : 'actuatorController'
//  '<S1>'   : 'actuatorController/Subsystem1'
//  '<S2>'   : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table'
//  '<S3>'   : 'actuatorController/Subsystem1/Compare To Constant'
//  '<S4>'   : 'actuatorController/Subsystem1/Compare To Constant1'
//  '<S5>'   : 'actuatorController/Subsystem1/Compare To Constant2'
//  '<S6>'   : 'actuatorController/Subsystem1/Compare To Constant3'
//  '<S7>'   : 'actuatorController/Subsystem1/Compare To Constant4'
//  '<S8>'   : 'actuatorController/Subsystem1/Compare To Constant5'
//  '<S9>'   : 'actuatorController/Subsystem1/Degrees to Radians'
//  '<S10>'  : 'actuatorController/Subsystem1/Degrees to Radians1'
//  '<S11>'  : 'actuatorController/Subsystem1/Position Controller'
//  '<S12>'  : 'actuatorController/Subsystem1/Speed Controller'
//  '<S13>'  : 'actuatorController/Subsystem1/Thrust Controller'
//  '<S14>'  : 'actuatorController/Subsystem1/reference conditioning'
//  '<S15>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/Saturation Dynamic'
//  '<S16>'  : 'actuatorController/Subsystem1/Position Controller/Discrete Derivative'
//  '<S17>'  : 'actuatorController/Subsystem1/Thrust Controller/Saturation Dynamic'
//  '<S18>'  : 'actuatorController/Subsystem1/reference conditioning/first order hiz limitli filtre'

#endif                                 // actuatorController_h_

//
// File trailer for generated code.
//
// [EOF]
//
