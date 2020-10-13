#include "controlledmotor.h"
#include "PID_v1.h"

#include <QThread>


ControlledMotor::ControlledMotor(DcMotor* motor, RPMmeter* speedMeter, QObject* parent)
    : QObject(parent)
    , pMotor(motor)
    , pSpeedMeter(speedMeter)
{
    wantedSpeed = 0.0;
    pPid = new PID(0.02, 0.0, 0.0, DIRECT);
    pPid->SetSampleTime(200);
    bTerminate = false;
    connect(&updateTimer, SIGNAL(timeout()),
            this, SLOT(updateSpeed()));
}


void
ControlledMotor::go() {
    while(!bTerminate) {

    }
}


void
ControlledMotor::setSpeed(double speed) {
    wantedSpeed = speed;
}

