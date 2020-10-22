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
    speedMax = 8.0;// In giri/s
    targetSpeed = 0.0;

    currentP = 0.0;
    currentI = 0.0;
    currentD = 0.0;
    pPid = new PID(currentP, currentI, currentD, DIRECT);

    msSamplingTime = 100;
    pPid->SetSampleTime(msSamplingTime);
    pPid->SetMode(AUTOMATIC);
    pPid->SetOutputLimits(-1.0, 1.0);

    bTerminate = false;
}


void
MotorController::setPIDmode(int Mode) {
    pPid->SetMode(Mode);
}


void
MotorController::go() {
    // The Timer MUST be created after the
    // Controller Thread has been started.
    pUpdateTimer = new QTimer();
    pUpdateTimer->setTimerType(Qt::PreciseTimer);
    connect(pUpdateTimer, SIGNAL(timeout()),
            this, SLOT(updateSpeed()));
    pUpdateTimer->start(msSamplingTime);
}


void
MotorController::updateSpeed() {
    if(!bTerminate) {
        currentSpeed = pSpeedMeter->currentSpeed()/speedMax;
        double commandedSpeed = pPid->Compute(currentSpeed, targetSpeed);
        if(commandedSpeed < 0.0)
            //pMotor->goBackward(-commandedSpeed);// Fino a quando il sensore di
            pMotor->goForward(0.0);// rotazione non fornisce la direzione
        else
            pMotor->goForward(commandedSpeed);
        emit MotorValues(targetSpeed, currentSpeed, commandedSpeed);
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

