#pragma once

#include <QObject>
#include <pigpiod_if2.h>


typedef struct {
    uint32_t transitionCounter[32] = {0};
    uint32_t tick0[32]             = {0};
    uint32_t lastTick[32]          = {0};
} callbackData;


#ifdef __cplusplus
extern "C" {
#endif

CBFuncEx_t statusChanged(int      handle,
                         unsigned user_gpio,
                         unsigned level,
                         uint32_t currentTick,
                         void*    pData);
#ifdef __cplusplus
}
#endif


class RPMmeter : public QObject
{
    Q_OBJECT

public:
    explicit RPMmeter(uint gpioPin, int gpioHandle, QObject *parent = nullptr);
    double currentSpeed();

signals:

public slots:

private:
    uint inputPin;
    int gpioHostHandle;
};
