// Name :        GrCamera.cpp
// Description : Implementation of the CGrCamera camera control class.  This is
//               an easy-to-use class for implementation of basic camera controls
//               such as pan/tilt/roll/dolly/etc.
// Version :     2-01-00 1.00 Initial implementation.
//               2-03-03 1.01 Version not dependent on CGrPoint and CGrTransform.
// Author :      Charles B. Owen


#include <cmath>
#include "GrCamera.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGrCamera::CGrCamera()
{
    m_mousemode = PITCHYAW;
    m_gravity = false;
    Set(QVector3D(0.0, 0.0, 30.0), // Eye vector
        QVector3D(0.0, 0.0, 0.0),  // Center
        QVector3D(0.0, 1.0, 0.0)); // Up vector
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
CGrCamera::GetFieldOfView() const {
    return m_fieldofview;
}


bool
CGrCamera::GetGravity() const {
    return m_gravity;
}


QVector3D
CGrCamera::Up() {
    return m_up;
}


QVector3D
CGrCamera::Center() {
    return m_center;
}


QVector3D
CGrCamera::Eye() {
    return m_eye;
}


void
CGrCamera::Set(const QVector3D p_eye,
               const QVector3D p_center,
               const QVector3D p_up)
{
    m_eye    = p_eye;
    m_center = p_center;
    m_up     = p_up;
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
CGrCamera::Dolly(const QVector3D p) {
   QMatrix4x4 t;
   DollyHelper(t, p);
   m_center = t * m_center;
   m_eye    = t * m_eye;
}


void
CGrCamera::DollyCamera(const QVector3D p) {
    QMatrix4x4 t;
    DollyHelper(t, p);
    m_eye = t * m_eye;
    ComputeFrame();
}


void
CGrCamera::DollyCenter(const QVector3D p) {
    QMatrix4x4 t;
    DollyHelper(t, p);
    m_center = t * m_center;
    ComputeFrame();
}


void
CGrCamera::DollyHelper(QMatrix4x4 &m, QVector3D t) {
    QMatrix4x4 uncam;
    UnRotCamera(uncam);
    QMatrix4x4 tran;
    tran.translate(t);
    QMatrix4x4 tocam;
    RotCamera(tocam);
    m = uncam*tran*tocam;
}


void 
CGrCamera::MouseMove(int x, int y) {
    switch(m_mousemode) {
        case PANTILT:
            Pan((x-m_mousex)  * -0.1);
            Tilt((y-m_mousey) * -0.1);
            break;

        case ROLLMOVE:
            Roll((x-m_mousex) * 0.1);
            DollyCamera(QVector3D(0.0, 0.0, 0.01*(y-m_mousey)));
            break;

        case DOLLYXY:
            DollyCamera(QVector3D(0.01*(x-m_mousex), 0.01*(y-m_mousey), 0));
            break;

        case PITCHYAW:
            Yaw((x-m_mousex)   * 0.8); // GS Changed Movement Sign
            Pitch((y-m_mousey) * 0.8); // GS Changed Movement Sign
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
CGrCamera::Distance() {
    return (m_eye-m_center).length();
}


inline void
CGrCamera::RotCamera(QMatrix4x4 &m) {
    m.setToIdentity();
    // m_camera is the camera frame.
    m(0, 0) = m_camerax.x();
    m(0, 1) = m_camerax.y();
    m(0, 2) = m_camerax.z();

    m(1, 0) = m_cameray.x();
    m(1, 1) = m_cameray.y();
    m(1, 2) = m_cameray.z();

    m(2, 0) = m_cameraz.x();
    m(2, 1) = m_cameraz.y();
    m(2, 2) = m_cameraz.z();
}


inline void
CGrCamera::UnRotCamera(QMatrix4x4 &m) {
    m.setToIdentity();
    m(0, 0) = m_camerax.x();
    m(1, 0) = m_camerax.y();
    m(2, 0) = m_camerax.z();

    m(0, 1) = m_cameray.x();
    m(1, 1) = m_cameray.y();
    m(2, 1) = m_cameray.z();

    m(0, 2) = m_cameraz.x();
    m(1, 2) = m_cameraz.y();
    m(2, 2) = m_cameraz.z();
}


void
CGrCamera::RotCameraX(QMatrix4x4 &m, const double a) {
    QMatrix4x4 uncam;
    UnRotCamera(uncam);
    QMatrix4x4 rot;
    rot.rotate(a, QVector3D(1.0, 0.0, 0.0));
    QMatrix4x4 tocam;
    RotCamera(tocam);

    m = uncam * rot * tocam;
}


void
CGrCamera::RotCameraY(QMatrix4x4 &m, const double a) {
    QMatrix4x4 uncam;
    UnRotCamera(uncam);
    QMatrix4x4 rot;
    rot.rotate(a, QVector3D(0.0, 1.0, 0.0));
    QMatrix4x4 tocam;
    RotCamera(tocam);

    m = uncam * rot * tocam;
}


void
CGrCamera::RotCameraZ(QMatrix4x4 &m, const double a) {
    QMatrix4x4 uncam;
    UnRotCamera(uncam);
    QMatrix4x4 rot;
    rot.rotate(a, QVector3D(0.0, 0.0, 1.0));
    QMatrix4x4 tocam;
    RotCamera(tocam);

    m = uncam * rot * tocam;
}

