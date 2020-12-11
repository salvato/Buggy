#ifndef SKYBOXWIDGET_H
#define SKYBOXWIDGET_H


class
        QOpenGLSkyboxWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit QOpenGLSkyboxWidget(const QString& frontImagePath,
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


    Posted on 2019-07-28 by Amin
    Creating a Skybox Using C++, Qt and OpenGL

    Skyboxes are commonly used in video games to create a realistic and wide sense of environment. In addition, they can be used to display 360 degree panoramic images, which is the reason why Computer Vision enthusiasts like myself are interested in this topic. To create a Skybox, you need a set of 6 images that correspond to the 6 sides of a cube. In this tutorial, we’ll learn how to create a Skybox using Qt with OpenGL.

    The whole idea is to create a cube using the 6 square images as the sides, then place a camera right in the middle that can be moved around using mouse. Additionally, scrolling can be used to zoom in and out. Here is a video that depicts the Qt Widget that we’ll have by the end of this tutorial:

    You can search online for countless sets of Skybox images. The one used in the preceding example is created using the following images:

    Let’s start at the end. We’re looking forward to creating an OpenGL based Qt widget, called QOpenGLSkyboxWidget, that can be added to our Qt windows and used as simply as the following example:

    QOpenGLSkyboxWidget widget("front.jpg",
                               "back.jpg",
                               "top.jpg",
                               "bottom.jpg",
                               "left.jpg",
                               "right.jpg");

    Here is the bare minimum definition of our class, similar to any other subclass of QOpenGLWidget if you’ve seen examples on this topic before:

    class QOpenGLSkyboxWidget : public QOpenGLWidget, protected QOpenGLFunctions
    {
        Q_OBJECT
    public:
        explicit QOpenGLSkyboxWidget(const QString& frontImagePath,
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

    };

    In addition, we need a bit more, in order to take care of the Skybox related stuff, zoom in and out, pan around the image and so on. Here is what you need to add to the class definition:

    protected
        void mousePressEvent(QMouseEvent *e) override;
        void mouseReleaseEvent(QMouseEvent *) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void wheelEvent(QWheelEvent *event) override;

        void timerEvent(QTimerEvent *) override;

    private:
        void loadImages();

        QOpenGLShaderProgram mProgram;
        QOpenGLTexture mTexture;
        QOpenGLBuffer mVertexBuf;
        QBasicTimer mTimer;

        struct
        {
            float verticalAngle;
            float aspectRatio;
            float nearPlane;
            float farPlane;
        } mPerspective;

        struct
        {
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

#endif // SKYBOXWIDGET_H
