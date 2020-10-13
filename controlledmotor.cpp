#include "controlledmotor.h"
#include "PID_v1.h"


ControlledMotor::ControlledMotor(DcMotor* motor, RPMmeter* speedMeter, QObject* parent)
    : QObject(parent)
    , pMotor(motor)
    , pSpeedMeter(speedMeter)
{
    wantedSpeed = 0.0;
    pPid = new PID(0.02, 0.0, 0.0, DIRECT);
    pPid->SetSampleTime(200);
}


void
ControlledMotor::stop() {

}


void
ControlledMotor::goForward(double speed) {

}


void
ControlledMotor::goBackward(double speed) {

}

