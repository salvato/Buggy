// Arduino PID Library - Version 1.0.1
// by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
//
// This Library is licensed under a GPLv3 License

#include <PID_v1.h>


// Constructor (...)
// The parameters specified here are those for for which we can't set up
// reliable defaults, so we need to have the user set them.
PID::PID(double Kp, double Ki, double Kd, int ControllerDirection) {
    lastInput = 0.0;
    output    = 0.0;
    inAuto    = false;
    ITerm     = 0.0;
    controllerDirection = DIRECT;
    SetOutputLimits(0, 255);
    SampleTime = 100; //default Controller Sample Time in ms

    PID::SetControllerDirection(ControllerDirection);
    PID::SetTunings(Kp, Ki, Kd);

    lastTime  = static_cast<unsigned long>(micros()/1000)-SampleTime;
}


// Compute()
// This, as they say, is where the magic happens.  this function should be called
// every time "void loop()" executes.  the function will decide for itself whether
// a new pid Output needs to be computed
double
PID::Compute(double input, double setpoint) {
    if(!inAuto) return output;
    unsigned long now = static_cast<unsigned long>(micros()/1000);
    unsigned long timeChange = (now - lastTime);

    if(timeChange >= SampleTime) {
        // Compute all the working error variables
        double error = setpoint - input;
        ITerm += (ki * error);
        if(ITerm > outMax)
            ITerm= outMax;
        else if(ITerm < outMin)
            ITerm= outMin;
        double dInput = (input - lastInput);

        // Compute PID Output
        output = kp*error + ITerm - kd*dInput;

        if(output > outMax)
            output = outMax;
        else if(output < outMin)
            output = outMin;

        // Remember some variables for next time
        lastInput = input;
        lastTime = now;
    }
    return output;
}


// SetTunings(...)*************************************************************
// This function allows the controller's dynamic performance to be adjusted.
// it's called automatically from the constructor, but tunings can also
// be adjusted on the fly during normal operation
void
PID::SetTunings(double Kp, double Ki, double Kd) {
    if (Kp<0 || Ki<0 || Kd<0) return;

    dispKp = Kp;
    dispKi = Ki;
    dispKd = Kd;

    double SampleTimeInSec = (double(SampleTime))/1000.0;

    kp = Kp;
    ki = Ki * SampleTimeInSec;
    kd = Kd / SampleTimeInSec;

    if(controllerDirection == REVERSE) {
        kp = (0 - kp);
        ki = (0 - ki);
        kd = (0 - kd);
    }
}


// SetSampleTime(...)
// sets the period, in Milliseconds, at which the calculation is performed
void
PID::SetSampleTime(int NewSampleTime) {
    if(NewSampleTime > 0) {
        double ratio  = double(NewSampleTime) / double(SampleTime);
        ki *= ratio;
        kd /= ratio;
        SampleTime = static_cast<unsigned long>(NewSampleTime);
    }
}


// SetOutputLimits(...)****************************************************
//    This function will be used far more often than SetInputLimits.  while
//  the input to the controller will generally be in the 0-1023 range (which is
//  the default already,)  the output will be a little different.  maybe they'll
//  be doing a time window and will need 0-8000 or something.  or maybe they'll
//  want to clamp it from 0-125.  who knows.  at any rate, that can all be done
//  here.
void
PID::SetOutputLimits(double Min, double Max) {
    if(Min >= Max) return;
    outMin = Min;
    outMax = Max;

    if(inAuto) {
        if(output > outMax)
            output = outMax;
        else if(output < outMin)
            output = outMin;

        if(ITerm > outMax)
            ITerm = outMax;
        else if(ITerm < outMin)
            ITerm = outMin;
    }
}


// SetMode(...)
// Allows the controller Mode to be set to manual (0) or Automatic (non-zero)
// when the transition from manual to auto occurs, the controller is
// automatically initialized
void
PID::SetMode(int Mode) {
    bool newAuto = (Mode == AUTOMATIC);
    if(newAuto == !inAuto) {  // we just went from manual to auto
        PID::Initialize(lastInput);
    }
    inAuto = newAuto;
}


// Initialize()
// does all the things that need to happen to ensure a bumpless transfer
// from manual to automatic mode.
void
PID::Initialize(double input) {
    ITerm = output;
    lastInput = input;
    if(ITerm > outMax)
        ITerm = outMax;
    else if(ITerm < outMin)
        ITerm = outMin;
}


// SetControllerDirection()
// The PID will either be connected to a DIRECT acting process (+Output leads
// to +Input) or a REVERSE acting process(+Output leads to -Input.)  we need to
// know which one, because otherwise we may increase the output when we should
// be decreasing.  This is called from the constructor.
void
PID::SetControllerDirection(int Direction) {
    if(inAuto && Direction !=controllerDirection) {
        kp = (0 - kp);
        ki = (0 - ki);
        kd = (0 - kd);
    }
    controllerDirection = Direction;
}


// Status Functions*************************************************************
// Just because you set the Kp=-1 doesn't mean it actually happened.  these
// functions query the internal state of the PID.  they're here for display
// purposes.  this are the functions the PID Front-end uses for example
double
PID::GetKp(){
    return  dispKp;
}


double
PID::GetKi() {
    return  dispKi;
}


double
PID::GetKd(){
    return  dispKd;
}


int
PID::GetMode() {
    return inAuto ? AUTOMATIC : MANUAL;
}


int
PID::GetDirection() {
    return controllerDirection;
}


time_t
PID::micros() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec*static_cast<time_t>(1000000)+tv.tv_usec;
}
