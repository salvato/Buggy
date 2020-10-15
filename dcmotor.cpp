#include "dcmotor.h"
#include "pigpiod_if2.h"


DcMotor::DcMotor(uint forwardPin, uint backwardPin, int gpioHandle, QObject *parent)
    : QObject(parent)
    , forward(forwardPin)
    , backward(backwardPin)
    , gpioHostHandle(gpioHandle)
{
    pwmFrequency = 100;
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


