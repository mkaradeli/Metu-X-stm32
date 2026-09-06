//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// File: HWIL.h
//
// Code generated for Simulink model 'HWIL'.
//
// Model version                  : 1.32
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Sun Aug 30 01:45:16 2026
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM Cortex-M
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#ifndef HWIL_h_
#define HWIL_h_
#include <cmath>
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#include <cstring>

#ifndef ODE3_INTG
#define ODE3_INTG

// ODE3 Integration Data
struct ODE3_IntgData {
  real_T *y;                           // output
  real_T *f[3];                        // derivatives
};

#endif




// Class declaration for model HWIL
class HWIL final
{
  // public data and function members
 public:
  // Block signals and states (default storage) for system '<Root>'
  struct DW {
    real_T Divide;                     // '<S1>/Divide'
    real_T Integrator;                 // '<S1>/Integrator'
    boolean_T Integrator1_DWORK1;      // '<S1>/Integrator1'
    boolean_T Integrator_DWORK1;       // '<S1>/Integrator'
  };

  // Continuous states (default storage)
  struct X {
    real_T Integrator1_CSTATE;         // '<S1>/Integrator1'
    real_T Integrator_CSTATE;          // '<S1>/Integrator'
  };

  // State derivatives (default storage)
  struct XDot {
    real_T Integrator1_CSTATE;         // '<S1>/Integrator1'
    real_T Integrator_CSTATE;          // '<S1>/Integrator'
  };

  // State disabled
  struct XDis {
    boolean_T Integrator1_CSTATE;      // '<S1>/Integrator1'
    boolean_T Integrator_CSTATE;       // '<S1>/Integrator'
  };

  // External inputs (root inport signals with default storage)
  struct ExtU {
    real32_T F_nozzle_1;               // '<Root>/F_nozzle_1'
    real32_T F_nozzle_2;               // '<Root>/F_nozzle_2'
    real32_T F_nozzle_3;               // '<Root>/F_nozzle_3'
    real32_T F_nozzle_4;               // '<Root>/F_nozzle_4'
    real32_T P_nozzle_1;               // '<Root>/P_nozzle_1'
    real32_T P_nozzle_2;               // '<Root>/P_nozzle_2'
    real32_T P_nozzle_3;               // '<Root>/P_nozzle_3'
    real32_T P_nozzle_4;               // '<Root>/P_nozzle_4'
    real32_T P_nozzle_manifold;        // '<Root>/P_nozzle_manifold'
    real32_T quaternion_true[4];       // '<Root>/quaternion_true'
    real32_T V0;                       // '<Root>/V0'
    real32_T X0;                       // '<Root>/X0'
    boolean_T enable_spring;           // '<Root>/enable_spring'
  };

  // External outputs (root outports fed by signals with default storage)
  struct ExtY {
    real32_T mass;                     // '<Root>/mass'
    real_T acceleration[3];            // '<Root>/acceleration'
    real_T velocity;                   // '<Root>/velocity'
    real_T position;                   // '<Root>/position'
    real32_T quaternion_sim[4];        // '<Root>/quaternion_sim'
    real32_T angular_velocity[3];      // '<Root>/angular_velocity'
    real_T lidar_height;               // '<Root>/lidar_height'
  };

  // Real-time Model Data Structure
  using odeFSubArray = real_T[2];
  struct RT_MODEL {
    const char_T *errorStatus;
    RTWSolverInfo solverInfo;
    X *contStates;
    int_T *periodicContStateIndices;
    real_T *periodicContStateRanges;
    real_T *derivs;
    XDis *contStateDisabled;
    boolean_T zCCacheNeedsReset;
    boolean_T derivCacheNeedsReset;
    boolean_T CTOutputIncnstWithState;
    real_T odeY[2];
    real_T odeF[3][2];
    ODE3_IntgData intgData;

    //
    //  Sizes:
    //  The following substructure contains sizes information
    //  for many of the model attributes such as inputs, outputs,
    //  dwork, sample times, etc.

    struct {
      int_T numContStates;
      int_T numPeriodicContStates;
      int_T numSampTimes;
    } Sizes;

    //
    //  Timing:
    //  The following substructure contains information regarding
    //  the timing information for the model.

    struct {
      uint32_T clockTick0;
      time_T stepSize0;
      uint32_T clockTick1;
      boolean_T firstInitCondFlag;
      time_T tStart;
      SimTimeStep simTimeStep;
      boolean_T stopRequestedFlag;
      time_T *t;
      time_T tArray[2];
    } Timing;

    time_T** getTPtrPtr();
    time_T* getTPtr() const;
    void setTPtr(time_T* aTPtr);
    boolean_T getStopRequested() const;
    void setStopRequested(boolean_T aStopRequested);
    boolean_T isMinorTimeStep() const;
    void setFirstInitCond(boolean_T aFirstInitCond);
    boolean_T isFirstInitCond() const;
    boolean_T isMajorTimeStep() const;
    boolean_T* getStopRequestedPtr();
    const char_T** getErrorStatusPtr();
    time_T getTStart() const;
    const char_T* getErrorStatus() const;
    void setErrorStatus(const char_T* const aErrorStatus);
  };

  // Copy Constructor
  HWIL(HWIL const&) = delete;

  // Assignment Operator
  HWIL& operator= (HWIL const&) & = delete;

  // Move Constructor
  HWIL(HWIL &&) = delete;

  // Move Assignment Operator
  HWIL& operator= (HWIL &&) = delete;

  // Real-Time Model get method
  HWIL::RT_MODEL * getRTM();

  // External inputs
  ExtU rtU;

  // External outputs
  ExtY rtY;

  // model initialize function
  void initialize();

  // model step function
  void step();

  // Constructor
  HWIL();

  // Destructor
  ~HWIL();

  // private data and function members
 private:
  // Block states
  DW rtDW;

  // Block continuous states
  X rtX;

  // Block Continuous state disabled vector
  XDis rtXDis;

  // Global mass matrix

  // Continuous states update member function
  void rt_ertODEUpdateContinuousStates(RTWSolverInfo *si );

  // Derivatives member function
  void HWIL_derivatives();

  // Real-Time Model
  RT_MODEL rtM;
};

//-
//  These blocks were eliminated from the model due to optimizations:
//
//  Block '<S1>/Cast To Double' : Eliminate redundant data type conversion


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
//  '<Root>' : 'HWIL'
//  '<S1>'   : 'HWIL/Subsystem'

#endif                                 // HWIL_h_

//
// File trailer for generated code.
//
// [EOF]
//
