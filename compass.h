#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class Compass : public QOpenGLFunctions
{
public:
    Compass();
    ~Compass();

public:
    void draw(const QMatrix4x4 projectionMatrix, const QMatrix4x4 viewMatrix);

protected:
    void initGeometry();
    void initTextures();
    void initShaders();

private:
    QOpenGLShaderProgram compassProgram;
    GLuint               compassTexture;
    GLuint               compassVertexBuf;
};
