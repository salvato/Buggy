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
    void Gravity(bool p_gravity);
    void DollyCenter(double x, double y, double z);
    void DollyCamera(double x, double y, double z);
    void Dolly(double x, double y, double z);
    void Pitch(double d);
    void Yaw(double d);
    void Roll(double d);
    void Tilt(double d);
    void Pan(double d);
    void Set3dv(const double *p_eye, const double *p_center, const double *p_up);
    void Set(double p_eyex, double p_eyey, double p_eyez, double p_cenx, double p_ceny, double p_cenz, double p_upx, double p_upy, double p_upz);

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

    void FieldOfView(double f) {m_fieldofview = f;}
    double FieldOfView() const {return m_fieldofview;}
    bool Gravity() const {return m_gravity;}

    enum eMouseMode {PANTILT, ROLLMOVE, PITCHYAW, DOLLYXY};
    void MouseMode(eMouseMode m) {m_mousemode = m;}
    eMouseMode MouseMode() const {return m_mousemode;}
    void MouseDown(int x, int y) {m_mousex = x;  m_mousey = y;}
    void MouseMove(int x, int y);

private:
    void DollyHelper(double m[4][4], double x, double y, double z);
    void ComputeFrame();

    void RotCamera(double m[4][4]);
    void UnRotCamera(double m[4][4]);
    void RotCameraX(double m[4][4], double a);
    void RotCameraY(double m[4][4], double a);
    void RotCameraZ(double m[4][4], double a);

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
