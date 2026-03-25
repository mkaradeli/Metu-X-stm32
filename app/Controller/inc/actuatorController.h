//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: actuatorController.h
//
// Code generated for Simulink model 'actuatorController'.
//
// Model version                  : 1.43
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Sat Mar 21 17:05:46 2026
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
                                                               //    '<S9>/Gain'
                                                               //    '<S9>/Gain1'
                                                               //    '<S9>/Saturation'
                                                               //    '<S10>/Gain1'
                                                               //    '<S11>/Discrete-Time Integrator'
                                                               //    '<S11>/Gain'
                                                               //    '<S11>/Gain1'
                                                               //    '<S11>/Gain2'
                                                               //    '<S11>/Rate Limiter'
                                                               //    '<S11>/Saturation'

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
      real32_T Probe[2];               // '<S34>/Probe'
      real32_T Probe_h[2];             // '<S20>/Probe'
      real32_T Probe_i[2];             // '<S27>/Probe'
      real32_T Product;                // '<S10>/Product'
      real32_T Gain1;                  // '<S10>/Gain1'
      real32_T Saturation_m;           // '<S9>/Saturation'
      real32_T DiscreteTimeIntegrator_DSTATE;// '<S42>/Discrete-Time Integrator' 
      real32_T DiscreteTimeIntegrator_DSTATE_b;// '<S11>/Discrete-Time Integrator' 
      real32_T Integrator_DSTATE;      // '<S40>/Integrator'
      real32_T Integrator_DSTATE_i;    // '<S26>/Integrator'
      real32_T Integrator_DSTATE_d;    // '<S33>/Integrator'
      real32_T UD_DSTATE;              // '<S13>/UD'
      real32_T PrevY;                  // '<S11>/Rate Limiter'
      int8_T Integrator_PrevResetState;// '<S40>/Integrator'
      int8_T Integrator_PrevResetState_h;// '<S26>/Integrator'
      int8_T Integrator_PrevResetState_c;// '<S33>/Integrator'
      uint8_T Integrator_IC_LOADING;   // '<S40>/Integrator'
      uint8_T Integrator_IC_LOADING_m; // '<S26>/Integrator'
      uint8_T Integrator_IC_LOADING_f; // '<S33>/Integrator'
      boolean_T SpeedController_MODE;  // '<S1>/Speed Controller'
      boolean_T PressureController_MODE;// '<S1>/Pressure Controller'
      boolean_T PositionController_MODE;// '<S1>/Position Controller'
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
    void initialize();

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

extern "C"
{
  extern real_T rtInf;
  extern real_T rtMinusInf;
  extern real_T rtNaN;
  extern real32_T rtInfF;
  extern real32_T rtMinusInfF;
  extern real32_T rtNaNF;
  static boolean_T rtIsInf(real_T value);
  static boolean_T rtIsInfF(real32_T value);
  static boolean_T rtIsNaN(real_T value);
  static boolean_T rtIsNaNF(real32_T value);
}                                      // extern "C"

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S13>/Data Type Duplicate' : Unused code path elimination
//  Block '<S9>/Scope' : Unused code path elimination
//  Block '<S10>/Scope' : Unused code path elimination
//  Block '<S10>/Scope1' : Unused code path elimination
//  Block '<S10>/Scope2' : Unused code path elimination
//  Block '<S10>/Scope3' : Unused code path elimination
//  Block '<S10>/Scope4' : Unused code path elimination
//  Block '<S10>/Scope5' : Unused code path elimination
//  Block '<S10>/Scope6' : Unused code path elimination
//  Block '<S10>/Scope7' : Unused code path elimination
//  Block '<S10>/Scope8' : Unused code path elimination
//  Block '<S41>/Data Type Duplicate' : Unused code path elimination
//  Block '<S41>/Data Type Propagation' : Unused code path elimination
//  Block '<S1>/Scope' : Unused code path elimination
//  Block '<S1>/Scope1' : Unused code path elimination
//  Block '<S12>/Scope' : Unused code path elimination
//  Block '<S10>/Gain3' : Eliminated nontunable gain of 1
//  Block '<S26>/Saturation' : Eliminated Saturate block
//  Block '<S15>/K' : Eliminated nontunable gain of 1
//  Block '<S33>/Saturation' : Eliminated Saturate block
//  Block '<S16>/K' : Eliminated nontunable gain of 1
//  Block '<S40>/Saturation' : Eliminated Saturate block
//  Block '<S17>/K' : Eliminated nontunable gain of 1
//  Block '<S42>/K' : Eliminated nontunable gain of 1


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
//  '<S2>'   : 'actuatorController/Subsystem1/Compare To Constant'
//  '<S3>'   : 'actuatorController/Subsystem1/Compare To Constant1'
//  '<S4>'   : 'actuatorController/Subsystem1/Compare To Constant2'
//  '<S5>'   : 'actuatorController/Subsystem1/Compare To Constant3'
//  '<S6>'   : 'actuatorController/Subsystem1/Compare To Constant4'
//  '<S7>'   : 'actuatorController/Subsystem1/Degrees to Radians'
//  '<S8>'   : 'actuatorController/Subsystem1/Degrees to Radians1'
//  '<S9>'   : 'actuatorController/Subsystem1/Position Controller'
//  '<S10>'  : 'actuatorController/Subsystem1/Pressure Controller'
//  '<S11>'  : 'actuatorController/Subsystem1/Speed Controller'
//  '<S12>'  : 'actuatorController/Subsystem1/reference conditioning'
//  '<S13>'  : 'actuatorController/Subsystem1/Position Controller/Discrete Derivative'
//  '<S14>'  : 'actuatorController/Subsystem1/Pressure Controller/Compare To Constant'
//  '<S15>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)1'
//  '<S16>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)2'
//  '<S17>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)3'
//  '<S18>'  : 'actuatorController/Subsystem1/Pressure Controller/ValveModelForController'
//  '<S19>'  : 'actuatorController/Subsystem1/Pressure Controller/ValveModelForPlant'
//  '<S20>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)1/Enable//disable time constant'
//  '<S21>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)1/Initialization'
//  '<S22>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)1/Integrator (Discrete or Continuous)'
//  '<S23>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)1/Enable//disable time constant/Compare To Constant'
//  '<S24>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)1/Enable//disable time constant/Compare To Zero'
//  '<S25>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)1/Initialization/Init_u'
//  '<S26>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)1/Integrator (Discrete or Continuous)/Discrete'
//  '<S27>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)2/Enable//disable time constant'
//  '<S28>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)2/Initialization'
//  '<S29>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)2/Integrator (Discrete or Continuous)'
//  '<S30>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)2/Enable//disable time constant/Compare To Constant'
//  '<S31>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)2/Enable//disable time constant/Compare To Zero'
//  '<S32>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)2/Initialization/Init_u'
//  '<S33>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)2/Integrator (Discrete or Continuous)/Discrete'
//  '<S34>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)3/Enable//disable time constant'
//  '<S35>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)3/Initialization'
//  '<S36>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)3/Integrator (Discrete or Continuous)'
//  '<S37>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)3/Enable//disable time constant/Compare To Constant'
//  '<S38>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)3/Enable//disable time constant/Compare To Zero'
//  '<S39>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)3/Initialization/Init_u'
//  '<S40>'  : 'actuatorController/Subsystem1/Pressure Controller/Low-Pass Filter (Discrete or Continuous)3/Integrator (Discrete or Continuous)/Discrete'
//  '<S41>'  : 'actuatorController/Subsystem1/Pressure Controller/ValveModelForController/Saturation Dynamic'
//  '<S42>'  : 'actuatorController/Subsystem1/reference conditioning/first order hiz limitli filtre'

#endif                                 // actuatorController_h_

//
// File trailer for generated code.
//
// [EOF]
//
