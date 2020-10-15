#include "controlledmotor.h"
#include "dcmotor.h"
#include "rpmmeter.h"
#include "PID_v1.h"

#include <QThread>
#include <QDebug>


ControlledMotor::ControlledMotor(DcMotor* motor, RPMmeter* speedMeter, QObject* parent)
    : QObject(parent)
    , pMotor(motor)
    , pSpeedMeter(speedMeter)
{
    sampleTime_ms = 100;
    wantedSpeed = 0.0;
    speedMax = 12.0;// In giri/s
    currentP = 0.0;
    currentI = 0.0;
    currentD = 0.0;
    pPid = new PID(currentP, currentI, currentD, DIRECT);
    pPid->SetSampleTime(sampleTime_ms);
    pPid->SetMode(AUTOMATIC);
    pPid->SetOutputLimits(-1.0, 1.0);
    bTerminate = false;
}


void
ControlledMotor::go() {
    pUpdateTimer = new QTimer();
    connect(pUpdateTimer, SIGNAL(timeout()),
            this, SLOT(updateSpeed()));
    pUpdateTimer->start(sampleTime_ms);
}


void
ControlledMotor::updateSpeed() {
    if(!bTerminate) {
        currentSpeed = pSpeedMeter->currentSpeed()/speedMax;
        double speed = pPid->Compute(currentSpeed, wantedSpeed);
        if(speed < 0.0)
            pMotor->goBackward(wantedSpeed+speed);
        else {
            pMotor->goForward(wantedSpeed-speed);
        }
        emit LMotorValues(wantedSpeed, currentSpeed, speed);
    }
    else {
        pMotor->stop();
        qDebug() << "Motor Stopped";
        pUpdateTimer->stop();
        thread()->quit();
    }
}


void
ControlledMotor::setP(double p) {
    currentP = p;
    pPid->SetTunings(currentP, currentI, currentD);
}


void
ControlledMotor::setI(double i) {
    currentI = i;
    pPid->SetTunings(currentP, currentI, currentD);
}


void
ControlledMotor::setD(double d) {
    currentD = d;
    pPid->SetTunings(currentP, currentI, currentD);
}


void
ControlledMotor::setSpeed(double speed) {
    wantedSpeed = speed;
}


void
ControlledMotor::terminate() {
    bTerminate = true;
}

