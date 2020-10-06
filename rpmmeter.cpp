#include "rpmmeter.h"
#include <QDebug>


RPMmeter::RPMmeter(uint gpioPin, int gpioHandle, QObject *parent)
    : QObject(parent)
    , inputPin(gpioPin)
    , gpioHostHandle(gpioHandle)
{
    if(gpioHostHandle < 0) {
        perror("GPIO not Initialized");
        exit(EXIT_FAILURE);
    }
    int iStatus = set_mode(gpioHostHandle, inputPin, PI_INPUT);
    if(iStatus) {
        perror("Unable to set rpm meter as input");
        exit(EXIT_FAILURE);
    }
    iStatus = set_pull_up_down(gpioHostHandle, inputPin, PI_PUD_DOWN);
    if(iStatus) {
        perror("Unable to set rpm meter as input");
        exit(EXIT_FAILURE);
    }
    iStatus = callback_ex(gpioHostHandle,
                          inputPin,
                          EITHER_EDGE,
                          reinterpret_cast<CBFuncEx_t>(statusChanged),
                          reinterpret_cast<void *>(&userData));
}


CBFuncEx_t
statusChanged(int handle,
              unsigned user_gpio,
              unsigned level,
              uint32_t currentTick,
              void *userdata)
{
    Q_UNUSED(handle)

    callbackData userData;
    userData = *(reinterpret_cast<callbackData*>(userdata));
    qDebug() << user_gpio << level << currentTick;
    return nullptr;
}
