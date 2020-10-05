#include "rpmmeter.h"
#include <pigpio.h>


RPMmeter::RPMmeter(int gpioPin, int gpioHandle, QObject *parent)
    : QObject(parent)
    , inputPin(gpioPin)
    , gpioHostHandle(gpioHandle)
{
    if(gpioHostHandle < 0) {
        perror("GPIO not Initialized");
        exit(EXIT_FAILURE);
    }
}

