#pragma once

#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>


class CGrCamera
{
public:
    CGrCamera();
    virtual ~CGrCamera();

    double CameraDistance();
    void Gravity(const bool p_gravity);
    void DollyCenter(const double x, const double y, const double z);
    void DollyCamera(const double x, const double y, const double z);
    void Dolly(const double x, const double y, const double z);
    void Pitch(const double d);
    void Yaw(const double d);
    void Roll(const double d);
    void Tilt(const double d);
    void Pan(const double d);
    void Set3dv(const double *p_eye, const double *p_center, const double *p_up);
    void Set(const double p_eyex, const double p_eyey, const double p_eyez,
             const double p_cenx, const double p_ceny, const double p_cenz,
             const double p_upx,  const double p_upy,  const double p_upz);

    QVector3D Eye();
    double EyeX();
    double EyeY();
    double EyeZ();

    QVector3D Up();
    double UpX();
    double UpY();
    double UpZ();

    QVector3D Center();
    double CenterX();
    double CenterY();
    double CenterZ();

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
    void DollyHelper(double m[4][4], const double x, const double y, const double z);
    void ComputeFrame();

    void RotCamera(double m[4][4]);
    void UnRotCamera(double m[4][4]);
    void RotCameraX(double m[4][4], const double a);
    void RotCameraY(double m[4][4], const double a);
    void RotCameraZ(double m[4][4], const double a);

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
