#include "rpmmeter.h"
#include <sys/time.h>
#include <QDebug>
#include <QTimer>


static callbackData* pUserData;


//typedef void (*CBFuncEx_t)
//   (int pi, unsigned user_gpio, unsigned level, uint32_t tick, void *userdata);
CBFuncEx_t
statusChanged(int      handle,
              unsigned user_gpio,
              unsigned level,
              uint32_t currentTick,
              void*    pData)
{
    // currentTick is the number of microseconds since boot.
    // WARNING: this wraps around from 4294967295 to 0 roughly every 72 minutes
    Q_UNUSED(handle)
    Q_UNUSED(level)
    Q_UNUSED(pData)

    pUserData->transitionCounter[user_gpio]++;
    pUserData->lastTick[user_gpio] = currentTick;
    return nullptr;
}


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
    pUserData = new callbackData;
    pUserData->transitionCounter[inputPin] = 0;
    pUserData->tick0[inputPin] = 0;
    pUserData->lastTick[inputPin] = 0;

    iStatus = callback_ex(gpioHostHandle,
                          inputPin,
                          EITHER_EDGE,
                          reinterpret_cast<CBFuncEx_t>(statusChanged),
                          reinterpret_cast<void *>(pUserData));

    if(iStatus==pigif_duplicate_callback) {
        perror("Duplicate Callback");
        exit(EXIT_FAILURE);
    }
    if(iStatus==pigif_bad_malloc) {
        perror("Bad malloc");
        exit(EXIT_FAILURE);
    }
    if(iStatus==pigif_bad_callback) {
        perror("Bad Callback");
        exit(EXIT_FAILURE);
    }
    encoderTicks = 38;
}


double
RPMmeter::currentSpeed() { // In giri/s
    double dt = (pUserData->lastTick[inputPin]-pUserData->tick0[inputPin])*1.0e-6;
    if(dt == 0.0) return 0.0;
    double speed = pUserData->transitionCounter[inputPin] /
                   (double(encoderTicks)*dt);
    pUserData->transitionCounter[inputPin] = 0;
    pUserData->tick0[inputPin] = pUserData->lastTick[inputPin];
    return speed;
}
