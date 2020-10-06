#include "rpmmeter.h"
#include <QDebug>


static callbackData userData;


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
    ::userData.pMainWindow = parent;
    iStatus = callback_ex(gpioHostHandle,
                          inputPin,
                          EITHER_EDGE,
                          reinterpret_cast<CBFuncEx_t>(statusChanged),
                          reinterpret_cast<void *>(&::userData));
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
}


CBFuncEx_t
statusChanged(int handle,
              unsigned user_gpio,
              unsigned level,
              uint32_t currentTick,
              void *userdata)
{
    // currentTick is the number of microseconds since boot.
    // WARNING: this wraps around from 4294967295 to 0 roughly every 72 minutes
    Q_UNUSED(handle)
    Q_UNUSED(level)

    userData = *(reinterpret_cast<callbackData*>(userdata));
    userData.transitionCounter[user_gpio]++;
    int64_t dt = currentTick-userData.tick0[user_gpio];
    if(dt > 1000000) {
        qDebug() << user_gpio
                 << userData.transitionCounter[user_gpio];
        userData.transitionCounter[user_gpio] = 0;
        userData.tick0[user_gpio] = currentTick;
    }
    if(dt < 0) {
        userData.tick0[user_gpio] = currentTick;
        userData.transitionCounter[user_gpio] = 0;
    }
    return nullptr;
}
