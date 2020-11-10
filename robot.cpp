#include <robot.h>
#include <cmath>
#include <QThread>


// Hardware Connections:
//
// DFRobot 10DOF :
//      SDA on BCM 2:    pin 3 in the 40 pins GPIO connector
//      SDL on BCM 3:    pin 5 in the 40 pins GPIO connector
//      Vcc on 5V Power: pin 4 in the 40 pins GPIO connector
//      GND on GND:      pin 6 in the 40 pins GPIO connector

Robot::Robot(QObject *parent)
    : QObject(parent)
{
}


__useconds_t
Robot::micros() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * __suseconds_t(1000000) + tv.tv_usec;
}


bool
Robot::forward(double speed) {
    if(speed > 1.0) speed = 1.0;
    if(speed < 0.0) speed = 0.0;
    return false;
}


bool
Robot::backward(double speed) {
    if(speed > 1.0) speed = 1.0;
    if(speed < 0.0) speed = 0.0;
    return false;
}


bool
Robot::stop() {
    return false;
}


bool
Robot::left(double speed) {
    if(speed > 1.0) speed = 1.0;
    if(speed < 0.0) speed = 0.0;
    return false;
}


bool
Robot::right(double speed) {
    if(speed > 1.0) speed = 1.0;
    if(speed < 0.0) speed = 0.0;
    return false;
}


void
Robot::onUpdateTimeElapsed() {
    now = micros();
    delta = float(now-lastUpdate)/1000000.f;
}


bool
Robot::getOrientation(float* q0, float* q1, float* q2, float* q3) {
    return true;
}


void
Robot::onTimeToSendData() {
    emit sendOrientation(q0, q1, q2, q3);
}
