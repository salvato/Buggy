#ifndef RPMMETER_H
#define RPMMETER_H

#include <QObject>
#include <pigpiod_if2.h>


typedef struct {
    int transitionCounter;
    int iHumidity;
    int iTemperature;
    int callBackId;
    void* pMainWindow = Q_NULLPTR;
} callbackData;

#ifdef __cplusplus
extern "C" {
#endif

CBFuncEx_t statusChanged(int handle,
                         unsigned user_gpio,
                         unsigned level,
                         uint32_t currentTick,
                         void *userdata);
#ifdef __cplusplus
}
#endif


class RPMmeter : public QObject
{
    Q_OBJECT

public:
    explicit RPMmeter(uint gpioPin, int gpioHandle, QObject *parent = nullptr);

signals:

public slots:


private:
    uint inputPin;
    int gpioHostHandle;
    callbackData userData;
};

#endif // RPMMETER_H
