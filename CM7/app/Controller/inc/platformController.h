//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: platformController.h
//
// Code generated for Simulink model 'platformController'.
//
// Model version                  : 1.77
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Sun Sep  6 18:20:35 2026
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

#ifndef DEFINED_TYPEDEF_FOR_struct_RdUhJaXg9zE9UN7ulmGlDE_
#define DEFINED_TYPEDEF_FOR_struct_RdUhJaXg9zE9UN7ulmGlDE_

struct struct_RdUhJaXg9zE9UN7ulmGlDE
{
  real_T empty;
  real_T full;
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
                                          //  Referenced by: '<S2>/Constant'


// Class declaration for model platformController
class PlatformController final
{
  // public data and function members
 public:
  // Block signals and states (default storage) for system '<Root>'
  struct DW {
    real_T RateTransition1[3];         // '<S5>/Rate Transition1'
    real_T Integrator_DSTATE[3];       // '<S164>/Integrator'
    real_T RateTransition1_Buffer0[3]; // '<S5>/Rate Transition1'
    real_T RateTransition6_Buffer[4];  // '<S2>/Rate Transition6'
    real_T RateTransition6_Buffer_c[4];// '<S5>/Rate Transition6'
    real_T Gain;                       // '<S2>/Gain'
    real_T Integrator_DSTATE_a;        // '<S108>/Integrator'
    real_T Integrator_DSTATE_m;        // '<S49>/Integrator'
    real_T RateTransition_Buffer;      // '<S2>/Rate Transition'
    real_T RateTransition4_Buffer;     // '<S2>/Rate Transition4'
    real_T RateTransition3_Buffer;     // '<S2>/Rate Transition3'
    real_T RateTransition2_Buffer;     // '<S2>/Rate Transition2'
    real_T RateTransition1_Buffer;     // '<S2>/Rate Transition1'
    int8_T Integrator_PrevResetState;  // '<S108>/Integrator'
    int8_T Integrator_PrevResetState_o;// '<S49>/Integrator'
    boolean_T RateTransition5_Buffer;  // '<S2>/Rate Transition5'
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
    real_T angularVelocity[3];         // '<Root>/angularVelocity'
  };

  // External outputs (root outports fed by signals with default storage)
  struct ExtY {
    real_T Fy_pos;                     // '<Root>/Fy_pos'
    real_T Fy_neg;                     // '<Root>/Fy_neg'
    real_T Fx_neg;                     // '<Root>/Fx_neg'
    real_T Fx_pos;                     // '<Root>/Fx_pos'
    real_T omega_demand[3];            // '<Root>/omega_demand'
  };

  // Parameters (default storage)
  struct P {
    struct_V02IQCW8z51sc9ktARnSeC platform_targets;// Variable: platform_targets
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
                                                      //    '<S161>/Integral Gain'
                                                      //    '<S169>/Proportional Gain'

    struct_RdUhJaXg9zE9UN7ulmGlDE mass;// Variable: mass
                                          //  Referenced by:
                                          //    '<S3>/Bias'
                                          //    '<S4>/Bias'

    real_T PIDController_InitialConditionF;
                              // Mask Parameter: PIDController_InitialConditionF
                                 //  Referenced by: '<S164>/Integrator'

    real_T PIDController_InitialConditio_c;
                              // Mask Parameter: PIDController_InitialConditio_c
                                 //  Referenced by: '<S108>/Integrator'

    real_T PIDController_InitialConditio_m;
                              // Mask Parameter: PIDController_InitialConditio_m
                                 //  Referenced by: '<S49>/Integrator'

    real_T PIDController_Kt;           // Mask Parameter: PIDController_Kt
                                          //  Referenced by: '<S61>/Kt'

    real_T PIDController_Kt_n;         // Mask Parameter: PIDController_Kt_n
                                          //  Referenced by: '<S120>/Kt'

    real_T PIDController_LowerIntegratorSa;
                              // Mask Parameter: PIDController_LowerIntegratorSa
                                 //  Referenced by: '<S108>/Integrator'

    real_T PIDController_LowerIntegrator_j;
                              // Mask Parameter: PIDController_LowerIntegrator_j
                                 //  Referenced by: '<S49>/Integrator'

    real_T PIDController_UpperIntegratorSa;
                              // Mask Parameter: PIDController_UpperIntegratorSa
                                 //  Referenced by: '<S108>/Integrator'

    real_T PIDController_UpperIntegrator_h;
                              // Mask Parameter: PIDController_UpperIntegrator_h
                                 //  Referenced by: '<S49>/Integrator'

    real_T CompareToConstant2_const; // Mask Parameter: CompareToConstant2_const
                                        //  Referenced by: '<S8>/Constant'

    real_T Constant_Value;             // Expression: -9.8
                                          //  Referenced by: '<S3>/Constant'

    real_T Constant5_Value;            // Expression: 0
                                          //  Referenced by: '<S4>/Constant5'

    real_T Bias3_Bias;                 // Expression: 9.81
                                          //  Referenced by: '<S4>/Bias3'

    real_T Constant8_Value;            // Expression: 0
                                          //  Referenced by: '<S3>/Constant8'

    real_T Constant5_Value_e;          // Expression: 0
                                          //  Referenced by: '<S3>/Constant5'

    real_T Bias3_Bias_o;               // Expression: 9.81
                                          //  Referenced by: '<S3>/Bias3'

    real_T Constant1_Value;            // Expression: 0
                                          //  Referenced by: '<S2>/Constant1'

    real_T Constant1_Value_c;          // Expression: 0
                                          //  Referenced by: '<S38>/Constant1'

    real_T Constant1_Value_g;          // Expression: 0
                                          //  Referenced by: '<S97>/Constant1'

    real_T Integrator_gainval;         // Computed Parameter: Integrator_gainval
                                          //  Referenced by: '<S164>/Integrator'

    real_T RateTransition1_InitialConditio;// Expression: 0
                                              //  Referenced by: '<S5>/Rate Transition1'

    real_T Saturation_UpperSat;        // Expression: 272
                                          //  Referenced by: '<S128>/Saturation'

    real_T Saturation_LowerSat;        // Expression: -272
                                          //  Referenced by: '<S128>/Saturation'

    real_T Gain_Gain;                  // Expression: 2.85/300
                                          //  Referenced by: '<S4>/Gain'

    real_T Gain3_Gain;                 // Expression: -2
                                          //  Referenced by: '<S4>/Gain3'

    real_T Bias5_Bias;                 // Expression: 1
                                          //  Referenced by: '<S4>/Bias5'

    real_T Saturation4_UpperSat;       // Expression: 1
                                          //  Referenced by: '<S4>/Saturation4'

    real_T Saturation4_LowerSat;       // Expression: 0.7
                                          //  Referenced by: '<S4>/Saturation4'

    real_T Bias4_Bias;                 // Expression: -9.81
                                          //  Referenced by: '<S4>/Bias4'

    real_T Integrator_gainval_c;     // Computed Parameter: Integrator_gainval_c
                                        //  Referenced by: '<S108>/Integrator'

    real_T Saturation3_LowerSat;       // Expression: -9.81
                                          //  Referenced by: '<S4>/Saturation3'

    real_T Constant2_Value;            // Expression: -1
                                          //  Referenced by: '<S4>/Constant2'

    real_T Constant1_Value_l;          // Expression: -9.81
                                          //  Referenced by: '<S4>/Constant1'

    real_T Gain_Gain_i;                // Expression: 2.85/300
                                          //  Referenced by: '<S3>/Gain'

    real_T Gain3_Gain_j;               // Expression: -2
                                          //  Referenced by: '<S3>/Gain3'

    real_T Bias5_Bias_i;               // Expression: 1
                                          //  Referenced by: '<S3>/Bias5'

    real_T Saturation4_UpperSat_l;     // Expression: 1
                                          //  Referenced by: '<S3>/Saturation4'

    real_T Saturation4_LowerSat_m;     // Expression: 0.7
                                          //  Referenced by: '<S3>/Saturation4'

    real_T Bias4_Bias_i;               // Expression: -9.81
                                          //  Referenced by: '<S3>/Bias4'

    real_T Saturation1_UpperSat;       // Expression: inf
                                          //  Referenced by: '<S3>/Saturation1'

    real_T Saturation1_LowerSat;       // Expression: 0
                                          //  Referenced by: '<S3>/Saturation1'

    real_T Saturation2_LowerSat;       // Expression: -inf
                                          //  Referenced by: '<S3>/Saturation2'

    real_T Integrator_gainval_cz;   // Computed Parameter: Integrator_gainval_cz
                                       //  Referenced by: '<S49>/Integrator'

    real_T Saturation3_LowerSat_a;     // Expression: -inf
                                          //  Referenced by: '<S3>/Saturation3'

    real_T Constant2_Value_c;          // Expression: -1
                                          //  Referenced by: '<S3>/Constant2'

    real_T Constant1_Value_h;          // Expression: -9.81
                                          //  Referenced by: '<S3>/Constant1'

    real_T Gain_Gain_f;                // Expression: 1/4
                                          //  Referenced by: '<S2>/Gain'

    real_T Saturation1_UpperSat_b;     // Expression: 272
                                          //  Referenced by: '<S128>/Saturation1'

    real_T Saturation1_LowerSat_j;     // Expression: -272
                                          //  Referenced by: '<S128>/Saturation1'

    real_T Saturation2_UpperSat;       // Expression: 272
                                          //  Referenced by: '<S128>/Saturation2'

    real_T Saturation2_LowerSat_d;     // Expression: -272
                                          //  Referenced by: '<S128>/Saturation2'

    real_T Saturation3_UpperSat;       // Expression: 272
                                          //  Referenced by: '<S128>/Saturation3'

    real_T Saturation3_LowerSat_i;     // Expression: -272
                                          //  Referenced by: '<S128>/Saturation3'

    real_T Bias6_Bias;                 // Expression: -9.81
                                          //  Referenced by: '<S3>/Bias6'

    real_T Clamping_zero_Value;        // Expression: 0
                                          //  Referenced by: '<S38>/Clamping_zero'

    real_T Bias6_Bias_h;               // Expression: -9.81
                                          //  Referenced by: '<S4>/Bias6'

    real_T Clamping_zero_Value_j;      // Expression: 0
                                          //  Referenced by: '<S97>/Clamping_zero'

    int8_T Constant_Value_n;           // Computed Parameter: Constant_Value_n
                                          //  Referenced by: '<S38>/Constant'

    int8_T Constant2_Value_l;          // Computed Parameter: Constant2_Value_l
                                          //  Referenced by: '<S38>/Constant2'

    int8_T Constant3_Value;            // Computed Parameter: Constant3_Value
                                          //  Referenced by: '<S38>/Constant3'

    int8_T Constant4_Value;            // Computed Parameter: Constant4_Value
                                          //  Referenced by: '<S38>/Constant4'

    int8_T Constant_Value_nl;          // Computed Parameter: Constant_Value_nl
                                          //  Referenced by: '<S97>/Constant'

    int8_T Constant2_Value_o;          // Computed Parameter: Constant2_Value_o
                                          //  Referenced by: '<S97>/Constant2'

    int8_T Constant3_Value_b;          // Computed Parameter: Constant3_Value_b
                                          //  Referenced by: '<S97>/Constant3'

    int8_T Constant4_Value_m;          // Computed Parameter: Constant4_Value_m
                                          //  Referenced by: '<S97>/Constant4'

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
        uint8_T TID[4];
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

  // Tunable parameters
  static P rtP;

  // Real-Time Model
  RT_MODEL rtM;
};

extern "C"
{
  extern real_T rtInf;
  extern real_T rtMinusInf;
  extern real_T rtNaN;
  extern real32_T rtInfF;
  extern real32_T rtMinusInfF;
  extern real32_T rtNaNF;
//  static boolean_T rtIsInf(real_T value);
//  static boolean_T rtIsInfF(real32_T value);
//  static boolean_T rtIsNaN(real_T value);
//  static boolean_T rtIsNaNF(real32_T value);
}                                      // extern "C"

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
//  Block '<S5>/Product' : Unused code path elimination
//  Block '<S2>/Cast To Boolean' : Unused code path elimination
//  Block '<S2>/Cast To Boolean1' : Unused code path elimination
//  Block '<S2>/Cast To Boolean2' : Unused code path elimination
//  Block '<S2>/Cast To Boolean3' : Unused code path elimination
//  Block '<S5>/Rate Transition2' : Eliminated since input and output rates are identical


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
//  '<S129>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller'
//  '<S130>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Anti-windup'
//  '<S131>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/D Gain'
//  '<S132>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/External Derivative'
//  '<S133>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Filter'
//  '<S134>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Filter ICs'
//  '<S135>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/I Gain'
//  '<S136>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Ideal P Gain'
//  '<S137>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Ideal P Gain Fdbk'
//  '<S138>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Integrator'
//  '<S139>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Integrator ICs'
//  '<S140>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/N Copy'
//  '<S141>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/N Gain'
//  '<S142>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/P Copy'
//  '<S143>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Parallel P Gain'
//  '<S144>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Reset Signal'
//  '<S145>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Saturation'
//  '<S146>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Saturation Fdbk'
//  '<S147>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Sum'
//  '<S148>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Sum Fdbk'
//  '<S149>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Tracking Mode'
//  '<S150>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Tracking Mode Sum'
//  '<S151>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Tsamp - Integral'
//  '<S152>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Tsamp - Ngain'
//  '<S153>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/postSat Signal'
//  '<S154>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/preInt Signal'
//  '<S155>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/preSat Signal'
//  '<S156>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Anti-windup/Passthrough'
//  '<S157>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/D Gain/Disabled'
//  '<S158>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/External Derivative/Disabled'
//  '<S159>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Filter/Disabled'
//  '<S160>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Filter ICs/Disabled'
//  '<S161>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/I Gain/Internal Parameters'
//  '<S162>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Ideal P Gain/Passthrough'
//  '<S163>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Ideal P Gain Fdbk/Disabled'
//  '<S164>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Integrator/Discrete'
//  '<S165>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Integrator ICs/Internal IC'
//  '<S166>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/N Copy/Disabled wSignal Specification'
//  '<S167>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/N Gain/Disabled'
//  '<S168>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/P Copy/Disabled'
//  '<S169>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Parallel P Gain/Internal Parameters'
//  '<S170>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Reset Signal/Disabled'
//  '<S171>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Saturation/Passthrough'
//  '<S172>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Saturation Fdbk/Disabled'
//  '<S173>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Sum/Sum_PI'
//  '<S174>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Sum Fdbk/Disabled'
//  '<S175>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Tracking Mode/Disabled'
//  '<S176>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Tracking Mode Sum/Passthrough'
//  '<S177>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S178>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/Tsamp - Ngain/Passthrough'
//  '<S179>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/postSat Signal/Forward_Path'
//  '<S180>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/preInt Signal/Internal PreInt'
//  '<S181>' : 'platformController/PlatformController/PlatformController/Attitude Controller/Rate_Loop/PID Controller/preSat Signal/Forward_Path'

#endif                                 // platformController_h_

//
// File trailer for generated code.
//
// [EOF]
//
