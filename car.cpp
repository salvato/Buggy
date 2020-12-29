#include "car.h"
#include <QtMath>
#include <QMatrix4x4>


// 12 CPR Quadrature Encoder
// Motor Gear Ratio 1:9
// Quadrature encoder mode 3 (x4 mode)


Car::Car() {
    wheelDiameter         = 0.69; // in dm
    wheelsDistance        = 2.0;  // in dm
    wheelToCenterDistance = 0.5*wheelsDistance; // in dm
    pulsesPerRevolution   = 12*4*9;

    Reset(0, 0);
}


double
Car::FromPulsesToPath(const int pulses) {
    double path = (double(pulses)/double(pulsesPerRevolution))*M_PI*wheelDiameter;
    return path;
}


double
Car::FromPathToAngle(const double path) {
    double angle = fmod(path/wheelToCenterDistance, 2.0*M_PI);
    return angle;
}


void
Car::Move(const int rightPulses, const int leftPulses) {
    QMatrix4x4 transform;
    double rightAngle = FromPathToAngle(FromPulsesToPath(rightPulses-lastRPulses));
    double leftAngle  = FromPathToAngle(FromPulsesToPath(leftPulses -lastLPulses));

    lastRPulses = rightPulses;
    lastLPulses = leftPulses;

    // Let only the Right Wheel to move
    xL = carPosition.x() - wheelToCenterDistance * cos(carAngle);
    zL = carPosition.z() + wheelToCenterDistance * sin(carAngle);

    transform.setToIdentity();

    transform.translate(xL, 0.0, zL);
    transform.rotate(qRadiansToDegrees(rightAngle), QVector3D(0.0, 1.0, 0.0));
    transform.translate(-xL, 0.0, -zL);
    carPosition = transform*carPosition;
    Position = carPosition;// += transform*QVector3D(0.0, 1.0, 0.0) - QVector3D(0.0, 1.0, 0.0);
    carAngle += rightAngle;

    // Now move only the Left Wheel
    xR = carPosition.x() + wheelToCenterDistance * cos(carAngle);
    zR = carPosition.z() - wheelToCenterDistance * sin(carAngle);

    transform.setToIdentity();

    transform.translate(xR, 0.0, zR);
    transform.rotate(qRadiansToDegrees(-leftAngle), QVector3D(0.0, 1.0, 0.0));
    transform.translate(-xR, 0.0, -zR);
    carPosition = transform*carPosition;
    Position = carPosition;// += transform*QVector3D(0.0, 1.0, 0.0) - QVector3D(0.0, 1.0, 0.0);
    carAngle -= leftAngle;
    carAngle = fmod(carAngle, 2.0*M_PI);
    Rotation = QQuaternion::fromAxisAndAngle(QVector3D(0.0, 1.0, 0.0), qRadiansToDegrees(carAngle));

    qDebug()
             << "Angle =" << qRadiansToDegrees(carAngle)
             << "xL=" << xL
             << "zL=" << zL
             << "X:" << carPosition.x()
             << "Z:" << carPosition.z();
}


void
Car::Reset(const int rightPulses, const int leftPulses) {
    lastRPulses = rightPulses;
    lastLPulses = leftPulses;

    carPosition = QVector3D(0.0, 0.0, 0.0);
    carAngle = 0.0;
    Rotation = QQuaternion::fromAxisAndAngle(QVector3D(0.0, 1.0, 0.0), qRadiansToDegrees(carAngle));
}


QVector3D Car::GetPosition() {
    return Position;
}


QQuaternion
Car::GetRotation() {
    return Rotation;
}
