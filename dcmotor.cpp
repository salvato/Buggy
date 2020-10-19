#include "dcmotor.h"
#include "pigpiod_if2.h"

// Caratteristiche Motore:

// Tensione applicabile: 3 - 12 V DC
// Velocità di rotazione 200RPM @ 4.5V - 170mA
// Coppia max 800gf @ 3V
// Riduzione 1:48
// Diametro Ruota 69mm

// Resistenza Avvolgimento: 0.94 Ohm   @ 100Hz
// Induttanza Avvolgimento: 1.8 mHenry @ 100Hz

// http://ctms.engin.umich.edu/CTMS/index.php?example=MotorSpeed&section=SystemModeling

// (J)     moment of inertia of the rotor     0.01 kg.m^2
// (b)     motor viscous friction constant    0.1 N.m.s
// (Ke)    electromotive force constant       0.01 V/rad/sec
// (Kt)    motor torque constant              0.01 N.m/Amp
// (R)     electric resistance                1 Ohm
// (L)     electric inductance                0.5 H

DcMotor::DcMotor(uint forwardPin, uint backwardPin, int gpioHandle, QObject *parent)
    : QObject(parent)
    , forward(forwardPin)
    , backward(backwardPin)
    , gpioHostHandle(gpioHandle)
{
    pwmFrequency = 250;
    if(!init())
        exit(EXIT_FAILURE);
}


bool
DcMotor::init() {
    if(gpioHostHandle < 0) {
        perror("pigpiod Not Initialized");
        return false;
    }
    int iResult;
    iResult = set_PWM_frequency(gpioHostHandle, forward, pwmFrequency);
    if((iResult == PI_BAD_USER_GPIO) ||
       (iResult == PI_NOT_PERMITTED))
    {
        perror("Unable to set PWM Frequency");
        return false;
    }
    iResult = set_PWM_frequency(gpioHostHandle, backward, pwmFrequency);
    if((iResult == PI_BAD_USER_GPIO) ||
       (iResult == PI_NOT_PERMITTED))
    {
        perror("Unable to set PWM Frequency");
        return false;
    }

    iResult = get_PWM_range(gpioHostHandle, forward);
    if(iResult == PI_BAD_USER_GPIO) {
        perror("Unable to get PWM Range");
        return false;
    }
    forwardPWMrange = iResult;
    iResult = get_PWM_range(gpioHostHandle, backward);
    if(iResult == PI_BAD_USER_GPIO) {
        perror("Unable to get PWM Range");
        return false;
    }
    backwardPWMrange = iResult;

    if(!stop()) {
        return false;
    }

    return true;
}


bool
DcMotor::stop() {
    int iResult = set_PWM_dutycycle(gpioHostHandle, forward, 0);
    if((iResult == PI_BAD_USER_GPIO) ||
       (iResult == PI_BAD_DUTYCYCLE) ||
       (iResult == PI_NOT_PERMITTED))
    {
        perror("Unable to set PWM Duty Cycle");
        return false;
    }
    iResult = set_PWM_dutycycle(gpioHostHandle, backward, 0);
    if((iResult == PI_BAD_USER_GPIO) ||
       (iResult == PI_BAD_DUTYCYCLE) ||
       (iResult == PI_NOT_PERMITTED))
    {
        perror("Unable to set PWM Duty Cycle");
        return false;
    }
    return true;
}


bool
DcMotor::goForward(double speed) {
    uint forwardSpeed = uint(speed*forwardPWMrange+0.5);
    int iResult;
    iResult = set_PWM_dutycycle(gpioHostHandle, forward, forwardSpeed);
    if((iResult == PI_BAD_USER_GPIO) ||
       (iResult == PI_NOT_PERMITTED))
    {
        perror("Unable to set PWM Frequency");
        return false;
    }
    return true;
}


bool
DcMotor::goBackward(double speed) {
    uint backwardSpeed = uint(speed*backwardPWMrange+0.5);
    int iResult = set_PWM_dutycycle(gpioHostHandle, backward, backwardSpeed);
    if((iResult == PI_BAD_USER_GPIO) ||
       (iResult == PI_NOT_PERMITTED))
    {
        perror("Unable to set PWM Frequency");
        return false;
    }
    return true;
}


