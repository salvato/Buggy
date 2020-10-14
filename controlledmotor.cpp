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
    wantedSpeed = 0.5;
    currentP = 0.01;
    currentI = 0.0;
    currentD = 0.0;
    pPid = new PID(currentP, currentI, currentD, DIRECT);
    pPid->SetSampleTime(200);
    pPid->SetMode(AUTOMATIC);
    bTerminate = false;
}

time_t
ControlledMotor::micros() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec*static_cast<long>(1000000)+tv.tv_usec;
}

void
ControlledMotor::go() {
    pUpdateTimer = new QTimer();
    connect(pUpdateTimer, SIGNAL(timeout()),
            this, SLOT(updateSpeed()));
    pUpdateTimer->start(200);
}


void
ControlledMotor::updateSpeed() {
    if(!bTerminate) {
        currentSpeed = pSpeedMeter->currentSpeed()/100.0;
        double speed = pPid->Compute(currentSpeed, wantedSpeed);
        emit LMotorValues(wantedSpeed, currentSpeed, speed);
        if(speed < 0)
            pMotor->goBackward(-speed);
        else {
            pMotor->goForward(speed);
        }
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

