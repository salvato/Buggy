// Name :        GrCamera.cpp
// Description : Implementation of the CGrCamera camera control class.  This is
//               an easy-to-use class for implementation of basic camera controls
//               such as pan/tilt/roll/dolly/etc.
// Version :     2-01-00 1.00 Initial implementation.
//               2-03-03 1.01 Version not dependent on CGrPoint and CGrTransform.
// Author :      Charles B. Owen


#include <cmath>
#include "GrCamera.h"

const double GR_PI   = 3.1415926535897932384626433832795;
//const double GR_PI2  = 2.0 * GR_PI;
//const double GR_RTOD = 180.0 / GR_PI;      // Converts radians to degrees
const double GR_DTOR = GR_PI / 180.;      // Converts degrees to radians


// Some linear algebra helper routines

inline void 
_Identity(double t[4][4]) {
    for(int i=0;  i<4;  i++)
        for(int j=0;  j<4;  j++)
            t[i][j] = i == j ? 1.0 : 0.0;
}


inline void 
_Translate(double t[4][4], const double x, const double y, const double z) {
    _Identity(t);
    t[0][3] = x;
    t[1][3] = y;
    t[2][3] = z;
}


inline void 
_RotateX(double m[4][4], const double r) {
    double rr = r * GR_DTOR;
    double cr = cos(rr);
    double sr = sin(rr);

    m[0][0] = 1;  m[0][1] = 0;   m[0][2] = 0;   m[0][3] = 0;
    m[1][0] = 0;  m[1][1] = cr;  m[1][2] =-sr;  m[1][3] = 0;
    m[2][0] = 0;  m[2][1] = sr;  m[2][2] = cr;  m[2][3] = 0;
    m[3][0] = 0;  m[3][1] = 0;   m[3][2] = 0;   m[3][3] = 1;
}


inline void 
_RotateY(double m[4][4], const double r) {
    double rr = r * GR_DTOR;
    double cr = cos(rr);
    double sr = sin(rr);

    m[0][0] = cr;  m[0][1] = 0;  m[0][2] = sr;  m[0][3] = 0;
    m[1][0] = 0;   m[1][1] = 1;  m[1][2] = 0;   m[1][3] = 0;
    m[2][0] =-sr;  m[2][1] = 0;  m[2][2] = cr;  m[2][3] = 0;
    m[3][0] = 0;   m[3][1] = 0;  m[3][2] = 0;   m[3][3] = 1;
}


inline void 
_RotateZ(double m[4][4], const double r) {
    double rr = r * GR_DTOR;
    double cr = cos(rr);
    double sr = sin(rr);

    m[0][0] = cr;  m[0][1] =-sr;  m[0][2] = 0;  m[0][3] = 0;
    m[1][0] = sr;  m[1][1] = cr;  m[1][2] = 0;  m[1][3] = 0;
    m[2][0] = 0;   m[2][1] = 0;   m[2][2] = 1;  m[2][3] = 0;
    m[3][0] = 0;   m[3][1] = 0;   m[3][2] = 0;  m[3][3] = 1;
}


inline void 
_Multiply(const double a[4][4], const double b[4][4], double res[4][4]) {
    for(int r=0;  r<4;  r++)
        for(int c=0;  c<4;  c++) {
            res[r][c] = a[r][0]*b[0][c] + a[r][1]*b[1][c] + a[r][2]*b[2][c] + a[r][3]*b[3][c];
        }
}


inline void 
_Multiply(const double a[4][4], const double b[4][4], const double c[4][4], double res[4][4]) {
    double i[4][4];
    _Multiply(a, b, i);
    _Multiply(i, c, res);
}


inline void 
_MultiplyPoint(const double m[4][4], double p[3]) {
    double x = p[0];
    double y = p[1];
    double z = p[2];
    p[0] = m[0][0]*x + m[0][1]*y + m[0][2]*z + m[0][3];
    p[1] = m[1][0]*x + m[1][1]*y + m[1][2]*z + m[1][3];
    p[2] = m[2][0]*x + m[2][1]*y + m[2][2]*z + m[2][3];
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGrCamera::CGrCamera()
{
    m_mousemode = PITCHYAW;
    m_gravity = false;
    Set(0.0, 0.0, 30.0, // Eye vector
        0.0, 0.0, 0.0,  // Center
        0.0, 1.0, 0.0); // Up vector
    FieldOfView(70.0);
}


CGrCamera::~CGrCamera() {
}


void
CGrCamera::MouseMode(eMouseMode m) {
    m_mousemode = m;
}


CGrCamera::eMouseMode
CGrCamera::MouseMode() const {
    return m_mousemode;
}


void
CGrCamera::MouseDown(int x, int y) {
    m_mousex = x;
    m_mousey = y;
}


void
CGrCamera::FieldOfView(const double f) {
    m_fieldofview = f;
}


double
CGrCamera::FieldOfView() const {
    return m_fieldofview;
}


bool
CGrCamera::Gravity() const {
    return m_gravity;
}


QVector3D
CGrCamera::Up() {
    return m_up;
}


double
CGrCamera::UpX() {
    return m_up.x();
}


double
CGrCamera::UpY() {
    return m_up.y();
}


double
CGrCamera::UpZ() {
    return m_up.z();
}


QVector3D
CGrCamera::Center() {
    return m_center;
}


double
CGrCamera::CenterX() {
    return m_center.x();
}


double
CGrCamera::CenterY() {
    return m_center.y();
}


double
CGrCamera::CenterZ() {
    return m_center.z();
}


QVector3D
CGrCamera::Eye() {
    return m_eye;
}


double
CGrCamera::EyeX() {
    return m_eye.x();
}


double
CGrCamera::EyeY() {
    return m_eye.y();
}


double
CGrCamera::EyeZ() {
    return m_eye.z();
}


void 
CGrCamera::Set(const double p_eyex, const double p_eyey, const double p_eyez,
               const double p_centerx, const double p_centery, const double p_centerz,
               const double p_upx, const double p_upy, const double p_upz)
{
    m_eye    = QVector3D(p_eyex, p_eyey, p_eyez);
    m_center = QVector3D(p_centerx, p_centery, p_centerz);
    m_up     = QVector3D(p_upx, p_upy, p_upz);
    ComputeFrame();
}


void 
CGrCamera::Set3dv(const double *p_eye, const double *p_center, const double *p_up) {
    m_eye    = QVector3D(p_eye[0], p_eye[1], p_eye[2]);
    m_center = QVector3D(p_center[0], p_center[1], p_center[2]);
    m_up     = QVector3D(p_up[0], p_up[1], p_up[2]);
    ComputeFrame();
}


// Name :         CGrCamera::ComputeFrame()
// Description :  We maintain variables that describe the X,Y,Z axis of 
//                the camera frame.  This function computes those values.
void 
CGrCamera::ComputeFrame() {
    if(m_gravity)
        m_up = QVector3D(0.0, 1.0, 0.0);
    m_cameraz = m_eye - m_center;
    m_cameraz.normalize();
    m_camerax = m_camerax.crossProduct(m_cameraz, m_up);
    m_camerax.normalize();
    m_cameray = m_cameray.crossProduct(m_cameraz, m_camerax);
}


// Camera rotation operations.
// These function rotate the camera around the eye position.
void 
CGrCamera::Pan(const double d) {
    QMatrix4x4 transform;
    transform.translate(-m_eye);        // Move to Origin
    transform.rotate(d, 0.0, 1.0, 0.0); // Then Rotate around Y
    transform.translate(m_eye);         // Back to the original position
    m_center = transform * m_center;    // Apply the transformation
    m_up     = transform * m_up;        // ...Idem...

    ComputeFrame();
}


void 
CGrCamera::Tilt(const double d) {
    QMatrix4x4 transform;
    transform.translate(-m_eye);        // Move to Origin
    transform.rotate(d, 1.0, 0.0, 0.0); // Then Rotate around X
    transform.translate(m_eye);         // Back to the original position
    m_center = transform * m_center;    // Apply the transformation
    m_up     = transform * m_up;        // ...Idem...

    ComputeFrame();
}


void 
CGrCamera::Roll(const double d) {
    QMatrix4x4 transform;
    transform.translate(-m_eye);        // Move to Origin
    transform.rotate(d, 0.0, 0.0, 1.0); // Then Rotate around Z
    transform.translate(m_eye);         // Back to the original position
    m_center = transform * m_center;    // Apply the transformation
    m_up     = transform * m_up;        // ...Idem...

    ComputeFrame();
}


// Center rotation operations.
// These function rotate the camera around the center location.
// Note that camera roll and center roll would be the same thing.
// So, we only need Yaw and Pitch.
void 
CGrCamera::Yaw(const double d) {
    QMatrix4x4 transform;
    transform.translate(-m_center);     // Move to Origin
    transform.rotate(d, 0.0, 1.0, 0.0); // Then Rotate around Y
    transform.translate(m_center);      // Back to the original position
    m_eye = transform * m_eye;          // Apply the transformation
    m_up  = transform * m_up;           // ...Idem...

    ComputeFrame();
}


void 
CGrCamera::Pitch(const double d) {
    QMatrix4x4 transform;
    transform.translate(-m_center);     // Move to Origin
    transform.rotate(d, 1.0, 0.0, 0.0); // Then Rotate around X
    transform.translate(m_center);      // Back to the original position
    m_eye = transform * m_eye;          // Apply the transformation
    m_up  = transform * m_up;           // ...Idem...
    ComputeFrame();
}


// Name :         CGrCamera::Dolly()
// Description :  A camera dolly operation moves the camera in space.
//                This function moves the camera and center together.
void 
CGrCamera::Dolly(const double x, const double y, const double z) {
    double t[4][4];
    DollyHelper(t, x, y, z);

    double center[3] = {m_center.x(), m_center.y(), m_center.z()};
    _MultiplyPoint(t, center);
    m_center = QVector3D(center[0], center[1], center[2]);

    double eye[3] = {m_eye.x(), m_eye.y(), m_eye.z()};
    _MultiplyPoint(t, eye);
    m_eye = QVector3D(eye[0], eye[1], eye[2]);

    // Frame does not change...
}


void 
CGrCamera::DollyCamera(const double x, const double y, const double z) {
    double t[4][4];
    DollyHelper(t, x, y, z);

    double eye[3] = {m_eye.x(), m_eye.y(), m_eye.z()};
    _MultiplyPoint(t, eye);
    m_eye = QVector3D(eye[0], eye[1], eye[2]);

    ComputeFrame();
}


void 
CGrCamera::DollyCenter(const double x, const double y, const double z) {
    double t[4][4];
    DollyHelper(t, x, y, z);

    double center[3] = {m_center.x(), m_center.y(), m_center.z()};
    _MultiplyPoint(t, center);
    m_center = QVector3D(center[0], center[1], center[2]);
    ComputeFrame();
}


void 
CGrCamera::DollyHelper(double m[4][4], double x, double y, double z) {
    double uncam[4][4];
    UnRotCamera(uncam);
    double tran[4][4];
    _Translate(tran, x, y, z);
    double tocam[4][4];
    RotCamera(tocam);

    _Multiply(uncam, tran, tocam, m);
}


void 
CGrCamera::MouseMove(int x, int y) {
    switch(m_mousemode) {
        case PANTILT:
            Pan((x-m_mousex) * -0.1);
            Tilt((y-m_mousey) * -0.1);
            break;

        case ROLLMOVE:
            Roll((x-m_mousex) * 0.1);
            DollyCamera(0.0, 0.0, 0.01*(y-m_mousey));
            break;

        case DOLLYXY:
            DollyCamera(0.01*(x-m_mousex), 0.01*(y-m_mousey), 0);
            break;

        case PITCHYAW:
            Yaw((x-m_mousex) * 0.8);  // GS Changed Movement Sign
            Pitch((y-m_mousey) * 0.8);// GS Changed Movement Sign
            break;
    }
    m_mousex = x;
    m_mousey = y;
}


// Name :         CGrCamera::Gravity()
// Description :  Turn on or off gravity.  Gravity simply 
//                forces the up direction to stay up.
void 
CGrCamera::Gravity(const bool p_gravity) {
    if(m_gravity == p_gravity)
        return;
    m_gravity = p_gravity;
    if(m_gravity) {
        m_up = QVector3D(0.0, 1.0, 0.0);
        ComputeFrame();
    }
}


// Name :         CGrCamera::CameraDistance()
// Description :  Returns the distance from the camera to the center
double 
CGrCamera::CameraDistance() {
    return (m_eye-m_center).length();
}


inline void 
CGrCamera::RotCamera(double m[4][4]) {
    _Identity(m);
    // m_camera is the camera frame.

    m[0][0] = m_camerax.x();
    m[0][1] = m_camerax.y();
    m[0][2] = m_camerax.z();

    m[1][0] = m_cameray.x();
    m[1][1] = m_cameray.y();
    m[1][2] = m_cameray.z();

    m[2][0] = m_cameraz.x();
    m[2][1] = m_cameraz.y();
    m[2][2] = m_cameraz.z();
}


inline void 
CGrCamera::UnRotCamera(double m[4][4]) {
    _Identity(m);
    m[0][0] = m_camerax.x();
    m[1][0] = m_camerax.y();
    m[2][0] = m_camerax.z();

    m[0][1] = m_cameray.x();
    m[1][1] = m_cameray.y();
    m[2][1] = m_cameray.z();

    m[0][2] = m_cameraz.x();
    m[1][2] = m_cameraz.y();
    m[2][2] = m_cameraz.z();
}


void 
CGrCamera::RotCameraX(double m[4][4], const double a) {
    double uncam[4][4];
    UnRotCamera(uncam);
    double rot[4][4];
    _RotateX(rot, a);
    double tocam[4][4];
    RotCamera(tocam);

    // m = uncam * rot * tocam
    _Multiply(uncam, rot, tocam, m);
}


void 
CGrCamera::RotCameraY(double m[4][4], const double a) {
    double uncam[4][4];
    UnRotCamera(uncam);
    double rot[4][4];
    _RotateY(rot, a);
    double tocam[4][4];
    RotCamera(tocam);

    _Multiply(uncam, rot, tocam, m);
}


void 
CGrCamera::RotCameraZ(double m[4][4], const double a) {
    double uncam[4][4];
    UnRotCamera(uncam);
    double rot[4][4];
    _RotateZ(rot, a);
    double tocam[4][4];
    RotCamera(tocam);

    _Multiply(uncam, rot, tocam, m);
}