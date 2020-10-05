#include <robot.h>
#include <pigpiod_if2.h>
#include <cmath>
#include <QThread>


// Hardware Connections:
//
// DFRobot 10DOF :
//      SDA on BCM 2:    pin 3 in the 40 pins GPIO connector
//      SDL on BCM 3:    pin 5 in the 40 pins GPIO connector
//      Vcc on 5V Power: pin 4 in the 40 pins GPIO connector
//      GND on GND:      pin 6 in the 40 pins GPIO connector

Robot::Robot(uint leftForwardPin, uint leftBackwardPin,
             uint rightForwardPin, uint rightBackwardPin,
             int gpioHandle, QObject *parent)
    : QObject(parent)
    , gpioHostHandle(gpioHandle)
{
    leftMotor  = new DcMotor(leftForwardPin,  leftBackwardPin,  gpioHostHandle, parent);
    rightMotor = new DcMotor(rightForwardPin, rightBackwardPin, gpioHostHandle, parent);

    initAHRSsensor();

    samplingFrequency = 300;
    pMadgwick = new Madgwick();
    pMadgwick->begin(samplingFrequency);

    while(!pAcc->getInterruptSource(7)) {}
    pAcc->get_Gxyz(&values[0]);
    while(!pGyro->isRawDataReadyOn()) {}
    pGyro->readGyro(&values[3]);
    while(!pMagn->isDataReady()) {}
    pMagn->ReadScaledAxis(&values[6]);

    for(int i=0; i<10000; i++) {
        pMadgwick->update(values[3], values[4], values[5],
                          values[0], values[1], values[2],
                          values[6], values[7], values[8]);
    }

    updateTimer.setTimerType(Qt::PreciseTimer);
    connect(&updateTimer, SIGNAL(timeout()),
            this, SLOT(onUpdateTimeElapsed()));
    lastUpdate = micros();
    now = lastUpdate;
    updateTimer.start(int32_t(1000.0/double(samplingFrequency)+0.5));
}


__suseconds_t
Robot::micros() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * __suseconds_t(1000000) + tv.tv_usec;
}


void
Robot::initAHRSsensor() {
    pAcc  = new ADXL345(); // init ADXL345
    pAcc->init(ACC_ADDR);
    pAcc->setRangeSetting(2); // +/-2g. Possible values are: 2g, 4g, 8g, 16g

    pGyro = new ITG3200(); // init ITG3200
    pGyro->init(ITG3200_DEF_ADDR);
    if(isStationary()) { // Gyro calibration done only when stationary
        QThread::msleep(1000);
        pGyro->zeroCalibrate(600, 10); // calibrate the ITG3200
    }
    else {
        pGyro->offsets[0] = short(GyroXOffset);
        pGyro->offsets[1] = short(GyroYOffset);
        pGyro->offsets[2] = short(GyroZOffset);
    }

    pMagn = new HMC5883L();// init HMC5883L
    pMagn->SetScale(1300); // Set the scale (in milli Gauss) of the compass.
    pMagn->SetMeasurementMode(Measurement_Continuous); // Set the measurement mode to Continuous

    //bmp085Calibration(); // init barometric pressure sensor
}


bool
Robot::isStationary() {
    return false;
}


bool
Robot::forward(double speed) {
    if(speed > 1.0) speed = 1.0;
    if(speed < 0.0) speed = 0.0;
    if(leftMotor->goForward(speed) && rightMotor->goForward(speed))
        return true;
    return false;
}


bool
Robot::backward(double speed) {
    if(speed > 1.0) speed = 1.0;
    if(speed < 0.0) speed = 0.0;
    if(leftMotor->goBackward(speed) && rightMotor->goBackward(speed))
        return true;
    return false;
}


bool
Robot::stop() {
    if(leftMotor->stop() && rightMotor->stop())
        return true;
    return false;
}


bool
Robot::left(double speed) {
    if(speed > 1.0) speed = 1.0;
    if(speed < 0.0) speed = 0.0;
    if(leftMotor->goBackward(speed) && rightMotor->goForward(speed))
        return true;
    return false;
}


bool
Robot::right(double speed) {
    if(speed > 1.0) speed = 1.0;
    if(speed < 0.0) speed = 0.0;
    if(leftMotor->goForward(speed) && rightMotor->goBackward(speed))
        return true;
    return false;
}


void
Robot::onUpdateTimeElapsed() {
    //==================================================================
    //  !!! Attention !!!
    //==================================================================
    // Reasonable convergence can be achieved in two or three iterations
    // meaning that we should operate this sensor fusion filter at a
    // rate two or three times the output data rate of the sensor.
    //
    // Deliver sensor values at the Madgwick algorithm
    // in the expected format which is rad/s, m/s² and mG (milliGauss)
    //==================================================================

    if(pAcc->getInterruptSource(7))
        pAcc->get_Gxyz(&values[0]);

    if(pGyro->isRawDataReadyOn())
        pGyro->readGyro(&values[3]);

    if(pMagn->isDataReady())
        pMagn->ReadScaledAxis(&values[6]);

    now = micros();
    delta = float(now-lastUpdate)/1000000.f;
    pMadgwick->setInvFreq(delta);
    lastUpdate = now;
    pMadgwick->update(values[3], values[4], values[5],
                      values[0], values[1], values[2],
            values[6], values[7], values[8]);
}


bool
Robot::getOrientation(float* q0, float* q1, float* q2, float* q3) {
    if(!pMadgwick) return false;
    pMadgwick->getRotation(q0, q1, q2, q3);
    return true;
}
