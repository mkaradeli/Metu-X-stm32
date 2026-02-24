//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: currentController.h
//
// Code generated for Simulink model 'currentController'.
//
// Model version                  : 1.84
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Tue Feb 24 22:37:53 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#ifndef currentController_h_
#define currentController_h_
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

// Imported (extern) block parameters
extern controller_modes controller_mode;// Variable: controller_mode
                                           //  Referenced by: '<Root>/Constant'


// Class declaration for model currentController
namespace controller
{
  class current final
  {
    // public data and function members
   public:
    // Block signals and states (default storage) for system '<Root>'
    struct DW {
      real32_T DelayInput_DSTATE;      // '<S3>/Delay Input'
      real32_T DelayOutput_DSTATE;     // '<S3>/Delay Output'
      real32_T DelayOutput_DSTATE_l;   // '<S4>/Delay Output'
      real32_T DelayInput_DSTATE_p;    // '<S4>/Delay Input'
      boolean_T Subsystem3_MODE;       // '<Root>/Subsystem3'
    };

    // External inputs (root inport signals with default storage)
    struct ExtU {
      real32_T current_ref;            // '<Root>/current_ref'
      real32_T current_feedback;       // '<Root>/current_feedback'
    };

    // External outputs (root outports fed by signals with default storage)
    struct ExtY {
      real32_T Duty;                   // '<Root>/Duty'
    };

    // Parameters (default storage)
    struct P {
      controller_modes CompareToConstant_const;
                                      // Mask Parameter: CompareToConstant_const
                                         //  Referenced by: '<S1>/Constant'

      real32_T ChatGPT_ICPrevInput;    // Mask Parameter: ChatGPT_ICPrevInput
                                          //  Referenced by: '<S3>/Delay Input'

      real32_T GORKEM_ICPrevInput;     // Mask Parameter: GORKEM_ICPrevInput
                                          //  Referenced by: '<S4>/Delay Input'

      real32_T ChatGPT_ICPrevOutput;   // Mask Parameter: ChatGPT_ICPrevOutput
                                          //  Referenced by: '<S3>/Delay Output'

      real32_T GORKEM_ICPrevOutput;    // Mask Parameter: GORKEM_ICPrevOutput
                                          //  Referenced by: '<S4>/Delay Output'

      real32_T ChatGPT_PoleZ;          // Mask Parameter: ChatGPT_PoleZ
                                          //  Referenced by: '<S3>/GainPole'

      real32_T GORKEM_PoleZ;           // Mask Parameter: GORKEM_PoleZ
                                          //  Referenced by: '<S4>/GainPole'

      real32_T ChatGPT_ZeroZ;          // Mask Parameter: ChatGPT_ZeroZ
                                          //  Referenced by: '<S3>/GainZero'

      real32_T GORKEM_ZeroZ;           // Mask Parameter: GORKEM_ZeroZ
                                          //  Referenced by: '<S4>/GainZero'

      real32_T Duty_Y0;                // Computed Parameter: Duty_Y0
                                          //  Referenced by: '<S2>/Duty'

      real32_T Gain5_Gain;             // Computed Parameter: Gain5_Gain
                                          //  Referenced by: '<S2>/Gain5'

      real32_T Gain2_Gain;             // Computed Parameter: Gain2_Gain
                                          //  Referenced by: '<S2>/Gain2'

      real32_T Saturation_UpperSat;   // Computed Parameter: Saturation_UpperSat
                                         //  Referenced by: '<S3>/Saturation'

      real32_T Saturation_LowerSat;   // Computed Parameter: Saturation_LowerSat
                                         //  Referenced by: '<S3>/Saturation'

      real32_T Gain1_Gain;             // Computed Parameter: Gain1_Gain
                                          //  Referenced by: '<S2>/Gain1'

      real32_T Gain4_Gain;             // Computed Parameter: Gain4_Gain
                                          //  Referenced by: '<S2>/Gain4'

      real32_T Saturation_UpperSat_l;
                                    // Computed Parameter: Saturation_UpperSat_l
                                       //  Referenced by: '<S4>/Saturation'

      real32_T Saturation_LowerSat_j;
                                    // Computed Parameter: Saturation_LowerSat_j
                                       //  Referenced by: '<S4>/Saturation'

      uint8_T ManualSwitch_CurrentSetting;
                              // Computed Parameter: ManualSwitch_CurrentSetting
                                 //  Referenced by: '<S2>/Manual Switch'

    };

    // Copy Constructor
    current(current const&) = delete;

    // Assignment Operator
    current& operator= (current const&) & = delete;

    // Move Constructor
    current(current &&) = delete;

    // Move Assignment Operator
    current& operator= (current &&) = delete;

    // External inputs
    ExtU rtU;

    // External outputs
    ExtY rtY;

    // model initialize function
    void initialize();

    // model step function
    void step();

    // Constructor
    current();

    // Destructor
    ~current();

    // private data and function members
   private:
    // Block states
    DW rtDW;

    // Tunable parameters
    static P rtP;
  };
}

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<Root>/Scope' : Unused code path elimination
//  Block '<S3>/DataTypeProp' : Unused code path elimination
//  Block '<S4>/DataTypeProp' : Unused code path elimination
//  Block '<S2>/Scope' : Unused code path elimination
//  Block '<S3>/Downcast' : Eliminate redundant data type conversion
//  Block '<S4>/Downcast' : Eliminate redundant data type conversion


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
//  '<Root>' : 'currentController'
//  '<S1>'   : 'currentController/Compare To Constant'
//  '<S2>'   : 'currentController/Subsystem3'
//  '<S3>'   : 'currentController/Subsystem3/ChatGPT'
//  '<S4>'   : 'currentController/Subsystem3/GORKEM'

#endif                                 // currentController_h_

//
// File trailer for generated code.
//
// [EOF]
//
