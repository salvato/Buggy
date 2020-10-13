#pragma once

#include <QObject>
#include <QTimer>

QT_FORWARD_DECLARE_CLASS(DcMotor)
QT_FORWARD_DECLARE_CLASS(RPMmeter)
QT_FORWARD_DECLARE_CLASS(PID)


class ControlledMotor : public QObject
{
    Q_OBJECT

public:
    explicit ControlledMotor(DcMotor* motor, RPMmeter* speedMeter, QObject* parent);
    void go();
    void setSpeed(double speed);

signals:

public slots:

private:
    DcMotor*  pMotor;
    RPMmeter* pSpeedMeter;
    PID*      pPid;

    volatile double wantedSpeed;
    volatile bool bTerminate;
    QTimer updateTimer;
};
