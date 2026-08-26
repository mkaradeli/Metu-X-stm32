//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: actuatorController.h
//
// Code generated for Simulink model 'actuatorController'.
//
// Model version                  : 1.73
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Mon Aug 17 23:22:53 2026
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

#ifndef DEFINED_TYPEDEF_FOR_struct_McVkfAUK1xSt1FP9rTvxdE_
#define DEFINED_TYPEDEF_FOR_struct_McVkfAUK1xSt1FP9rTvxdE_

struct struct_McVkfAUK1xSt1FP9rTvxdE
{
  real32_T Kp;
  real32_T Ki;
};

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_IJnI4imAtcq7GOnq1yNUdE_
#define DEFINED_TYPEDEF_FOR_struct_IJnI4imAtcq7GOnq1yNUdE_

struct struct_IJnI4imAtcq7GOnq1yNUdE
{
  struct_lIrpsBx2XGeWflfk3xVByG current;
  struct_efjc54AtCP4u6rYRnJfUyF speed;
  struct_UOhA0kLqPt9jH3DECgPk1E position;
  struct_McVkfAUK1xSt1FP9rTvxdE pressure;
};

#endif

//
//  Exported Global Parameters
//
//  Note: Exported global parameters are tunable parameters with an exported
//  global storage class designation.  Code generation will declare the memory for
//  these parameters and exports their symbols.
//

extern struct_IJnI4imAtcq7GOnq1yNUdE currentControllerGains;// Variable: controllerGains
                                                               //  Referenced by:
                                                               //    '<S12>/Gain'
                                                               //    '<S12>/Gain1'
                                                               //    '<S12>/Saturation'
                                                               //    '<S13>/Discrete-Time Integrator'
                                                               //    '<S13>/Gain'
                                                               //    '<S13>/Gain1'
                                                               //    '<S13>/Gain2'
                                                               //    '<S13>/Rate Limiter'
                                                               //    '<S13>/Saturation'
                                                               //    '<S50>/Integral Gain'
                                                               //    '<S58>/Proportional Gain'

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
      real_T RateLimiter;              // '<S2>/Rate Limiter'
      real_T Integrator_DSTATE;        // '<S53>/Integrator'
      real_T PrevY;                    // '<S2>/Rate Limiter'
      real32_T Product;                // '<S14>/Product'
      real32_T Saturation_m;           // '<S12>/Saturation'
      real32_T DiscreteTimeIntegrator_DSTATE;// '<S72>/Discrete-Time Integrator' 
      real32_T DiscreteTimeIntegrator_DSTATE_b;// '<S13>/Discrete-Time Integrator' 
      real32_T UD_DSTATE;              // '<S71>/UD'
      real32_T PrevY_h;                // '<S13>/Rate Limiter'
      boolean_T SpeedController_MODE;  // '<S1>/Speed Controller'
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
      real32_T ValveFitPressureRatios[11];// '<Root>/ValveFitPressureRatios'
      real32_T F_demand;               // '<Root>/F_demand'
      real32_T nozzle_gain;            // '<Root>/nozzle_gain'
    };

    // External outputs (root outports fed by signals with default storage)
    struct ExtY {
      real32_T currentDemand;          // '<Root>/currentDemand'
      real32_T speedDemand;            // '<Root>/speedDemand'
      real32_T position_demand;        // '<Root>/position_demand'
      real32_T pos_ref_rate_limited;   // '<Root>/pos_ref_rate_limited'
      real32_T P_nozzle_demand1;       // '<Root>/P_nozzle_demand1'
      real32_T ThrustMax;              // '<Root>/ThrustMax'
      real32_T ThrustEstimate;         // '<Root>/ThrustEstimate'
    };

    // Parameters (default storage)
    struct P {
      real_T PIDController_InitialConditionF;
                              // Mask Parameter: PIDController_InitialConditionF
                                 //  Referenced by: '<S53>/Integrator'

      real_T PIDController_Kt;         // Mask Parameter: PIDController_Kt
                                          //  Referenced by: '<S64>/Kt'

      controller_modes CompareToConstant3_const;
                                     // Mask Parameter: CompareToConstant3_const
                                        //  Referenced by: '<S6>/Constant'

      controller_modes CompareToConstant1_const;
                                     // Mask Parameter: CompareToConstant1_const
                                        //  Referenced by: '<S4>/Constant'

      controller_modes CompareToConstant_const;
                                      // Mask Parameter: CompareToConstant_const
                                         //  Referenced by: '<S3>/Constant'

      controller_modes CompareToConstant4_const;
                                     // Mask Parameter: CompareToConstant4_const
                                        //  Referenced by: '<S7>/Constant'

      controller_modes CompareToConstant6_const;
                                     // Mask Parameter: CompareToConstant6_const
                                        //  Referenced by: '<S9>/Constant'

      controller_modes CompareToConstant5_const;
                                     // Mask Parameter: CompareToConstant5_const
                                        //  Referenced by: '<S8>/Constant'

      controller_modes CompareToConstant2_const;
                                     // Mask Parameter: CompareToConstant2_const
                                        //  Referenced by: '<S5>/Constant'

      real32_T DiscreteDerivative_ICPrevScaled;
                              // Mask Parameter: DiscreteDerivative_ICPrevScaled
                                 //  Referenced by: '<S71>/UD'

      real32_T CompareToConstant_const_p;
                                    // Mask Parameter: CompareToConstant_const_p
                                       //  Referenced by: '<S16>/Constant'

      real_T Constant_Value;           // Expression: 0
                                          //  Referenced by: '<S2>/Constant'

      real_T Constant1_Value;          // Expression: 0
                                          //  Referenced by: '<S2>/Constant1'

      real_T Theta_Y0;                 // Computed Parameter: Theta_Y0
                                          //  Referenced by: '<S2>/Theta'

      real_T Integrator_gainval;       // Computed Parameter: Integrator_gainval
                                          //  Referenced by: '<S53>/Integrator'

      real_T Saturation3_UpperSat;     // Expression: 1200
                                          //  Referenced by: '<S2>/Saturation3'

      real_T Saturation3_LowerSat;     // Expression: 0
                                          //  Referenced by: '<S2>/Saturation3'

      real_T RateLimiter_RisingLim;    // Expression: 2000
                                          //  Referenced by: '<S2>/Rate Limiter'

      real_T RateLimiter_FallingLim;   // Expression: -2000
                                          //  Referenced by: '<S2>/Rate Limiter'

      real_T RateLimiter_IC;           // Expression: 0
                                          //  Referenced by: '<S2>/Rate Limiter'

      real32_T Gain_Gain;              // Computed Parameter: Gain_Gain
                                          //  Referenced by: '<S2>/Gain'

      real32_T Saturation_UpperSat;   // Computed Parameter: Saturation_UpperSat
                                         //  Referenced by: '<S2>/Saturation'

      real32_T Saturation_LowerSat;   // Computed Parameter: Saturation_LowerSat
                                         //  Referenced by: '<S2>/Saturation'

      real32_T Saturation1_UpperSat; // Computed Parameter: Saturation1_UpperSat
                                        //  Referenced by: '<S2>/Saturation1'

      real32_T Saturation1_LowerSat; // Computed Parameter: Saturation1_LowerSat
                                        //  Referenced by: '<S2>/Saturation1'

      real32_T uDLookupTable_tableData[11];
                                  // Computed Parameter: uDLookupTable_tableData
                                     //  Referenced by: '<S2>/1-D Lookup Table'

      real32_T SpeedDemand_Y0;         // Computed Parameter: SpeedDemand_Y0
                                          //  Referenced by: '<S12>/SpeedDemand'

      real32_T TSamp_WtEt;             // Computed Parameter: TSamp_WtEt
                                          //  Referenced by: '<S71>/TSamp'

      real32_T Y_Y0;                   // Computed Parameter: Y_Y0
                                          //  Referenced by: '<S13>/Y'

      real32_T DiscreteTimeIntegrator_gainval;
                           // Computed Parameter: DiscreteTimeIntegrator_gainval
                              //  Referenced by: '<S13>/Discrete-Time Integrator'

      real32_T DiscreteTimeIntegrator_IC;
                                // Computed Parameter: DiscreteTimeIntegrator_IC
                                   //  Referenced by: '<S13>/Discrete-Time Integrator'

      real32_T RateLimiter_IC_o;       // Computed Parameter: RateLimiter_IC_o
                                          //  Referenced by: '<S13>/Rate Limiter'

      real32_T P_nozzle_demand_Y0;     // Computed Parameter: P_nozzle_demand_Y0
                                          //  Referenced by: '<S14>/P_nozzle_demand'

      real32_T Saturation1_UpperSat_g;
                                   // Computed Parameter: Saturation1_UpperSat_g
                                      //  Referenced by: '<S14>/Saturation1'

      real32_T Saturation1_LowerSat_h;
                                   // Computed Parameter: Saturation1_LowerSat_h
                                      //  Referenced by: '<S14>/Saturation1'

      real32_T Saturation_UpperSat_p;
                                    // Computed Parameter: Saturation_UpperSat_p
                                       //  Referenced by: '<S14>/Saturation'

      real32_T Saturation_LowerSat_k;
                                    // Computed Parameter: Saturation_LowerSat_k
                                       //  Referenced by: '<S14>/Saturation'

      real32_T DiscreteTimeIntegrator_gainva_l;
                          // Computed Parameter: DiscreteTimeIntegrator_gainva_l
                             //  Referenced by: '<S72>/Discrete-Time Integrator'

      real32_T DiscreteTimeIntegrator_IC_c;
                              // Computed Parameter: DiscreteTimeIntegrator_IC_c
                                 //  Referenced by: '<S72>/Discrete-Time Integrator'

      real32_T Gain1_Gain;             // Computed Parameter: Gain1_Gain
                                          //  Referenced by: '<S10>/Gain1'

      real32_T Gain1_Gain_p;           // Computed Parameter: Gain1_Gain_p
                                          //  Referenced by: '<S11>/Gain1'

      real32_T Saturation1_UpperSat_k;
                                   // Computed Parameter: Saturation1_UpperSat_k
                                      //  Referenced by: '<S15>/Saturation1'

      real32_T Saturation1_LowerSat_k;
                                   // Computed Parameter: Saturation1_LowerSat_k
                                      //  Referenced by: '<S15>/Saturation1'

      real32_T K_Gain;                 // Computed Parameter: K_Gain
                                          //  Referenced by: '<S72>/K'

      real32_T Gain_Gain_h;            // Computed Parameter: Gain_Gain_h
                                          //  Referenced by: '<S72>/Gain'

      real32_T Saturation_UpperSat_k;
                                    // Computed Parameter: Saturation_UpperSat_k
                                       //  Referenced by: '<S72>/Saturation'

      real32_T Saturation_LowerSat_kk;
                                   // Computed Parameter: Saturation_LowerSat_kk
                                      //  Referenced by: '<S72>/Saturation'

      real32_T Gain_Gain_j;            // Computed Parameter: Gain_Gain_j
                                          //  Referenced by: '<S1>/Gain'

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

    // Tunable parameters
    static P rtP;

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
//  Block '<S18>/Data Type Duplicate' : Unused code path elimination
//  Block '<S18>/Data Type Propagation' : Unused code path elimination
//  Block '<S71>/Data Type Duplicate' : Unused code path elimination
//  Block '<S12>/Scope' : Unused code path elimination
//  Block '<S1>/Scope' : Unused code path elimination
//  Block '<S15>/Scope' : Unused code path elimination


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
//  '<S9>'   : 'actuatorController/Subsystem1/Compare To Constant6'
//  '<S10>'  : 'actuatorController/Subsystem1/Degrees to Radians'
//  '<S11>'  : 'actuatorController/Subsystem1/Degrees to Radians1'
//  '<S12>'  : 'actuatorController/Subsystem1/Position Controller'
//  '<S13>'  : 'actuatorController/Subsystem1/Speed Controller'
//  '<S14>'  : 'actuatorController/Subsystem1/Thrust Controller'
//  '<S15>'  : 'actuatorController/Subsystem1/reference conditioning'
//  '<S16>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/Compare To Constant'
//  '<S17>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller'
//  '<S18>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/Saturation Dynamic'
//  '<S19>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Anti-windup'
//  '<S20>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/D Gain'
//  '<S21>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/External Derivative'
//  '<S22>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Filter'
//  '<S23>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Filter ICs'
//  '<S24>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/I Gain'
//  '<S25>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Ideal P Gain'
//  '<S26>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Ideal P Gain Fdbk'
//  '<S27>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Integrator'
//  '<S28>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Integrator ICs'
//  '<S29>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/N Copy'
//  '<S30>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/N Gain'
//  '<S31>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/P Copy'
//  '<S32>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Parallel P Gain'
//  '<S33>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Reset Signal'
//  '<S34>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Saturation'
//  '<S35>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Saturation Fdbk'
//  '<S36>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Sum'
//  '<S37>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Sum Fdbk'
//  '<S38>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Tracking Mode'
//  '<S39>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Tracking Mode Sum'
//  '<S40>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Tsamp - Integral'
//  '<S41>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Tsamp - Ngain'
//  '<S42>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/postSat Signal'
//  '<S43>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/preInt Signal'
//  '<S44>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/preSat Signal'
//  '<S45>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Anti-windup/Passthrough'
//  '<S46>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/D Gain/Disabled'
//  '<S47>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/External Derivative/Disabled'
//  '<S48>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Filter/Disabled'
//  '<S49>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Filter ICs/Disabled'
//  '<S50>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/I Gain/Internal Parameters'
//  '<S51>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Ideal P Gain/Passthrough'
//  '<S52>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Ideal P Gain Fdbk/Disabled'
//  '<S53>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Integrator/Discrete'
//  '<S54>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Integrator ICs/Internal IC'
//  '<S55>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/N Copy/Disabled wSignal Specification'
//  '<S56>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/N Gain/Disabled'
//  '<S57>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/P Copy/Disabled'
//  '<S58>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Parallel P Gain/Internal Parameters'
//  '<S59>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Reset Signal/Disabled'
//  '<S60>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Saturation/Passthrough'
//  '<S61>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Saturation Fdbk/Disabled'
//  '<S62>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Sum/Sum_PI'
//  '<S63>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Sum Fdbk/Disabled'
//  '<S64>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Tracking Mode/Enabled'
//  '<S65>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Tracking Mode Sum/Tracking Mode'
//  '<S66>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Tsamp - Integral/TsSignalSpecification'
//  '<S67>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/Tsamp - Ngain/Passthrough'
//  '<S68>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/postSat Signal/Forward_Path'
//  '<S69>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/preInt Signal/Internal PreInt'
//  '<S70>'  : 'actuatorController/Subsystem1/1D Valve Lookup Controller External Table/PID Controller/preSat Signal/Forward_Path'
//  '<S71>'  : 'actuatorController/Subsystem1/Position Controller/Discrete Derivative'
//  '<S72>'  : 'actuatorController/Subsystem1/reference conditioning/first order hiz limitli filtre'

#endif                                 // actuatorController_h_

//
// File trailer for generated code.
//
// [EOF]
//
