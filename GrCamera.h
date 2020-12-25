#pragma once

#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>


class CGrCamera
{
public:
    CGrCamera();
    virtual ~CGrCamera();
    void  Set(const QVector3D p_eye, const QVector3D p_center, const QVector3D p_up);

    double Distance();
    void Gravity(const bool p_gravity);
    void DollyCenter(const QVector3D p);
    void DollyCamera(const QVector3D p);
    void Dolly(const QVector3D p);
    void Pitch(const double d);
    void Yaw(const double d);
    void Roll(const double d);
    void Tilt(const double d);
    void Pan(const double d);

    QVector3D Eye();
    QVector3D Up();
    QVector3D Center();

    enum eMouseMode {
        PANTILT,
        ROLLMOVE,
        PITCHYAW,
        DOLLYXY
    };
    void MouseMode(eMouseMode m);
    eMouseMode MouseMode() const;
    void MouseDown(int x, int y);
    void MouseMove(int x, int y);

    void FieldOfView(const double f);
    double FieldOfView() const;
    bool Gravity() const;

private:
    void DollyHelper(QMatrix4x4 &m, QVector3D t);
    void ComputeFrame();

//    void RotCamera(double m[4][4]);
    void RotCamera(QMatrix4x4 &m);
    void UnRotCamera(QMatrix4x4 &m);
//    void UnRotCamera(double m[4][4]);
    void RotCameraX(QMatrix4x4 &m, const double a);
    void RotCameraY(QMatrix4x4 &m, const double a);
    void RotCameraZ(QMatrix4x4 &m, const double a);
//    void RotCameraX(double m[4][4], const double a);
//    void RotCameraY(double m[4][4], const double a);
//    void RotCameraZ(double m[4][4], const double a);

private:
    QVector3D m_up;
    QVector3D m_center;
    QVector3D m_eye;

    // The camera frame.
    QVector3D m_camerax;
    QVector3D m_cameray;
    QVector3D m_cameraz;

    double m_fieldofview;
    int m_mousey;
    int m_mousex;
    eMouseMode m_mousemode;
    bool m_gravity;
};
