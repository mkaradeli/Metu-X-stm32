//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: platformController.h
//
// Code generated for Simulink model 'platformController'.
//
// Model version                  : 1.37
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Wed Aug 19 00:05:13 2026
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

#ifndef DEFINED_TYPEDEF_FOR_struct_JFTJBu46ZCgmmI8HtVUO8G_
#define DEFINED_TYPEDEF_FOR_struct_JFTJBu46ZCgmmI8HtVUO8G_

struct struct_JFTJBu46ZCgmmI8HtVUO8G
{
  real_T kP;
  real_T kI;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_IL1Ejcsec0eFI9PhCBlNqF_
#define DEFINED_TYPEDEF_FOR_struct_IL1Ejcsec0eFI9PhCBlNqF_

struct struct_IL1Ejcsec0eFI9PhCBlNqF
{
  real_T h_ref;
  real_T Kh;
  real_T v_sat[2];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_Efcp0d0Dwp641rcB9apAuB_
#define DEFINED_TYPEDEF_FOR_struct_Efcp0d0Dwp641rcB9apAuB_

struct struct_Efcp0d0Dwp641rcB9apAuB
{
  real_T V_td;
  real_T h_cut;
  real_T h_blend;
  real_T a_dec_high;
  real_T a_dec;
  real_T a_dec_low;
  real_T V_max;
  real_T Ts;
  real_T fs;
  real_T beta_max;
  real_T v_blend;
  struct_JFTJBu46ZCgmmI8HtVUO8G altitude;
  struct_IL1Ejcsec0eFI9PhCBlNqF hover;
};

#endif

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern struct_Efcp0d0Dwp641rcB9apAuB platform_targets;// Variable: platform_targets
                                                         //  Referenced by:
                                                         //    '<S2>/Bias1'
                                                         //    '<S2>/Bias2'
                                                         //    '<S2>/Constant3'
                                                         //    '<S2>/Constant4'
                                                         //    '<S2>/Gain1'
                                                         //    '<S2>/Gain2'
                                                         //    '<S2>/Saturation2'
                                                         //    '<S2>/Saturation3'
                                                         //    '<S3>/Bias1'
                                                         //    '<S3>/Constant3'
                                                         //    '<S3>/Constant4'
                                                         //    '<S3>/Gain1'
                                                         //    '<S3>/Gain5'
                                                         //    '<S3>/Saturation1'
                                                         //    '<S3>/Saturation3'
                                                         //    '<S5>/Constant'

extern mission_modes mission_mode;     // Variable: mission_mode
                                          //  Referenced by: '<S1>/Constant'


// Class declaration for model platformController
class PlatformController final
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
  PlatformController(PlatformController const&) = delete;

  // Assignment Operator
  PlatformController& operator= (PlatformController const&) & = delete;

  // Move Constructor
  PlatformController(PlatformController &&) = delete;

  // Move Assignment Operator
  PlatformController& operator= (PlatformController &&) = delete;

  // Real-Time Model get method
  PlatformController::RT_MODEL * getRTM();

  // External inputs
  ExtU rtU;

  // External outputs
  ExtY rtY;

  // model initialize function
  void initialize();

  // model step function
  void step();

  // Constructor
  PlatformController();

  // Destructor
  ~PlatformController();

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
//  '<S1>'   : 'platformController/PlatformController'
//  '<S2>'   : 'platformController/PlatformController/Altitude Controller Drop'
//  '<S3>'   : 'platformController/PlatformController/Altitude Controller Hover'
//  '<S4>'   : 'platformController/PlatformController/Attitude Controller'
//  '<S5>'   : 'platformController/PlatformController/Altitude Controller Drop/Compare To Constant'
//  '<S6>'   : 'platformController/PlatformController/Altitude Controller Drop/Compare To Constant1'
//  '<S7>'   : 'platformController/PlatformController/Altitude Controller Drop/Compare To Constant2'
//  '<S8>'   : 'platformController/PlatformController/Altitude Controller Drop/Compare To Constant3'
//  '<S9>'   : 'platformController/PlatformController/Altitude Controller Drop/PID Controller'
//  '<S10>'  : 'platformController/PlatformController/Altitude Controller Drop/Saturation Dynamic'
//  '<S11>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Anti-windup'
//  '<S12>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/D Gain'
//  '<S13>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/External Derivative'
//  '<S14>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Filter'
//  '<S15>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Filter ICs'
//  '<S16>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/I Gain'
//  '<S17>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Ideal P Gain'
//  '<S18>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Ideal P Gain Fdbk'
//  '<S19>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Integrator'
//  '<S20>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Integrator ICs'
//  '<S21>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/N Copy'
//  '<S22>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/N Gain'
//  '<S23>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/P Copy'
//  '<S24>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Parallel P Gain'
//  '<S25>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Reset Signal'
//  '<S26>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Saturation'
//  '<S27>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Saturation Fdbk'
//  '<S28>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Sum'
//  '<S29>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Sum Fdbk'
//  '<S30>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Tracking Mode'
//  '<S31>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Tracking Mode Sum'
//  '<S32>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Tsamp - Integral'
//  '<S33>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Tsamp - Ngain'
//  '<S34>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/postSat Signal'
//  '<S35>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/preInt Signal'
//  '<S36>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/preSat Signal'
//  '<S37>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel'
//  '<S38>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S39>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External'
//  '<S40>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External/Dead Zone Dynamic'
//  '<S41>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/D Gain/External Parameters'
//  '<S42>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/External Derivative/External Ydot'
//  '<S43>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Filter/Passthrough'
//  '<S44>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Filter ICs/Disabled'
//  '<S45>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/I Gain/External Parameters'
//  '<S46>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Ideal P Gain/Passthrough'
//  '<S47>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Ideal P Gain Fdbk/Disabled'
//  '<S48>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Integrator/Discrete'
//  '<S49>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Integrator ICs/Internal IC'
//  '<S50>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/N Copy/Disabled wSignal Specification'
//  '<S51>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/N Gain/Passthrough'
//  '<S52>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/P Copy/Disabled'
//  '<S53>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Parallel P Gain/External Parameters'
//  '<S54>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Reset Signal/External Reset'
//  '<S55>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Saturation/External'
//  '<S56>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Saturation/External/Saturation Dynamic'
//  '<S57>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Saturation Fdbk/Disabled'
//  '<S58>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Sum/Sum_PID'
//  '<S59>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Sum Fdbk/Disabled'
//  '<S60>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Tracking Mode/Enabled'
//  '<S61>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Tracking Mode Sum/Tracking Mode'
//  '<S62>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S63>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/Tsamp - Ngain/Passthrough'
//  '<S64>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/postSat Signal/Forward_Path'
//  '<S65>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/preInt Signal/Internal PreInt'
//  '<S66>'  : 'platformController/PlatformController/Altitude Controller Drop/PID Controller/preSat Signal/Forward_Path'
//  '<S67>'  : 'platformController/PlatformController/Altitude Controller Hover/Compare To Constant3'
//  '<S68>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller'
//  '<S69>'  : 'platformController/PlatformController/Altitude Controller Hover/Saturation Dynamic'
//  '<S70>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Anti-windup'
//  '<S71>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/D Gain'
//  '<S72>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/External Derivative'
//  '<S73>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Filter'
//  '<S74>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Filter ICs'
//  '<S75>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/I Gain'
//  '<S76>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Ideal P Gain'
//  '<S77>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Ideal P Gain Fdbk'
//  '<S78>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Integrator'
//  '<S79>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Integrator ICs'
//  '<S80>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/N Copy'
//  '<S81>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/N Gain'
//  '<S82>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/P Copy'
//  '<S83>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Parallel P Gain'
//  '<S84>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Reset Signal'
//  '<S85>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Saturation'
//  '<S86>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Saturation Fdbk'
//  '<S87>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Sum'
//  '<S88>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Sum Fdbk'
//  '<S89>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Tracking Mode'
//  '<S90>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Tracking Mode Sum'
//  '<S91>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Tsamp - Integral'
//  '<S92>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Tsamp - Ngain'
//  '<S93>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/postSat Signal'
//  '<S94>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/preInt Signal'
//  '<S95>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/preSat Signal'
//  '<S96>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel'
//  '<S97>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S98>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External'
//  '<S99>'  : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External/Dead Zone Dynamic'
//  '<S100>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/D Gain/External Parameters'
//  '<S101>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/External Derivative/External Ydot'
//  '<S102>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Filter/Passthrough'
//  '<S103>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Filter ICs/Disabled'
//  '<S104>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/I Gain/External Parameters'
//  '<S105>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Ideal P Gain/Passthrough'
//  '<S106>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Ideal P Gain Fdbk/Disabled'
//  '<S107>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Integrator/Discrete'
//  '<S108>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Integrator ICs/Internal IC'
//  '<S109>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/N Copy/Disabled wSignal Specification'
//  '<S110>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/N Gain/Passthrough'
//  '<S111>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/P Copy/Disabled'
//  '<S112>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Parallel P Gain/External Parameters'
//  '<S113>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Reset Signal/External Reset'
//  '<S114>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Saturation/External'
//  '<S115>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Saturation/External/Saturation Dynamic'
//  '<S116>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Saturation Fdbk/Disabled'
//  '<S117>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Sum/Sum_PID'
//  '<S118>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Sum Fdbk/Disabled'
//  '<S119>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Tracking Mode/Enabled'
//  '<S120>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Tracking Mode Sum/Tracking Mode'
//  '<S121>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S122>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/Tsamp - Ngain/Passthrough'
//  '<S123>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/postSat Signal/Forward_Path'
//  '<S124>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/preInt Signal/Internal PreInt'
//  '<S125>' : 'platformController/PlatformController/Altitude Controller Hover/PID Controller/preSat Signal/Forward_Path'

#endif                                 // platformController_h_

//
// File trailer for generated code.
//
// [EOF]
//
