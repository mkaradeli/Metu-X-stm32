//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: platformController.h
//
// Code generated for Simulink model 'platformController'.
//
// Model version                  : 1.99
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Sat Sep 12 20:35:34 2026
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

#ifndef DEFINED_TYPEDEF_FOR_struct_mDMskPTuLBbAOKfKE3o5aB_
#define DEFINED_TYPEDEF_FOR_struct_mDMskPTuLBbAOKfKE3o5aB_

struct struct_mDMskPTuLBbAOKfKE3o5aB
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

extern struct_mDMskPTuLBbAOKfKE3o5aB platform_targets;// Variable: platform_targets
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
                                                         //    '<S7>/Constant'
                                                         //    '<S73>/Constant6'
                                                         //    '<S137>/Gain3'
                                                         //    '<S138>/Gain4'
                                                         //    '<S138>/Gain5'
                                                         //    '<S167>/Kb'
                                                         //    '<S172>/Integral Gain'
                                                         //    '<S180>/Proportional Gain'

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
    real_T Integrator_DSTATE_f[3];     // '<S175>/Integrator'
    real_T RateTransition6_Buffer[4];  // '<S2>/Rate Transition6'
    real_T RateTransition1_Buffer0[3]; // '<S5>/Rate Transition1'
    real_T RateTransition6_Buffer_h[4];// '<S5>/Rate Transition6'
    real_T Gain;                       // '<S2>/Gain'
    real_T Height;                     // '<S132>/Height'
    real_T DiscreteTimeIntegrator_DSTATE;// '<S134>/Discrete-Time Integrator'
    real_T Integrator_DSTATE;          // '<S111>/Integrator'
    real_T DiscreteTimeIntegrator_DSTATE_j;// '<S72>/Discrete-Time Integrator'
    real_T Integrator_DSTATE_p;        // '<S50>/Integrator'
    real_T DiscreteTimeIntegrator_DSTATE_f;// '<S6>/Discrete-Time Integrator'
    real_T RateTransition_Buffer;      // '<S2>/Rate Transition'
    real_T RateTransition4_Buffer;     // '<S2>/Rate Transition4'
    real_T RateTransition3_Buffer;     // '<S2>/Rate Transition3'
    real_T RateTransition2_Buffer;     // '<S2>/Rate Transition2'
    real_T RateTransition1_Buffer;     // '<S2>/Rate Transition1'
    int8_T DiscreteTimeIntegrator_PrevRese;// '<S134>/Discrete-Time Integrator'
    int8_T Integrator_PrevResetState;  // '<S111>/Integrator'
    int8_T DiscreteTimeIntegrator_PrevRe_j;// '<S72>/Discrete-Time Integrator'
    int8_T Integrator_PrevResetState_n;// '<S50>/Integrator'
    int8_T Integrator_PrevResetState_m;// '<S175>/Integrator'
    int8_T DiscreteTimeIntegrator_PrevRe_e;// '<S6>/Discrete-Time Integrator'
    boolean_T DelayInput1_DSTATE;      // '<S131>/Delay Input1'
    boolean_T UnitDelay_DSTATE;        // '<S135>/Unit Delay'
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

  // Real-Time Model
  RT_MODEL rtM;
};

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S3>/AND' : Unused code path elimination
//  Block '<S8>/Compare' : Unused code path elimination
//  Block '<S8>/Constant' : Unused code path elimination
//  Block '<S10>/Compare' : Unused code path elimination
//  Block '<S10>/Constant' : Unused code path elimination
//  Block '<S3>/Constant6' : Unused code path elimination
//  Block '<S3>/Constant7' : Unused code path elimination
//  Block '<S3>/Gain4' : Unused code path elimination
//  Block '<S58>/Data Type Duplicate' : Unused code path elimination
//  Block '<S58>/Data Type Propagation' : Unused code path elimination
//  Block '<S3>/Product' : Unused code path elimination
//  Block '<S3>/Relational Operator' : Unused code path elimination
//  Block '<S3>/Saturation' : Unused code path elimination
//  Block '<S12>/Data Type Duplicate' : Unused code path elimination
//  Block '<S12>/Data Type Propagation' : Unused code path elimination
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
//  Block '<S69>/Compare' : Unused code path elimination
//  Block '<S69>/Constant' : Unused code path elimination
//  Block '<S4>/Constant' : Unused code path elimination
//  Block '<S119>/Data Type Duplicate' : Unused code path elimination
//  Block '<S119>/Data Type Propagation' : Unused code path elimination
//  Block '<S71>/Data Type Duplicate' : Unused code path elimination
//  Block '<S71>/Data Type Propagation' : Unused code path elimination
//  Block '<S4>/Scope' : Unused code path elimination
//  Block '<S4>/Scope1' : Unused code path elimination
//  Block '<S4>/Scope3' : Unused code path elimination
//  Block '<S4>/Scope4' : Unused code path elimination
//  Block '<S4>/Scope5' : Unused code path elimination
//  Block '<S4>/Scope8' : Unused code path elimination
//  Block '<S139>/Data Type Duplicate' : Unused code path elimination
//  Block '<S139>/Data Type Propagation' : Unused code path elimination
//  Block '<S137>/Scope' : Unused code path elimination
//  Block '<S5>/Product' : Unused code path elimination
//  Block '<S138>/Scope' : Unused code path elimination
//  Block '<S138>/Scope1' : Unused code path elimination
//  Block '<S138>/Scope2' : Unused code path elimination
//  Block '<S5>/Scope' : Unused code path elimination
//  Block '<S5>/Scope1' : Unused code path elimination
//  Block '<S2>/Cast To Boolean' : Unused code path elimination
//  Block '<S2>/Cast To Boolean1' : Unused code path elimination
//  Block '<S2>/Cast To Boolean2' : Unused code path elimination
//  Block '<S62>/Kt' : Eliminated nontunable gain of 1
//  Block '<S123>/Kt' : Eliminated nontunable gain of 1
//  Block '<S135>/Data Type Conversion' : Eliminate redundant data type conversion
//  Block '<S135>/Data Type Conversion1' : Eliminate redundant data type conversion
//  Block '<S5>/Rate Transition2' : Eliminated since input and output rates are identical
//  Block '<S2>/Cast To Boolean3' : Eliminate redundant data type conversion
//  Block '<S135>/Constant' : Unused code path elimination
//  Block '<S135>/Logical Operator1' : Unused code path elimination
//  Block '<S135>/Logical Operator2' : Unused code path elimination


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
//  '<S6>'   : 'platformController/PlatformController/PlatformController1/Subsystem'
//  '<S7>'   : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/Compare To Constant'
//  '<S8>'   : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/Compare To Constant1'
//  '<S9>'   : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/Compare To Constant2'
//  '<S10>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/Compare To Constant3'
//  '<S11>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller'
//  '<S12>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/Saturation Dynamic'
//  '<S13>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Anti-windup'
//  '<S14>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/D Gain'
//  '<S15>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/External Derivative'
//  '<S16>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Filter'
//  '<S17>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Filter ICs'
//  '<S18>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/I Gain'
//  '<S19>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Ideal P Gain'
//  '<S20>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Ideal P Gain Fdbk'
//  '<S21>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Integrator'
//  '<S22>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Integrator ICs'
//  '<S23>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/N Copy'
//  '<S24>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/N Gain'
//  '<S25>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/P Copy'
//  '<S26>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Parallel P Gain'
//  '<S27>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Reset Signal'
//  '<S28>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Saturation'
//  '<S29>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Saturation Fdbk'
//  '<S30>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Sum'
//  '<S31>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Sum Fdbk'
//  '<S32>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Tracking Mode'
//  '<S33>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Tracking Mode Sum'
//  '<S34>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Tsamp - Integral'
//  '<S35>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Tsamp - Ngain'
//  '<S36>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/postSat Signal'
//  '<S37>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/preInt Signal'
//  '<S38>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/preSat Signal'
//  '<S39>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel'
//  '<S40>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S41>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External'
//  '<S42>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External/Dead Zone Dynamic'
//  '<S43>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/D Gain/External Parameters'
//  '<S44>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/External Derivative/External Ydot'
//  '<S45>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Filter/Passthrough'
//  '<S46>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Filter ICs/Disabled'
//  '<S47>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/I Gain/External Parameters'
//  '<S48>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Ideal P Gain/Passthrough'
//  '<S49>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Ideal P Gain Fdbk/Disabled'
//  '<S50>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Integrator/Discrete'
//  '<S51>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Integrator ICs/Internal IC'
//  '<S52>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/N Copy/Disabled wSignal Specification'
//  '<S53>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/N Gain/Passthrough'
//  '<S54>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/P Copy/Disabled'
//  '<S55>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Parallel P Gain/External Parameters'
//  '<S56>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Reset Signal/External Reset'
//  '<S57>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Saturation/External'
//  '<S58>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Saturation/External/Saturation Dynamic'
//  '<S59>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Saturation Fdbk/Disabled'
//  '<S60>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Sum/Sum_PID'
//  '<S61>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Sum Fdbk/Disabled'
//  '<S62>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Tracking Mode/Enabled'
//  '<S63>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Tracking Mode Sum/Tracking Mode'
//  '<S64>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S65>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/Tsamp - Ngain/Passthrough'
//  '<S66>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/postSat Signal/Forward_Path'
//  '<S67>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/preInt Signal/Internal PreInt'
//  '<S68>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Drop/PID Controller/preSat Signal/Forward_Path'
//  '<S69>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Compare To Constant3'
//  '<S70>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller'
//  '<S71>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Saturation Dynamic'
//  '<S72>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem'
//  '<S73>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem1'
//  '<S74>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Anti-windup'
//  '<S75>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/D Gain'
//  '<S76>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/External Derivative'
//  '<S77>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Filter'
//  '<S78>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Filter ICs'
//  '<S79>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/I Gain'
//  '<S80>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Ideal P Gain'
//  '<S81>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Ideal P Gain Fdbk'
//  '<S82>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Integrator'
//  '<S83>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Integrator ICs'
//  '<S84>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/N Copy'
//  '<S85>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/N Gain'
//  '<S86>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/P Copy'
//  '<S87>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Parallel P Gain'
//  '<S88>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Reset Signal'
//  '<S89>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Saturation'
//  '<S90>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Saturation Fdbk'
//  '<S91>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Sum'
//  '<S92>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Sum Fdbk'
//  '<S93>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Tracking Mode'
//  '<S94>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Tracking Mode Sum'
//  '<S95>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Tsamp - Integral'
//  '<S96>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Tsamp - Ngain'
//  '<S97>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/postSat Signal'
//  '<S98>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/preInt Signal'
//  '<S99>'  : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/preSat Signal'
//  '<S100>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel'
//  '<S101>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone'
//  '<S102>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External'
//  '<S103>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/External/Dead Zone Dynamic'
//  '<S104>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/D Gain/External Parameters'
//  '<S105>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/External Derivative/External Ydot'
//  '<S106>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Filter/Passthrough'
//  '<S107>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Filter ICs/Disabled'
//  '<S108>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/I Gain/External Parameters'
//  '<S109>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Ideal P Gain/Passthrough'
//  '<S110>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Ideal P Gain Fdbk/Disabled'
//  '<S111>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Integrator/Discrete'
//  '<S112>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Integrator ICs/Internal IC'
//  '<S113>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/N Copy/Disabled wSignal Specification'
//  '<S114>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/N Gain/Passthrough'
//  '<S115>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/P Copy/Disabled'
//  '<S116>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Parallel P Gain/External Parameters'
//  '<S117>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Reset Signal/External Reset'
//  '<S118>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Saturation/External'
//  '<S119>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Saturation/External/Saturation Dynamic'
//  '<S120>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Saturation Fdbk/Disabled'
//  '<S121>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Sum/Sum_PID'
//  '<S122>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Sum Fdbk/Disabled'
//  '<S123>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Tracking Mode/Enabled'
//  '<S124>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Tracking Mode Sum/Tracking Mode'
//  '<S125>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S126>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/Tsamp - Ngain/Passthrough'
//  '<S127>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/postSat Signal/Forward_Path'
//  '<S128>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/preInt Signal/Internal PreInt'
//  '<S129>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/PID Controller/preSat Signal/Forward_Path'
//  '<S130>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem/Compare To Constant'
//  '<S131>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem1/Detect Increase'
//  '<S132>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem1/Enabled Subsystem'
//  '<S133>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem1/Set-Reset Flip-Flop'
//  '<S134>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem1/Subsystem'
//  '<S135>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem1/Set-Reset Flip-Flop/Discrete'
//  '<S136>' : 'platformController/PlatformController/PlatformController1/Altitude Controller Hover/Subsystem1/Subsystem/Compare To Constant'
//  '<S137>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Position Loop'
//  '<S138>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop'
//  '<S139>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Position Loop/Saturation Dynamic'
//  '<S140>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller'
//  '<S141>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Anti-windup'
//  '<S142>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/D Gain'
//  '<S143>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/External Derivative'
//  '<S144>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Filter'
//  '<S145>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Filter ICs'
//  '<S146>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/I Gain'
//  '<S147>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Ideal P Gain'
//  '<S148>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Ideal P Gain Fdbk'
//  '<S149>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Integrator'
//  '<S150>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Integrator ICs'
//  '<S151>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/N Copy'
//  '<S152>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/N Gain'
//  '<S153>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/P Copy'
//  '<S154>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Parallel P Gain'
//  '<S155>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Reset Signal'
//  '<S156>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Saturation'
//  '<S157>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Saturation Fdbk'
//  '<S158>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Sum'
//  '<S159>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Sum Fdbk'
//  '<S160>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Tracking Mode'
//  '<S161>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Tracking Mode Sum'
//  '<S162>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Tsamp - Integral'
//  '<S163>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Tsamp - Ngain'
//  '<S164>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/postSat Signal'
//  '<S165>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/preInt Signal'
//  '<S166>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/preSat Signal'
//  '<S167>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Anti-windup/Back Calculation'
//  '<S168>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/D Gain/Disabled'
//  '<S169>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/External Derivative/Disabled'
//  '<S170>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Filter/Disabled'
//  '<S171>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Filter ICs/Disabled'
//  '<S172>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/I Gain/Internal Parameters'
//  '<S173>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Ideal P Gain/Passthrough'
//  '<S174>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Ideal P Gain Fdbk/Disabled'
//  '<S175>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Integrator/Discrete'
//  '<S176>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Integrator ICs/Internal IC'
//  '<S177>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/N Copy/Disabled wSignal Specification'
//  '<S178>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/N Gain/Disabled'
//  '<S179>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/P Copy/Disabled'
//  '<S180>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Parallel P Gain/Internal Parameters'
//  '<S181>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Reset Signal/External Reset'
//  '<S182>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Saturation/Enabled'
//  '<S183>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Saturation Fdbk/Disabled'
//  '<S184>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Sum/Sum_PI'
//  '<S185>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Sum Fdbk/Disabled'
//  '<S186>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Tracking Mode/Disabled'
//  '<S187>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Tracking Mode Sum/Passthrough'
//  '<S188>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S189>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/Tsamp - Ngain/Passthrough'
//  '<S190>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/postSat Signal/Forward_Path'
//  '<S191>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/preInt Signal/Internal PreInt'
//  '<S192>' : 'platformController/PlatformController/PlatformController1/Attitude Controller/Rate_Loop/PID Controller/preSat Signal/Forward_Path'
//  '<S193>' : 'platformController/PlatformController/PlatformController1/Subsystem/Compare To Constant'

#endif                                 // platformController_h_

//
// File trailer for generated code.
//
// [EOF]
//
