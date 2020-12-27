#include "car.h"
#include <QtMath>
#include <QMatrix4x4>


// 12 CPR Quadrature Encoder
// Motor Gear Ratio 1:9
// Quadrature encoder mode 3 (x4 mode)


Car::Car()
{
    wheelDiameter         = 0.69; // in dm
    wheelDistance         = 2.0;  // in dm
    pulsesPerRevolution   = 12*4*9;

    Position = QVector3D(0.0, 0.0, 0.0);
    Rotation = QQuaternion();
    carAngle = 0.0;

    wheelToCenterDistance = 0.5*wheelDistance; // in dm
    lastRPulses = 0;
    lastLPulses = 0;
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
    QVector3D P0 = Position;
    double rightAngle = FromPathToAngle(FromPulsesToPath(rightPulses-lastRPulses));
    double leftAngle  = FromPathToAngle(FromPulsesToPath(leftPulses-lastLPulses));

    lastRPulses = rightPulses;
    lastLPulses = leftPulses;

    QMatrix4x4 transform;

    transform.setToIdentity();
    xL = P0.x()-wheelToCenterDistance*cos(carAngle);
    zL = P0.z()+wheelToCenterDistance*sin(carAngle);

    transform.translate(-xL, 0.0, -zL);
    transform.rotate(rightAngle, QVector3D(0.0, 1.0, 0.0));
    //transform.translate(xL, 0.0, zL);

    Position = transform*Position;

//    carAngle += rightAngle;
//    carAngle = fmod(carAngle, 2.0*M_PI);

    transform.setToIdentity();
    xR = P0.x()+wheelToCenterDistance*cos(carAngle);
    zR = P0.z()-wheelToCenterDistance*sin(carAngle);
    transform.translate(-xR, 0.0, -zR);
    transform.rotate(leftAngle, QVector3D(0.0, 1.0, 0.0));
    //transform.translate(xR, 0.0, zR);

    Position = transform*Position;

    carAngle += rightAngle-leftAngle;
    carAngle = fmod(carAngle, 2.0*M_PI);

    Rotation = QQuaternion::fromAxisAndAngle(QVector3D(0.0, 1.0, 0.0), qRadiansToDegrees(carAngle));

    qDebug() << "Angle ="
             << qRadiansToDegrees(carAngle)
             << "xL="
             << xL
             << "zL="
             << zL
             << "X:"
             << Position.x()
             << "Y:"
             << Position.z();
}


void
Car::Reset(const int rightPulses, const int leftPulses) {
    lastRPulses = rightPulses;
    lastLPulses = leftPulses;
    Position = QVector3D(0.0, 0.0, 0.0);
    Rotation = QQuaternion();
}


QVector3D
Car::GetPosition() {
    return Position;
}


QQuaternion
Car::GetRotation() {
    return Rotation;
}
