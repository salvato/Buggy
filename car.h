#pragma once

#include <QObject>
#include <QVector3D>
#include <QQuaternion>


class Car
{
public:
    Car();
    double FromPulsesToPath(const int pulses);
    double FromPathToAngle(const double path);
    void Move(const int rightPulses, const int leftPulses);
    void Reset(const int rightPulses, const int leftPulses);
    void SetPosition(const QVector3D initialPosition);
    void SetRotation(QQuaternion initialRotation);
    QVector3D GetPosition();
    QQuaternion GetRotation();

private:
    QVector3D Position;
    QQuaternion Rotation;
    QVector3D StartingPosition;
    QQuaternion StartingRotation;
    double wheelDiameter;
    double wheelsDistance;
    double wheelToCenterDistance;
    int    pulsesPerRevolution;
    double carAngle;
    int    lastRPulses;
    int    lastLPulses;
    double xL, zL, xR, zR;
};

