#pragma once

#include <QWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class Floor : public QOpenGLFunctions
{
public:
    Floor(QWidget* parent=nullptr);
    ~Floor();

public:
    void draw(const QMatrix4x4 projectionMatrix, const QMatrix4x4 viewMatrix);

protected:
    void initGeometry();
    void initTextures();
    void initShaders();

private:
    QWidget*             pParent;
    QOpenGLShaderProgram floorProgram;
    GLuint               floorTexture;
    GLuint               floorVertexBuf;
};
