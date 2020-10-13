#pragma once

#define LIBRARY_VERSION	1.0.0

#include <sys/time.h>
#include <inttypes.h>


//Constants used in some of the functions below
#define AUTOMATIC	1
#define MANUAL	    0
#define DIRECT      0
#define REVERSE     1

class PID
{
public:

    //commonly used functions **************************************************************************
    PID(double Kp, double Ki, double Kd,
        int ControllerDirection);         // * Setpoint and initial tuning parameters are also set here

    void SetMode(int Mode);               // * sets PID to either Manual (0) or Auto (non-0)

    double Compute(double input,          // * performs the PID calculation.  it should be
                   double setpoint);      //   called every cycle. ON/OFF and
                                          //   calculation frequency can be set using SetMode
                                          //   SetSampleTime respectively

    void SetOutputLimits(double, double); // * clamps the output to a specific range. 0-255 by default, but
                                          //   it's likely the user will want to change this depending on
                                          //   the application

    //available but not commonly used functions ********************************************************
    void SetTunings(double kp, double ki, // * While most users will set the tunings once in the
                    double kd);           //   constructor, this function gives the user the option
                                          //   of changing tunings during runtime for Adaptive control
    void SetControllerDirection(int);	  // * Sets the Direction, or "Action" of the controller. DIRECT
                                          //   means the output will increase when error is positive. REVERSE
                                          //   means the opposite.  it's very unlikely that this will be needed
                                          //   once it is set in the constructor.
    void SetSampleTime(int ms);           // * sets the frequency, in Milliseconds, with which
                                          //   the PID calculation is performed.  default is 100

    //Display functions ****************************************************************
    double GetKp();						  // * These functions query the pid for interal values.
    double GetKi();						  //   they were created mainly for the pid front-end,
    double GetKd();						  //   where it's important to know what is actually
    int GetMode();						  //   inside the PID.
    int GetDirection();					  //

private:
    time_t micros();
    void Initialize(double input);

    double dispKp;				// * we'll hold on to the tuning parameters in user-entered
    double dispKi;				//   format for display purposes
    double dispKd;				//
    
    double kp;                  // * (P)roportional Tuning Parameter
    double ki;                  // * (I)ntegral Tuning Parameter
    double kd;                  // * (D)erivative Tuning Parameter

    int controllerDirection;

    unsigned long lastTime;
    double ITerm, lastInput;

    unsigned long SampleTime;
    double outMin, outMax;
    double output;
    bool inAuto;
};


