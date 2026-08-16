//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: platformController.h
//
// Code generated for Simulink model 'platformController'.
//
// Model version                  : 1.31
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Sat Aug 15 19:01:51 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#ifndef platformController_h_
#define platformController_h_
#include "rtwtypes.h"
#ifndef DEFINED_TYPEDEF_FOR_mission_modes_
#define DEFINED_TYPEDEF_FOR_mission_modes_

enum class mission_modes
  : int32_T {
  DISABLE = 0,                         // Default value
  TESTFIRE,
  HOVER,
  DROP
};

#endif

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern mission_modes mission_mode;     // Variable: mission_mode
                                          //  Referenced by: '<S1>/Constant'


// Class declaration for model platformController
class platformController final
{
  // public data and function members
 public:
  // Block signals and states (default storage) for system '<Root>'
  struct DW {
    real_T Integrator_DSTATE;          // '<S48>/Integrator'
    real_T Integrator_DSTATE_a;        // '<S107>/Integrator'
    int8_T Integrator_PrevResetState;  // '<S48>/Integrator'
    int8_T Integrator_PrevResetState_b;// '<S107>/Integrator'
  };

  // External inputs (root inport signals with default storage)
  struct ExtU {
    real_T Height;                     // '<Root>/Height'
    real_T Velocity;                   // '<Root>/Velocity'
    real_T ManifoldPressure;           // '<Root>/ManifoldPressure'
    real_T quaternion[4];              // '<Root>/quaternion'
    boolean_T Dropped;                 // '<Root>/Dropped'
    real_T T_max_allowed;              // '<Root>/T_max_allowed'
    real_T T_alloc_total;              // '<Root>/T_alloc_total'
  };

  // External outputs (root outports fed by signals with default storage)
  struct ExtY {
    real_T FrontThrustCmd;             // '<Root>/FrontThrustCmd'
    real_T BackThrustCmd;              // '<Root>/BackThrustCmd'
    real_T LeftThrustCmd;              // '<Root>/LeftThrustCmd'
    real_T RightThrustCmd;             // '<Root>/RightThrustCmd'
  };

  // Real-time Model Data Structure
  struct RT_MODEL {
    const char_T * volatile errorStatus;
    const char_T* getErrorStatus() const;
    void setErrorStatus(const char_T* const volatile aErrorStatus);
  };

  // Copy Constructor
  platformController(platformController const&) = delete;

  // Assignment Operator
  platformController& operator= (platformController const&) & = delete;

  // Move Constructor
  platformController(platformController &&) = delete;

  // Move Assignment Operator
  platformController& operator= (platformController &&) = delete;

  // Real-Time Model get method
  platformController::RT_MODEL * getRTM();

  // External inputs
  ExtU rtU;

  // External outputs
  ExtY rtY;

  // model initialize function
  void initialize();

  // model step function
  void step();

  // Constructor
  platformController();

  // Destructor
  ~platformController();

  // private data and function members
 private:
  // Block states
  DW rtDW;

  // Real-Time Model
  RT_MODEL rtM;
};

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S2>/AND' : Unused code path elimination
//  Block '<S6>/Compare' : Unused code path elimination
//  Block '<S6>/Constant' : Unused code path elimination
//  Block '<S8>/Compare' : Unused code path elimination
//  Block '<S8>/Constant' : Unused code path elimination
//  Block '<S2>/Constant6' : Unused code path elimination
//  Block '<S2>/Constant7' : Unused code path elimination
//  Block '<S2>/Gain4' : Unused code path elimination
//  Block '<S56>/Data Type Duplicate' : Unused code path elimination
//  Block '<S56>/Data Type Propagation' : Unused code path elimination
//  Block '<S2>/Product' : Unused code path elimination
//  Block '<S2>/Relational Operator' : Unused code path elimination
//  Block '<S2>/Saturation' : Unused code path elimination
//  Block '<S10>/Data Type Duplicate' : Unused code path elimination
//  Block '<S10>/Data Type Propagation' : Unused code path elimination
//  Block '<S2>/Scope' : Unused code path elimination
//  Block '<S2>/Scope1' : Unused code path elimination
//  Block '<S2>/Scope2' : Unused code path elimination
//  Block '<S2>/Scope3' : Unused code path elimination
//  Block '<S2>/Scope4' : Unused code path elimination
//  Block '<S2>/Scope5' : Unused code path elimination
//  Block '<S2>/Scope6' : Unused code path elimination
//  Block '<S2>/Scope7' : Unused code path elimination
//  Block '<S2>/Scope8' : Unused code path elimination
//  Block '<S2>/Square2' : Unused code path elimination
//  Block '<S2>/Sum2' : Unused code path elimination
//  Block '<S2>/Sum3' : Unused code path elimination
//  Block '<S67>/Compare' : Unused code path elimination
//  Block '<S67>/Constant' : Unused code path elimination
//  Block '<S3>/Constant' : Unused code path elimination
//  Block '<S115>/Data Type Duplicate' : Unused code path elimination
//  Block '<S115>/Data Type Propagation' : Unused code path elimination
//  Block '<S69>/Data Type Duplicate' : Unused code path elimination
//  Block '<S69>/Data Type Propagation' : Unused code path elimination
//  Block '<S3>/Scope' : Unused code path elimination
//  Block '<S3>/Scope1' : Unused code path elimination
//  Block '<S3>/Scope3' : Unused code path elimination
//  Block '<S3>/Scope4' : Unused code path elimination
//  Block '<S3>/Scope5' : Unused code path elimination
//  Block '<S3>/Scope8' : Unused code path elimination
//  Block '<S1>/Cast To Boolean' : Unused code path elimination
//  Block '<S1>/Cast To Boolean1' : Unused code path elimination
//  Block '<S1>/Cast To Boolean2' : Unused code path elimination
//  Block '<S1>/Cast To Boolean3' : Unused code path elimination
//  Block '<S60>/Kt' : Eliminated nontunable gain of 1
//  Block '<S119>/Kt' : Eliminated nontunable gain of 1


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
//  '<Root>' : 'platformController'
//  '<S1>'   : 'platformController/Subsystem'
//  '<S2>'   : 'platformController/Subsystem/Altitude Controller Drop'
//  '<S3>'   : 'platformController/Subsystem/Altitude Controller Hover'
//  '<S4>'   : 'platformController/Subsystem/Attitude Controller'
//  '<S5>'   : 'platformController/Subsystem/Altitude Controller Drop/Compare To Constant'
//  '<S6>'   : 'platformController/Subsystem/Altitude Controller Drop/Compare To Constant1'
//  '<S7>'   : 'platformController/Subsystem/Altitude Controller Drop/Compare To Constant2'
//  '<S8>'   : 'platformController/Subsystem/Altitude Controller Drop/Compare To Constant3'
//  '<S9>'   : 'platformController/Subsystem/Altitude Controller Drop/PID Controller'
//  '<S10>'  : 'platformController/Subsystem/Altitude Controller Drop/Saturation Dynamic'
//  '<S11>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Anti-windup'
//  '<S12>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/D Gain'
//  '<S13>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/External Derivative'
//  '<S14>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Filter'
//  '<S15>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Filter ICs'
//  '<S16>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/I Gain'
//  '<S17>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Ideal P Gain'
//  '<S18>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Ideal P Gain Fdbk'
//  '<S19>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Integrator'
//  '<S20>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Integrator ICs'
//  '<S21>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/N Copy'
//  '<S22>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/N Gain'
//  '<S23>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/P Copy'
//  '<S24>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Parallel P Gain'
//  '<S25>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Reset Signal'
//  '<S26>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Saturation'
//  '<S27>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Saturation Fdbk'
//  '<S28>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Sum'
//  '<S29>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Sum Fdbk'
//  '<S30>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Tracking Mode'
//  '<S31>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Tracking Mode Sum'
//  '<S32>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Tsamp - Integral'
//  '<S33>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Tsamp - Ngain'
//  '<S34>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/postSat Signal'
//  '<S35>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/preInt Signal'
//  '<S36>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/preSat Signal'
//  '<S37>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel'
//  '<S38>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S39>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External'
//  '<S40>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External/Dead Zone Dynamic'
//  '<S41>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/D Gain/External Parameters'
//  '<S42>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/External Derivative/External Ydot'
//  '<S43>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Filter/Passthrough'
//  '<S44>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Filter ICs/Disabled'
//  '<S45>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/I Gain/External Parameters'
//  '<S46>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Ideal P Gain/Passthrough'
//  '<S47>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Ideal P Gain Fdbk/Disabled'
//  '<S48>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Integrator/Discrete'
//  '<S49>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Integrator ICs/Internal IC'
//  '<S50>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/N Copy/Disabled wSignal Specification'
//  '<S51>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/N Gain/Passthrough'
//  '<S52>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/P Copy/Disabled'
//  '<S53>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Parallel P Gain/External Parameters'
//  '<S54>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Reset Signal/External Reset'
//  '<S55>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Saturation/External'
//  '<S56>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Saturation/External/Saturation Dynamic'
//  '<S57>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Saturation Fdbk/Disabled'
//  '<S58>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Sum/Sum_PID'
//  '<S59>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Sum Fdbk/Disabled'
//  '<S60>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Tracking Mode/Enabled'
//  '<S61>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Tracking Mode Sum/Tracking Mode'
//  '<S62>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S63>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/Tsamp - Ngain/Passthrough'
//  '<S64>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/postSat Signal/Forward_Path'
//  '<S65>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/preInt Signal/Internal PreInt'
//  '<S66>'  : 'platformController/Subsystem/Altitude Controller Drop/PID Controller/preSat Signal/Forward_Path'
//  '<S67>'  : 'platformController/Subsystem/Altitude Controller Hover/Compare To Constant3'
//  '<S68>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller'
//  '<S69>'  : 'platformController/Subsystem/Altitude Controller Hover/Saturation Dynamic'
//  '<S70>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Anti-windup'
//  '<S71>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/D Gain'
//  '<S72>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/External Derivative'
//  '<S73>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Filter'
//  '<S74>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Filter ICs'
//  '<S75>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/I Gain'
//  '<S76>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Ideal P Gain'
//  '<S77>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Ideal P Gain Fdbk'
//  '<S78>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Integrator'
//  '<S79>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Integrator ICs'
//  '<S80>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/N Copy'
//  '<S81>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/N Gain'
//  '<S82>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/P Copy'
//  '<S83>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Parallel P Gain'
//  '<S84>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Reset Signal'
//  '<S85>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Saturation'
//  '<S86>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Saturation Fdbk'
//  '<S87>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Sum'
//  '<S88>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Sum Fdbk'
//  '<S89>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Tracking Mode'
//  '<S90>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Tracking Mode Sum'
//  '<S91>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Tsamp - Integral'
//  '<S92>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Tsamp - Ngain'
//  '<S93>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/postSat Signal'
//  '<S94>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/preInt Signal'
//  '<S95>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/preSat Signal'
//  '<S96>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel'
//  '<S97>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S98>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External'
//  '<S99>'  : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External/Dead Zone Dynamic'
//  '<S100>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/D Gain/External Parameters'
//  '<S101>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/External Derivative/External Ydot'
//  '<S102>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Filter/Passthrough'
//  '<S103>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Filter ICs/Disabled'
//  '<S104>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/I Gain/External Parameters'
//  '<S105>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Ideal P Gain/Passthrough'
//  '<S106>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Ideal P Gain Fdbk/Disabled'
//  '<S107>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Integrator/Discrete'
//  '<S108>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Integrator ICs/Internal IC'
//  '<S109>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/N Copy/Disabled wSignal Specification'
//  '<S110>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/N Gain/Passthrough'
//  '<S111>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/P Copy/Disabled'
//  '<S112>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Parallel P Gain/External Parameters'
//  '<S113>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Reset Signal/External Reset'
//  '<S114>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Saturation/External'
//  '<S115>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Saturation/External/Saturation Dynamic'
//  '<S116>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Saturation Fdbk/Disabled'
//  '<S117>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Sum/Sum_PID'
//  '<S118>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Sum Fdbk/Disabled'
//  '<S119>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Tracking Mode/Enabled'
//  '<S120>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Tracking Mode Sum/Tracking Mode'
//  '<S121>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S122>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/Tsamp - Ngain/Passthrough'
//  '<S123>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/postSat Signal/Forward_Path'
//  '<S124>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/preInt Signal/Internal PreInt'
//  '<S125>' : 'platformController/Subsystem/Altitude Controller Hover/PID Controller/preSat Signal/Forward_Path'

#endif                                 // platformController_h_

//
// File trailer for generated code.
//
// [EOF]
//
