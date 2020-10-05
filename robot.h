#pragma once

#include <QObject>
#include <QTimer>
#include <ADXL345.h>
#include <ITG3200.h>
#include <HMC5883L.h>
#include <MadgwickAHRS.h>
#include <dcmotor.h>
#include <sys/time.h>
#include <inttypes.h>


#define ACC_ADDR ADXL345_ADDR_ALT_LOW
#define ITG3200_DEF_ADDR ITG3200_ADDR_AD0_LOW
// HMC5843 address is fixed so don't bother to define it


class Robot : public QObject
{
    Q_OBJECT
public:
    explicit Robot(uint leftForwardPin, uint leftBackwardPin,
                   uint rightForwardPin, uint rightBackwardPin,
                   int gpioHandle, QObject *parent = nullptr);
    bool forward(double speed);
    bool backward(double speed);
    bool stop();
    bool left(double speed);
    bool right(double speed);
    bool getOrientation(float* q0, float* q1, float* q2, float* q3);

signals:

public slots:
    void onUpdateTimeElapsed();

protected:
    void initAHRSsensor();
    bool isStationary();
    __suseconds_t micros();

private:
    int gpioHostHandle;

    DcMotor*  leftMotor;
    DcMotor*  rightMotor;

    ADXL345*  pAcc;
    ITG3200*  pGyro;
    HMC5883L* pMagn;
    Madgwick* pMadgwick;

    float samplingFrequency;

    QTimer updateTimer;
    __suseconds_t lastUpdate;
    __suseconds_t now;
    float delta;
    float values[9];

    float GyroXOffset;
    float GyroYOffset;
    float GyroZOffset;
};
