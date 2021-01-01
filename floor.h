#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class Floor : public QOpenGLFunctions
{
public:
    Floor();
    ~Floor();

public:
    void draw(const QMatrix4x4 projectionMatrix, const QMatrix4x4 viewMatrix);

protected:
    void initGeometry();
    void initTextures();
    void initShaders();

private:
    QOpenGLShaderProgram floorProgram;
    GLuint               floorTexture;
};
