#pragma once


#include <QVector3D>


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

    void FieldOfView(double f) {m_fieldofview = f;}
    double FieldOfView() const {return m_fieldofview;}

    const double *Eye() const {return m_eye;}
    double EyeX() const {return m_eye[0];}
    double EyeY() const {return m_eye[1];}
    double EyeZ() const {return m_eye[2];}

    double* Up();
    double UpX();
    double UpY();
    double UpZ();

    const double *Center() const {return m_center;}
    double CenterX() const {return m_center[0];}
    double CenterY() const {return m_center[1];}
    double CenterZ() const {return m_center[2];}

    bool Gravity() const {return m_gravity;}
    enum eMouseMode {PANTILT, ROLLMOVE, PITCHYAW, DOLLYXY};
    void MouseMode(eMouseMode m) {m_mousemode = m;}
    eMouseMode MouseMode() const {return m_mousemode;}
    void MouseDown(int x, int y) {m_mousex = x;  m_mousey = y;}
    void MouseMove(int x, int y);

private:
    QVector3D m_up;
    double m_center[3];
    double m_eye[3];

    double m_fieldofview;
    int m_mousey;
    int m_mousex;
    eMouseMode m_mousemode;
    void DollyHelper(double m[4][4], double x, double y, double z);
    void ComputeFrame();
    bool m_gravity;

    // The camera frame.
    QVector3D m_camerax;
    QVector3D m_cameray;
    QVector3D m_cameraz;

    void RotCamera(double m[4][4]);
    void UnRotCamera(double m[4][4]);
    void RotCameraX(double m[4][4], double a);
    void RotCameraY(double m[4][4], double a);
    void RotCameraZ(double m[4][4], double a);
};
