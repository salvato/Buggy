#include "motorController.h"
#include "dcmotor.h"
#include "rpmmeter.h"
#include "PID_v1.h"

#include <QThread>
#include <QDebug>


MotorController::MotorController(DcMotor* motor, RPMmeter* speedMeter, QObject* parent)
    : QObject(parent)
    , pMotor(motor)
    , pSpeedMeter(speedMeter)
{
    msSampleTime = 100;
    targetSpeed = 0.0;
    speedMax = 8.0;// In giri/s
    currentP = 0.0;
    currentI = 0.0;
    currentD = 0.0;
    pPid = new PID(currentP, currentI, currentD, DIRECT);
    pPid->SetSampleTime(msSampleTime);
    pPid->SetMode(AUTOMATIC);
    pPid->SetOutputLimits(-1.0, 1.0);
    bTerminate = false;
}


void
MotorController::go() {
    pUpdateTimer = new QTimer();
    pUpdateTimer->setTimerType(Qt::PreciseTimer);
    connect(pUpdateTimer, SIGNAL(timeout()),
            this, SLOT(updateSpeed()));
    pUpdateTimer->start(msSampleTime);
}


void
MotorController::updateSpeed() {
    if(!bTerminate) {
        currentSpeed = pSpeedMeter->currentSpeed()/speedMax;
        double speed = pPid->Compute(currentSpeed, targetSpeed);
        if(speed < 0.0)
            pMotor->goForward(0.0);
            //pMotor->goBackward(-speed);
        else {
            pMotor->goForward(speed);
        }
        emit MotorValues(targetSpeed, currentSpeed, speed);
    }
    else {
        pMotor->stop();
        qDebug() << "Motor Stopped";
        pUpdateTimer->stop();
        thread()->quit();
    }
}


void
MotorController::setP(double p) {
    currentP = p;
    pPid->SetTunings(currentP, currentI, currentD);
}


void
MotorController::setI(double i) {
    currentI = i;
    pPid->SetTunings(currentP, currentI, currentD);
}


void
MotorController::setD(double d) {
    currentD = d;
    pPid->SetTunings(currentP, currentI, currentD);
}


void
MotorController::setSpeed(double speed) {
    targetSpeed = speed;
}


void
MotorController::terminate() {
    bTerminate = true;
}

