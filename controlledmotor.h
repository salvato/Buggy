#pragma once

#include <QObject>

QT_FORWARD_DECLARE_CLASS(DcMotor)
QT_FORWARD_DECLARE_CLASS(RPMmeter)
QT_FORWARD_DECLARE_CLASS(PID)


class ControlledMotor : public QObject
{
    Q_OBJECT

public:
    explicit ControlledMotor(DcMotor* motor, RPMmeter* speedMeter, QObject* parent);
    void stop();
    void goForward(double speed=1.0);
    void goBackward(double speed=1.0);

signals:

public slots:

private:
    DcMotor*  pMotor;
    RPMmeter* pSpeedMeter;
    PID*      pPid;

    double wantedSpeed;
};
