#pragma once

#include <QObject>
#include <QTimer>

QT_FORWARD_DECLARE_CLASS(DcMotor)
QT_FORWARD_DECLARE_CLASS(RPMmeter)
QT_FORWARD_DECLARE_CLASS(PID)


class MotorController : public QObject
{
    Q_OBJECT

public:
    explicit MotorController(DcMotor* motor, RPMmeter* speedMeter, QObject* parent=nullptr);

public:

signals:
    void MotorValues(double wantedSpeed, double currentSpeed, double speed);

public slots:
    void updateSpeed();
    void terminate();
    void go();
    void setSpeed(double speed);
    void setP(double p);
    void setI(double i);
    void setD(double d);

private:
    DcMotor*  pMotor;
    RPMmeter* pSpeedMeter;
    QTimer*   pUpdateTimer;
    PID*      pPid;

    volatile double wantedSpeed;
    volatile bool bTerminate;
    double currentSpeed;
    double currentP, currentI, currentD;
    double speedMax;
    int    sampleTime_ms;
};
