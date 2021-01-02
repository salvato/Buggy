#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QQuaternion>


class Compass : public QOpenGLFunctions
{
public:
    Compass(QWidget* parent);
    ~Compass();

public:
    void draw(const QMatrix4x4 projectionMatrix, const QMatrix4x4 viewMatrix);

protected:
    void initGeometry();
    void initTextures();
    void initShaders();

public:
    QQuaternion angle;

private:
    QWidget*             pParent;
    QOpenGLShaderProgram compassProgram;
    GLuint               compassTexture;
    GLuint               compassVertexBuf;
};
