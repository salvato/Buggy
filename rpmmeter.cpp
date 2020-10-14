#include "rpmmeter.h"
#include <sys/time.h>
#include <QDebug>
#include <QTimer>


static callbackData userData;
#define SAMPLETIME 200000 // in us

//typedef void (*CBFuncEx_t)
//   (int pi, unsigned user_gpio, unsigned level, uint32_t tick, void *userdata);

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
    Q_UNUSED(userdata)

    struct timespec tv;
    clock_gettime(CLOCK_BOOTTIME, &tv);
    uint64_t ticks = uint32_t(tv.tv_sec*CLOCKS_PER_SEC)+uint32_t(tv.tv_nsec/1000);
    uint64_t tick0 = uint32_t(currentTick);
    qDebug() << ticks << tick0 << tv.tv_sec << tv.tv_nsec;

    userData = *(reinterpret_cast<callbackData*>(userdata));
    userData.transitionCounter[user_gpio]++;
    int64_t dt = int64_t(currentTick)-int64_t(userData.tick0[user_gpio]);
    if(dt > SAMPLETIME) {
        userData.speed[user_gpio] = userData.transitionCounter[user_gpio];
        userData.transitionCounter[user_gpio] = 0;
        userData.tick0[user_gpio] = currentTick;
    }
    if(dt < 0) {
        userData.tick0[user_gpio] = currentTick;
        userData.transitionCounter[user_gpio] = 0;
    }
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
    ::userData.pMainWindow = parent;
    ::userData.speed[inputPin] = 0;
    ::userData.transitionCounter[inputPin] = 0;
    ::userData.tick0[inputPin] = 0;

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
    pResetTimer = new QTimer();
    connect(pResetTimer, SIGNAL(timeout()),
            this, SLOT(onTimeToReset()));
    pResetTimer->start(SAMPLETIME/500);
}


void
RPMmeter::onTimeToReset() {
    if(::userData.speed[inputPin] == 0)
        return;
    clock_gettime(CLOCK_BOOTTIME, &tv);
    uint64_t ticks = uint64_t(tv.tv_sec*CLOCKS_PER_SEC+tv.tv_nsec/1000);
    uint64_t tick0 = uint64_t(::userData.tick0[inputPin]);
    //qDebug() << ticks << tick0;
    if(ticks - tick0 > 2*SAMPLETIME) {
        ::userData.speed[inputPin] = 0;
    }
}


uint32_t
RPMmeter::currentSpeed() {
    return ::userData.speed[inputPin];
}
