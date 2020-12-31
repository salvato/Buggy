#pragma once

#include <model.h>
#include <QObject>
#include <QVector3D>
#include <QQuaternion>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>


class Car : protected QOpenGLFunctions {

public:
    Car();
    ~Car();

public:
    double      FromPulsesToPath(const int pulses);
    double      FromPathToAngle(const double path);
    void        Move(const int rightPulses, const int leftPulses);
    void        Reset(const int rightPulses, const int leftPulses);
    void        Reset();
    void        Reset(const QVector3D initialPosition, const double degrees);
    void        SetPosition(const QVector3D initialPosition);
    void        SetAngle(const double degrees);
    QVector3D   GetPosition();
    QQuaternion GetRotation();
    bool        loadObj();
    void        initGeometry();
    void        draw(QOpenGLShaderProgram *program);

private:
    Model*      pModel;
    QString     sObjPath;
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

    GLuint cubeVertexBuf;
    GLuint cubeIndexBuf;
    GLuint floorVertexBuf;
    GLuint buggyVertexBuf;
    GLuint buggyUvBuf;
    GLuint buggyNormalBuf;


    struct
    VertexData {
        QVector3D position;
        QVector2D texCoord;
    };

    QVector<QVector3D> vertices;
    QVector<QVector2D> uvs;
    QVector<QVector3D> normals; // Not used at the present.
};

