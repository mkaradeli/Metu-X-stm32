//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: platformController.h
//
// Code generated for Simulink model 'platformController'.
//
// Model version                  : 1.116
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Wed Sep 16 22:36:40 2026
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

#ifndef DEFINED_TYPEDEF_FOR_struct_Jxr89TJYpsbjCdhVuFSiDD_
#define DEFINED_TYPEDEF_FOR_struct_Jxr89TJYpsbjCdhVuFSiDD_

struct struct_Jxr89TJYpsbjCdhVuFSiDD
{
  real_T h_ref;
  real_T Kh;
  real_T Khv;
  real_T v_sat[2];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_v979rSJRDDDrYT4G0rVPqH_
#define DEFINED_TYPEDEF_FOR_struct_v979rSJRDDDrYT4G0rVPqH_

struct struct_v979rSJRDDDrYT4G0rVPqH
{
  real_T Kp_att;
  real_T w_sat;
  real_T tilt_abort;
  real_T Kp_rate;
  real_T Ki_rate;
  real_T r;
  real_T Alloc[16];
  real_T cog[3];
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_RzX5A87yYhLUhlAxm0ffzC_
#define DEFINED_TYPEDEF_FOR_struct_RzX5A87yYhLUhlAxm0ffzC_

struct struct_RzX5A87yYhLUhlAxm0ffzC
{
  real_T V_td;
  real_T h_cut;
  real_T h_blend;
  real_T a_dec;
  real_T a_dec_low;
  real_T V_max;
  real_T Ts;
  real_T fs;
  struct_okYxSE3z6idPvgUgXDzESH altitude;
  struct_Jxr89TJYpsbjCdhVuFSiDD hover;
  struct_v979rSJRDDDrYT4G0rVPqH attitude;
};

#endif

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern struct_RzX5A87yYhLUhlAxm0ffzC platform_targets;// Variable: platform_targets
                                                         //  Referenced by:
                                                         //    '<S3>/Bias1'
                                                         //    '<S3>/Bias2'
                                                         //    '<S3>/Constant3'
                                                         //    '<S3>/Constant4'
                                                         //    '<S3>/Gain1'
                                                         //    '<S3>/Gain2'
                                                         //    '<S3>/Saturation2'
                                                         //    '<S3>/Saturation3'
                                                         //    '<S4>/Constant3'
                                                         //    '<S4>/Constant4'
                                                         //    '<S4>/Gain1'
                                                         //    '<S4>/Gain5'
                                                         //    '<S4>/Saturation1'
                                                         //    '<S4>/Saturation3'
                                                         //    '<S8>/Constant'
                                                         //    '<S10>/Constant'
                                                         //    '<S74>/Constant6'
                                                         //    '<S138>/Gain3'
                                                         //    '<S139>/Gain4'
                                                         //    '<S139>/Gain5'
                                                         //    '<S168>/Kb'
                                                         //    '<S173>/Integral Gain'
                                                         //    '<S181>/Proportional Gain'

extern mission_modes mission_mode;     // Variable: mission_mode
                                          //  Referenced by: '<S2>/Constant'


// Class declaration for model platformController
class PlatformController final
{
  // public data and function members
 public:
  // Block signals and states (default storage) for system '<Root>'
  struct DW {
    real_T Integrator_DSTATE_f[3];     // '<S176>/Integrator'
    real_T RateTransition6_Buffer[4];  // '<S2>/Rate Transition6'
    real_T Gain;                       // '<S2>/Gain'
    real_T Height;                     // '<S133>/Height'
    real_T DiscreteTimeIntegrator_DSTATE;// '<S135>/Discrete-Time Integrator'
    real_T Integrator_DSTATE;          // '<S112>/Integrator'
    real_T DiscreteTimeIntegrator_DSTATE_j;// '<S73>/Discrete-Time Integrator'
    real_T Integrator_DSTATE_p;        // '<S51>/Integrator'
    real_T RateTransition_Buffer;      // '<S2>/Rate Transition'
    real_T RateTransition4_Buffer;     // '<S2>/Rate Transition4'
    real_T RateTransition3_Buffer;     // '<S2>/Rate Transition3'
    real_T RateTransition2_Buffer;     // '<S2>/Rate Transition2'
    real_T RateTransition1_Buffer;     // '<S2>/Rate Transition1'
    int8_T DiscreteTimeIntegrator_PrevRese;// '<S135>/Discrete-Time Integrator'
    int8_T Integrator_PrevResetState;  // '<S112>/Integrator'
    int8_T DiscreteTimeIntegrator_PrevRe_j;// '<S73>/Discrete-Time Integrator'
    int8_T Integrator_PrevResetState_n;// '<S51>/Integrator'
    int8_T Integrator_PrevResetState_m;// '<S176>/Integrator'
    boolean_T Compare;                 // '<S6>/Compare'
    boolean_T DelayInput1_DSTATE;      // '<S132>/Delay Input1'
    boolean_T UnitDelay_DSTATE;        // '<S136>/Unit Delay'
    boolean_T UnitDelay_DSTATE_b;      // '<S194>/Unit Delay'
    boolean_T RateTransition5_Buffer;  // '<S2>/Rate Transition5'
  };

  // Invariant block signals (default storage)
  struct ConstB {
    boolean_T LogicalOperator;         // '<S136>/Logical Operator'
    boolean_T LogicalOperator_d;       // '<S194>/Logical Operator'
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
    real_T gyro[3];                    // '<Root>/gyro'
    real_T quaternion_bias[4];         // '<Root>/quaternion_bias'
  };

  // External outputs (root outports fed by signals with default storage)
  struct ExtY {
    real_T Fy_pos;                     // '<Root>/Fy_pos'
    real_T Fy_neg;                     // '<Root>/Fy_neg'
    real_T Fx_neg;                     // '<Root>/Fx_neg'
    real_T Fx_pos;                     // '<Root>/Fx_pos'
    real_T omega_demand[3];            // '<Root>/omega_demand'
    real_T V_target;                   // '<Root>/V_target'
    real_T a_ff;                       // '<Root>/a_ff'
    real_T a_cmd;                      // '<Root>/a_cmd'
    real_T VerticalThrustCmd;          // '<Root>/VerticalThrustCmd'
  };

  // Real-time Model Data Structure
  struct RT_MODEL {
    const char_T * volatile errorStatus;

    //
    //  Timing:
    //  The following substructure contains information regarding
    //  the timing information for the model.

    struct {
      struct {
        uint8_T TID[3];
      } TaskCounters;
    } Timing;

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

extern const PlatformController::ConstB rtConstB;// constant block i/o

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S3>/AND' : Unused code path elimination
//  Block '<S9>/Compare' : Unused code path elimination
//  Block '<S9>/Constant' : Unused code path elimination
//  Block '<S11>/Compare' : Unused code path elimination
//  Block '<S11>/Constant' : Unused code path elimination
//  Block '<S3>/Constant6' : Unused code path elimination
//  Block '<S3>/Constant7' : Unused code path elimination
//  Block '<S3>/Gain4' : Unused code path elimination
//  Block '<S59>/Data Type Duplicate' : Unused code path elimination
//  Block '<S59>/Data Type Propagation' : Unused code path elimination
//  Block '<S3>/Product' : Unused code path elimination
//  Block '<S3>/Relational Operator' : Unused code path elimination
//  Block '<S3>/Saturation' : Unused code path elimination
//  Block '<S13>/Data Type Duplicate' : Unused code path elimination
//  Block '<S13>/Data Type Propagation' : Unused code path elimination
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
//  Block '<S70>/Compare' : Unused code path elimination
//  Block '<S70>/Constant' : Unused code path elimination
//  Block '<S4>/Constant' : Unused code path elimination
//  Block '<S120>/Data Type Duplicate' : Unused code path elimination
//  Block '<S120>/Data Type Propagation' : Unused code path elimination
//  Block '<S72>/Data Type Duplicate' : Unused code path elimination
//  Block '<S72>/Data Type Propagation' : Unused code path elimination
//  Block '<S4>/Scope' : Unused code path elimination
//  Block '<S4>/Scope1' : Unused code path elimination
//  Block '<S4>/Scope3' : Unused code path elimination
//  Block '<S4>/Scope4' : Unused code path elimination
//  Block '<S4>/Scope5' : Unused code path elimination
//  Block '<S4>/Scope8' : Unused code path elimination
//  Block '<S140>/Data Type Duplicate' : Unused code path elimination
//  Block '<S140>/Data Type Propagation' : Unused code path elimination
//  Block '<S138>/Scope' : Unused code path elimination
//  Block '<S139>/Scope' : Unused code path elimination
//  Block '<S139>/Scope1' : Unused code path elimination
//  Block '<S139>/Scope2' : Unused code path elimination
//  Block '<S5>/Scope' : Unused code path elimination
//  Block '<S5>/Scope1' : Unused code path elimination
//  Block '<S2>/Cast To Boolean' : Unused code path elimination
//  Block '<S2>/Cast To Boolean1' : Unused code path elimination
//  Block '<S2>/Cast To Boolean2' : Unused code path elimination
//  Block '<S194>/Logical Operator3' : Unused code path elimination
//  Block '<S63>/Kt' : Eliminated nontunable gain of 1
//  Block '<S124>/Kt' : Eliminated nontunable gain of 1
//  Block '<S136>/Data Type Conversion' : Eliminate redundant data type conversion
//  Block '<S136>/Data Type Conversion1' : Eliminate redundant data type conversion
//  Block '<S5>/Rate Transition' : Eliminated since input and output rates are identical
//  Block '<S5>/Rate Transition6' : Eliminated since input and output rates are identical
//  Block '<S2>/Cast To Boolean3' : Eliminate redundant data type conversion
//  Block '<S194>/Data Type Conversion' : Eliminate redundant data type conversion
//  Block '<S194>/Data Type Conversion1' : Eliminate redundant data type conversion
//  Block '<S136>/Constant' : Unused code path elimination
//  Block '<S136>/Logical Operator1' : Unused code path elimination
//  Block '<S136>/Logical Operator2' : Unused code path elimination
//  Block '<S194>/Constant' : Unused code path elimination
//  Block '<S194>/Logical Operator1' : Unused code path elimination
//  Block '<S194>/Logical Operator2' : Unused code path elimination


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
//  '<S2>'   : 'platformController/PlatformController/PlatformController1'
//  '<S3>'   : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop'
//  '<S4>'   : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover'
//  '<S5>'   : 'platformController/PlatformController/PlatformController1/Attitude Controller'
//  '<S6>'   : 'platformController/PlatformController/PlatformController1/Compare To Constant'
//  '<S7>'   : 'platformController/PlatformController/PlatformController1/Set-Reset Flip-Flop'
//  '<S8>'   : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/Compare To Constant'
//  '<S9>'   : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/Compare To Constant1'
//  '<S10>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/Compare To Constant2'
//  '<S11>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/Compare To Constant3'
//  '<S12>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller'
//  '<S13>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/Saturation Dynamic'
//  '<S14>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Anti-windup'
//  '<S15>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/D Gain'
//  '<S16>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/External Derivative'
//  '<S17>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Filter'
//  '<S18>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Filter ICs'
//  '<S19>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/I Gain'
//  '<S20>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Ideal P Gain'
//  '<S21>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Ideal P Gain Fdbk'
//  '<S22>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Integrator'
//  '<S23>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Integrator ICs'
//  '<S24>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/N Copy'
//  '<S25>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/N Gain'
//  '<S26>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/P Copy'
//  '<S27>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Parallel P Gain'
//  '<S28>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Reset Signal'
//  '<S29>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Saturation'
//  '<S30>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Saturation Fdbk'
//  '<S31>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Sum'
//  '<S32>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Sum Fdbk'
//  '<S33>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Tracking Mode'
//  '<S34>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Tracking Mode Sum'
//  '<S35>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Tsamp - Integral'
//  '<S36>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Tsamp - Ngain'
//  '<S37>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/postSat Signal'
//  '<S38>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/preInt Signal'
//  '<S39>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/preSat Signal'
//  '<S40>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel'
//  '<S41>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S42>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External'
//  '<S43>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External/Dead Zone Dynamic'
//  '<S44>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/D Gain/External Parameters'
//  '<S45>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/External Derivative/External Ydot'
//  '<S46>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Filter/Passthrough'
//  '<S47>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Filter ICs/Disabled'
//  '<S48>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/I Gain/External Parameters'
//  '<S49>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Ideal P Gain/Passthrough'
//  '<S50>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Ideal P Gain Fdbk/Disabled'
//  '<S51>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Integrator/Discrete'
//  '<S52>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Integrator ICs/Internal IC'
//  '<S53>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/N Copy/Disabled wSignal Specification'
//  '<S54>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/N Gain/Passthrough'
//  '<S55>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/P Copy/Disabled'
//  '<S56>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Parallel P Gain/External Parameters'
//  '<S57>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Reset Signal/External Reset'
//  '<S58>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Saturation/External'
//  '<S59>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Saturation/External/Saturation Dynamic'
//  '<S60>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Saturation Fdbk/Disabled'
//  '<S61>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Sum/Sum_PID'
//  '<S62>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Sum Fdbk/Disabled'
//  '<S63>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Tracking Mode/Enabled'
//  '<S64>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Tracking Mode Sum/Tracking Mode'
//  '<S65>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S66>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Tsamp - Ngain/Passthrough'
//  '<S67>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/postSat Signal/Forward_Path'
//  '<S68>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/preInt Signal/Internal PreInt'
//  '<S69>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/preSat Signal/Forward_Path'
//  '<S70>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Compare To Constant3'
//  '<S71>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller'
//  '<S72>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Saturation Dynamic'
//  '<S73>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem'
//  '<S74>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem1'
//  '<S75>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Anti-windup'
//  '<S76>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/D Gain'
//  '<S77>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/External Derivative'
//  '<S78>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Filter'
//  '<S79>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Filter ICs'
//  '<S80>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/I Gain'
//  '<S81>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Ideal P Gain'
//  '<S82>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Ideal P Gain Fdbk'
//  '<S83>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Integrator'
//  '<S84>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Integrator ICs'
//  '<S85>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/N Copy'
//  '<S86>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/N Gain'
//  '<S87>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/P Copy'
//  '<S88>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Parallel P Gain'
//  '<S89>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Reset Signal'
//  '<S90>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Saturation'
//  '<S91>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Saturation Fdbk'
//  '<S92>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Sum'
//  '<S93>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Sum Fdbk'
//  '<S94>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Tracking Mode'
//  '<S95>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Tracking Mode Sum'
//  '<S96>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Tsamp - Integral'
//  '<S97>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Tsamp - Ngain'
//  '<S98>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/postSat Signal'
//  '<S99>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/preInt Signal'
//  '<S100>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/preSat Signal'
//  '<S101>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel'
//  '<S102>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S103>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External'
//  '<S104>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External/Dead Zone Dynamic'
//  '<S105>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/D Gain/External Parameters'
//  '<S106>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/External Derivative/External Ydot'
//  '<S107>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Filter/Passthrough'
//  '<S108>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Filter ICs/Disabled'
//  '<S109>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/I Gain/External Parameters'
//  '<S110>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Ideal P Gain/Passthrough'
//  '<S111>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Ideal P Gain Fdbk/Disabled'
//  '<S112>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Integrator/Discrete'
//  '<S113>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Integrator ICs/Internal IC'
//  '<S114>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/N Copy/Disabled wSignal Specification'
//  '<S115>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/N Gain/Passthrough'
//  '<S116>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/P Copy/Disabled'
//  '<S117>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Parallel P Gain/External Parameters'
//  '<S118>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Reset Signal/External Reset'
//  '<S119>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Saturation/External'
//  '<S120>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Saturation/External/Saturation Dynamic'
//  '<S121>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Saturation Fdbk/Disabled'
//  '<S122>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Sum/Sum_PID'
//  '<S123>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Sum Fdbk/Disabled'
//  '<S124>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Tracking Mode/Enabled'
//  '<S125>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Tracking Mode Sum/Tracking Mode'
//  '<S126>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S127>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Tsamp - Ngain/Passthrough'
//  '<S128>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/postSat Signal/Forward_Path'
//  '<S129>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/preInt Signal/Internal PreInt'
//  '<S130>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/preSat Signal/Forward_Path'
//  '<S131>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem/Compare To Constant'
//  '<S132>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem1/Detect Increase'
//  '<S133>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem1/Enabled Subsystem'
//  '<S134>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem1/Set-Reset Flip-Flop'
//  '<S135>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem1/Subsystem'
//  '<S136>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem1/Set-Reset Flip-Flop/Discrete'
//  '<S137>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem1/Subsystem/Compare To Constant'
//  '<S138>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Position Loop'
//  '<S139>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop'
//  '<S140>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Position Loop/Saturation Dynamic'
//  '<S141>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller'
//  '<S142>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Anti-windup'
//  '<S143>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/D Gain'
//  '<S144>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/External Derivative'
//  '<S145>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Filter'
//  '<S146>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Filter ICs'
//  '<S147>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/I Gain'
//  '<S148>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Ideal P Gain'
//  '<S149>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Ideal P Gain Fdbk'
//  '<S150>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Integrator'
//  '<S151>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Integrator ICs'
//  '<S152>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/N Copy'
//  '<S153>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/N Gain'
//  '<S154>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/P Copy'
//  '<S155>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Parallel P Gain'
//  '<S156>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Reset Signal'
//  '<S157>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Saturation'
//  '<S158>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Saturation Fdbk'
//  '<S159>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Sum'
//  '<S160>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Sum Fdbk'
//  '<S161>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Tracking Mode'
//  '<S162>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Tracking Mode Sum'
//  '<S163>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Tsamp - Integral'
//  '<S164>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Tsamp - Ngain'
//  '<S165>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/postSat Signal'
//  '<S166>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/preInt Signal'
//  '<S167>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/preSat Signal'
//  '<S168>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Anti-windup/Back Calculation'
//  '<S169>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/D Gain/Disabled'
//  '<S170>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/External Derivative/Disabled'
//  '<S171>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Filter/Disabled'
//  '<S172>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Filter ICs/Disabled'
//  '<S173>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/I Gain/Internal Parameters'
//  '<S174>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Ideal P Gain/Passthrough'
//  '<S175>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Ideal P Gain Fdbk/Disabled'
//  '<S176>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Integrator/Discrete'
//  '<S177>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Integrator ICs/Internal IC'
//  '<S178>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/N Copy/Disabled wSignal Specification'
//  '<S179>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/N Gain/Disabled'
//  '<S180>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/P Copy/Disabled'
//  '<S181>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Parallel P Gain/Internal Parameters'
//  '<S182>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Reset Signal/External Reset'
//  '<S183>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Saturation/Enabled'
//  '<S184>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Saturation Fdbk/Disabled'
//  '<S185>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Sum/Sum_PI'
//  '<S186>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Sum Fdbk/Disabled'
//  '<S187>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Tracking Mode/Disabled'
//  '<S188>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Tracking Mode Sum/Passthrough'
//  '<S189>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S190>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Tsamp - Ngain/Passthrough'
//  '<S191>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/postSat Signal/Forward_Path'
//  '<S192>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/preInt Signal/Internal PreInt'
//  '<S193>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/preSat Signal/Forward_Path'
//  '<S194>' : 'platformController/PlatformController/PlatformController1/Set-Reset Flip-Flop/Discrete'

#endif                                 // platformController_h_

//
// File trailer for generated code.
//
// [EOF]
//
