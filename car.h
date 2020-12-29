#pragma once

#include <QObject>
#include <QVector3D>
#include <QQuaternion>


class Car
{
public:
    Car();
    double      FromPulsesToPath(const int pulses);
    double      FromPathToAngle(const double path);
    void        Move(const int rightPulses, const int leftPulses);
    void        Reset(const int rightPulses, const int leftPulses);
    void        SetPosition(const QVector3D initialPosition);
    void        SetAngle(const double degrees);
    QVector3D   GetPosition();
    QQuaternion GetRotation();

private:
    QVector3D   Position;
    double      carAngle;
    QVector3D   StartingPosition;
    double      startingAngle;

    int         lastRPulses;
    int         lastLPulses;
    double      wheelDiameter;
    double      wheelsDistance;
    double      wheelToCenterDistance;
    int         pulsesPerRevolution;
    double      xL, zL, xR, zR;
};

