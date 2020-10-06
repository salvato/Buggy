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


class RPMmeter : public QObject
{
    Q_OBJECT

public:
    explicit RPMmeter(uint gpioPin, int gpioHandle, QObject *parent = nullptr);
    static CBFuncEx_t statusChanged(int handle, unsigned user_gpio, unsigned level, uint32_t currentTick, void *userdata);

signals:

public slots:


private:
    uint inputPin;
    int gpioHostHandle;
    callbackData* pUserData;
};

#endif // RPMMETER_H
