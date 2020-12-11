#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QBasicTimer>


class
SkyboxWidget
    : public QOpenGLWidget
    , protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit SkyboxWidget(const QString& frontImagePath,
                          const QString& backImagePath,
                          const QString& topImagePath,
                          const QString& bottomImagePath,
                          const QString& leftImagePath,
                          const QString& rightImagePath,
                          QWidget *parent = Q_NULLPTR);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void timerEvent(QTimerEvent *) override;

    private:
        void loadImages();

        QOpenGLShaderProgram mProgram;
        QOpenGLTexture       mTexture;
        QOpenGLBuffer        mVertexBuf;
        QBasicTimer          mTimer;

        struct {
            float verticalAngle;
            float aspectRatio;
            float nearPlane;
            float farPlane;
        } mPerspective;

        struct {
            QVector3D eye;
            QVector3D center;
            QVector3D up;
        } mLookAt;

        QMatrix4x4 mModelMat, mViewMat, mProjectionMat;

        QVector3D mRotationAxis;
        QQuaternion mRotation;

        QVector2D mMousePressPosition;
        float mSpeed;

        QString mFrontImagePath;
        QString mBackImagePath;
        QString mTopImagePath;
        QString mBottomImagePath;
        QString mLeftImagePath;
        QString mRightImagePath;
};
