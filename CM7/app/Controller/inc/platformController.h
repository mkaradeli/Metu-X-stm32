//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: platformController.h
//
// Code generated for Simulink model 'platformController'.
//
// Model version                  : 1.64
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Thu Sep  3 22:08:56 2026
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

#ifndef DEFINED_TYPEDEF_FOR_struct_okYxSE3z6idPvgUgXDzESH_
#define DEFINED_TYPEDEF_FOR_struct_okYxSE3z6idPvgUgXDzESH_

struct struct_okYxSE3z6idPvgUgXDzESH
{
  real_T kP;
  real_T kI;
  real_T kP_hover;
  real_T kI_hover;
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

#ifndef DEFINED_TYPEDEF_FOR_struct_YbobqjaKaPadeQYd94tngB_
#define DEFINED_TYPEDEF_FOR_struct_YbobqjaKaPadeQYd94tngB_

struct struct_YbobqjaKaPadeQYd94tngB
{
  real_T Kp_att;
  real_T w_sat;
  real_T tilt_abort;
  real_T Kp_rate;
  real_T Ki_rate;
  real_T r;
  real_T Alloc[16];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_V02IQCW8z51sc9ktARnSeC_
#define DEFINED_TYPEDEF_FOR_struct_V02IQCW8z51sc9ktARnSeC_

struct struct_V02IQCW8z51sc9ktARnSeC
{
  real_T V_td;
  real_T h_cut;
  real_T h_blend;
  real_T a_dec;
  real_T a_dec_high;
  real_T a_dec_low;
  real_T V_max;
  real_T Ts;
  real_T fs;
  real_T v_blend;
  struct_okYxSE3z6idPvgUgXDzESH altitude;
  struct_IL1Ejcsec0eFI9PhCBlNqF hover;
  struct_YbobqjaKaPadeQYd94tngB attitude;
};

#endif

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern struct_V02IQCW8z51sc9ktARnSeC platform_targets;// Variable: platform_targets
                                                         //  Referenced by:
                                                         //    '<S3>/Bias1'
                                                         //    '<S3>/Bias2'
                                                         //    '<S3>/Constant3'
                                                         //    '<S3>/Constant4'
                                                         //    '<S3>/Gain1'
                                                         //    '<S3>/Gain2'
                                                         //    '<S3>/Saturation2'
                                                         //    '<S3>/Saturation3'
                                                         //    '<S4>/Bias1'
                                                         //    '<S4>/Constant3'
                                                         //    '<S4>/Constant4'
                                                         //    '<S4>/Gain1'
                                                         //    '<S4>/Gain5'
                                                         //    '<S4>/Saturation1'
                                                         //    '<S4>/Saturation3'
                                                         //    '<S6>/Constant'
                                                         //    '<S127>/Constant'
                                                         //    '<S127>/Gain3'
                                                         //    '<S128>/Gain4'
                                                         //    '<S128>/Gain5'
                                                         //    '<S162>/Integral Gain'
                                                         //    '<S170>/Proportional Gain'

extern mission_modes mission_mode;     // Variable: mission_mode
                                          //  Referenced by: '<S2>/Constant'


// Class declaration for model platformController
class PlatformController final
{
  // public data and function members
 public:
  // Block signals and states (default storage) for system '<Root>'
  struct DW {
    real_T Integrator_DSTATE[3];       // '<S165>/Integrator'
    real_T Integrator_DSTATE_a;        // '<S108>/Integrator'
    real_T Integrator_DSTATE_m;        // '<S49>/Integrator'
    int8_T Integrator_PrevResetState;  // '<S108>/Integrator'
    int8_T Integrator_PrevResetState_o;// '<S49>/Integrator'
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
    real_T gyro_x;                     // '<Root>/gyro_x'
    real_T gyro_y;                     // '<Root>/gyro_y'
    real_T gyro_z;                     // '<Root>/gyro_z'
  };

  // External outputs (root outports fed by signals with default storage)
  struct ExtY {
    real_T FrontThrustCmd;             // '<Root>/FrontThrustCmd'
    real_T BackThrustCmd;              // '<Root>/BackThrustCmd'
    real_T LeftThrustCmd;              // '<Root>/LeftThrustCmd'
    real_T RightThrustCmd;             // '<Root>/RightThrustCmd'
    real_T Mxyz[3];                    // '<Root>/Mxyz'
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
//  Block '<S3>/AND' : Unused code path elimination
//  Block '<S7>/Compare' : Unused code path elimination
//  Block '<S7>/Constant' : Unused code path elimination
//  Block '<S9>/Compare' : Unused code path elimination
//  Block '<S9>/Constant' : Unused code path elimination
//  Block '<S3>/Constant6' : Unused code path elimination
//  Block '<S3>/Constant7' : Unused code path elimination
//  Block '<S3>/Gain4' : Unused code path elimination
//  Block '<S57>/Data Type Duplicate' : Unused code path elimination
//  Block '<S57>/Data Type Propagation' : Unused code path elimination
//  Block '<S3>/Product' : Unused code path elimination
//  Block '<S3>/Relational Operator' : Unused code path elimination
//  Block '<S3>/Saturation' : Unused code path elimination
//  Block '<S11>/Data Type Duplicate' : Unused code path elimination
//  Block '<S11>/Data Type Propagation' : Unused code path elimination
//  Block '<S3>/Scope' : Unused code path elimination
//  Block '<S3>/Scope1' : Unused code path elimination
//  Block '<S3>/Scope2' : Unused code path elimination
//  Block '<S3>/Scope3' : Unused code path elimination
//  Block '<S3>/Scope4' : Unused code path elimination
//  Block '<S3>/Scope5' : Unused code path elimination
//  Block '<S3>/Scope6' : Unused code path elimination
//  Block '<S3>/Scope7' : Unused code path elimination
//  Block '<S3>/Scope8' : Unused code path elimination
//  Block '<S3>/Square2' : Unused code path elimination
//  Block '<S3>/Sum2' : Unused code path elimination
//  Block '<S3>/Sum3' : Unused code path elimination
//  Block '<S68>/Compare' : Unused code path elimination
//  Block '<S68>/Constant' : Unused code path elimination
//  Block '<S4>/Constant' : Unused code path elimination
//  Block '<S116>/Data Type Duplicate' : Unused code path elimination
//  Block '<S116>/Data Type Propagation' : Unused code path elimination
//  Block '<S70>/Data Type Duplicate' : Unused code path elimination
//  Block '<S70>/Data Type Propagation' : Unused code path elimination
//  Block '<S4>/Scope' : Unused code path elimination
//  Block '<S4>/Scope1' : Unused code path elimination
//  Block '<S4>/Scope3' : Unused code path elimination
//  Block '<S4>/Scope4' : Unused code path elimination
//  Block '<S4>/Scope5' : Unused code path elimination
//  Block '<S4>/Scope8' : Unused code path elimination
//  Block '<S2>/Cast To Boolean' : Unused code path elimination
//  Block '<S2>/Cast To Boolean1' : Unused code path elimination
//  Block '<S2>/Cast To Boolean2' : Unused code path elimination
//  Block '<S2>/Cast To Boolean3' : Unused code path elimination
//  Block '<S61>/Kt' : Eliminated nontunable gain of 1
//  Block '<S120>/Kt' : Eliminated nontunable gain of 1


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
//  '<S2>'   : 'platformController/PlatformController/PlatformController'
//  '<S3>'   : 'platformController/PlatformController/PlatformController/Altitude Controller Drop'
//  '<S4>'   : 'platformController/PlatformController/PlatformController/Altitude Controller Hover'
//  '<S5>'   : 'platformController/PlatformController/PlatformController/Attitude Controller'
//  '<S6>'   : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/Compare To Constant'
//  '<S7>'   : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/Compare To Constant1'
//  '<S8>'   : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/Compare To Constant2'
//  '<S9>'   : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/Compare To Constant3'
//  '<S10>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller'
//  '<S11>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/Saturation Dynamic'
//  '<S12>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Anti-windup'
//  '<S13>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/D Gain'
//  '<S14>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/External Derivative'
//  '<S15>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Filter'
//  '<S16>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Filter ICs'
//  '<S17>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/I Gain'
//  '<S18>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Ideal P Gain'
//  '<S19>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Ideal P Gain Fdbk'
//  '<S20>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Integrator'
//  '<S21>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Integrator ICs'
//  '<S22>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/N Copy'
//  '<S23>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/N Gain'
//  '<S24>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/P Copy'
//  '<S25>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Parallel P Gain'
//  '<S26>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Reset Signal'
//  '<S27>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Saturation'
//  '<S28>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Saturation Fdbk'
//  '<S29>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Sum'
//  '<S30>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Sum Fdbk'
//  '<S31>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Tracking Mode'
//  '<S32>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Tracking Mode Sum'
//  '<S33>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Tsamp - Integral'
//  '<S34>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Tsamp - Ngain'
//  '<S35>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/postSat Signal'
//  '<S36>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/preInt Signal'
//  '<S37>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/preSat Signal'
//  '<S38>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel'
//  '<S39>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S40>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External'
//  '<S41>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External/Dead Zone Dynamic'
//  '<S42>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/D Gain/External Parameters'
//  '<S43>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/External Derivative/External Ydot'
//  '<S44>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Filter/Passthrough'
//  '<S45>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Filter ICs/Disabled'
//  '<S46>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/I Gain/External Parameters'
//  '<S47>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Ideal P Gain/Passthrough'
//  '<S48>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Ideal P Gain Fdbk/Disabled'
//  '<S49>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Integrator/Discrete'
//  '<S50>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Integrator ICs/Internal IC'
//  '<S51>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/N Copy/Disabled wSignal Specification'
//  '<S52>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/N Gain/Passthrough'
//  '<S53>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/P Copy/Disabled'
//  '<S54>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Parallel P Gain/External Parameters'
//  '<S55>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Reset Signal/External Reset'
//  '<S56>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Saturation/External'
//  '<S57>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Saturation/External/Saturation Dynamic'
//  '<S58>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Saturation Fdbk/Disabled'
//  '<S59>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Sum/Sum_PID'
//  '<S60>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Sum Fdbk/Disabled'
//  '<S61>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Tracking Mode/Enabled'
//  '<S62>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Tracking Mode Sum/Tracking Mode'
//  '<S63>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S64>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/Tsamp - Ngain/Passthrough'
//  '<S65>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/postSat Signal/Forward_Path'
//  '<S66>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/preInt Signal/Internal PreInt'
//  '<S67>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Drop/PID Controller/preSat Signal/Forward_Path'
//  '<S68>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/Compare To Constant3'
//  '<S69>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller'
//  '<S70>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/Saturation Dynamic'
//  '<S71>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Anti-windup'
//  '<S72>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/D Gain'
//  '<S73>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/External Derivative'
//  '<S74>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Filter'
//  '<S75>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Filter ICs'
//  '<S76>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/I Gain'
//  '<S77>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Ideal P Gain'
//  '<S78>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Ideal P Gain Fdbk'
//  '<S79>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Integrator'
//  '<S80>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Integrator ICs'
//  '<S81>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/N Copy'
//  '<S82>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/N Gain'
//  '<S83>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/P Copy'
//  '<S84>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Parallel P Gain'
//  '<S85>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Reset Signal'
//  '<S86>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Saturation'
//  '<S87>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Saturation Fdbk'
//  '<S88>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Sum'
//  '<S89>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Sum Fdbk'
//  '<S90>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Tracking Mode'
//  '<S91>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Tracking Mode Sum'
//  '<S92>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Tsamp - Integral'
//  '<S93>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Tsamp - Ngain'
//  '<S94>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/postSat Signal'
//  '<S95>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/preInt Signal'
//  '<S96>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/preSat Signal'
//  '<S97>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel'
//  '<S98>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S99>'  : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External'
//  '<S100>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External/Dead Zone Dynamic'
//  '<S101>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/D Gain/External Parameters'
//  '<S102>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/External Derivative/External Ydot'
//  '<S103>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Filter/Passthrough'
//  '<S104>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Filter ICs/Disabled'
//  '<S105>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/I Gain/External Parameters'
//  '<S106>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Ideal P Gain/Passthrough'
//  '<S107>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Ideal P Gain Fdbk/Disabled'
//  '<S108>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Integrator/Discrete'
//  '<S109>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Integrator ICs/Internal IC'
//  '<S110>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/N Copy/Disabled wSignal Specification'
//  '<S111>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/N Gain/Passthrough'
//  '<S112>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/P Copy/Disabled'
//  '<S113>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Parallel P Gain/External Parameters'
//  '<S114>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Reset Signal/External Reset'
//  '<S115>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Saturation/External'
//  '<S116>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Saturation/External/Saturation Dynamic'
//  '<S117>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Saturation Fdbk/Disabled'
//  '<S118>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Sum/Sum_PID'
//  '<S119>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Sum Fdbk/Disabled'
//  '<S120>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Tracking Mode/Enabled'
//  '<S121>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Tracking Mode Sum/Tracking Mode'
//  '<S122>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S123>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/Tsamp - Ngain/Passthrough'
//  '<S124>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/postSat Signal/Forward_Path'
//  '<S125>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/preInt Signal/Internal PreInt'
//  '<S126>' : 'platformController/PlatformController/PlatformController/Altitude Controller Hover/PID Controller/preSat Signal/Forward_Path'
//  '<S127>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Position Loop'
//  '<S128>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop'
//  '<S129>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Position Loop/Cross Product'
//  '<S130>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller'
//  '<S131>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Anti-windup'
//  '<S132>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/D Gain'
//  '<S133>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/External Derivative'
//  '<S134>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Filter'
//  '<S135>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Filter ICs'
//  '<S136>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/I Gain'
//  '<S137>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Ideal P Gain'
//  '<S138>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Ideal P Gain Fdbk'
//  '<S139>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Integrator'
//  '<S140>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Integrator ICs'
//  '<S141>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/N Copy'
//  '<S142>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/N Gain'
//  '<S143>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/P Copy'
//  '<S144>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Parallel P Gain'
//  '<S145>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Reset Signal'
//  '<S146>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Saturation'
//  '<S147>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Saturation Fdbk'
//  '<S148>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Sum'
//  '<S149>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Sum Fdbk'
//  '<S150>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Tracking Mode'
//  '<S151>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Tracking Mode Sum'
//  '<S152>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Tsamp - Integral'
//  '<S153>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Tsamp - Ngain'
//  '<S154>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/postSat Signal'
//  '<S155>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/preInt Signal'
//  '<S156>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/preSat Signal'
//  '<S157>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Anti-windup/Passthrough'
//  '<S158>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/D Gain/Disabled'
//  '<S159>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/External Derivative/Disabled'
//  '<S160>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Filter/Disabled'
//  '<S161>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Filter ICs/Disabled'
//  '<S162>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/I Gain/Internal Parameters'
//  '<S163>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Ideal P Gain/Passthrough'
//  '<S164>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Ideal P Gain Fdbk/Disabled'
//  '<S165>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Integrator/Discrete'
//  '<S166>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Integrator ICs/Internal IC'
//  '<S167>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/N Copy/Disabled wSignal Specification'
//  '<S168>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/N Gain/Disabled'
//  '<S169>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/P Copy/Disabled'
//  '<S170>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Parallel P Gain/Internal Parameters'
//  '<S171>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Reset Signal/Disabled'
//  '<S172>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Saturation/Passthrough'
//  '<S173>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Saturation Fdbk/Disabled'
//  '<S174>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Sum/Sum_PI'
//  '<S175>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Sum Fdbk/Disabled'
//  '<S176>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Tracking Mode/Disabled'
//  '<S177>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Tracking Mode Sum/Passthrough'
//  '<S178>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S179>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Tsamp - Ngain/Passthrough'
//  '<S180>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/postSat Signal/Forward_Path'
//  '<S181>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/preInt Signal/Internal PreInt'
//  '<S182>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/preSat Signal/Forward_Path'

#endif                                 // platformController_h_

//
// File trailer for generated code.
//
// [EOF]
//
