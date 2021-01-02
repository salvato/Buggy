#include "compass.h"
#include <QWidget>
#include <QImage>
#include <QtMath>


Compass::Compass(QWidget* parent)
    : pParent(parent)
{
    angle = QQuaternion();
    compassTexture   = -1;
    compassVertexBuf = -1;
    initializeOpenGLFunctions();
    initGeometry();
    initTextures();
    initShaders();
}


Compass::~Compass() {
    glDeleteBuffers(1, &compassVertexBuf);
}


void
Compass::initGeometry() {
    QVector3D vertices[] = {
        QVector3D(-0.5f, -0.5f,  0.0f),
        QVector3D( 0.5f, -0.5f,  0.0f),
        QVector3D( 0.5f,  0.5f,  0.0f),

        QVector3D( 0.5f,  0.5f,  0.0f),
        QVector3D(-0.5f,  0.5f,  0.0f),
        QVector3D(-0.5f, -0.5f,  0.0f),
    };
    glGenBuffers(1, &compassVertexBuf);
    glBindBuffer(GL_ARRAY_BUFFER, compassVertexBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}


void
Compass::initTextures() {
    const QImage compassImage = QImage(":/Compass.png")
                                .convertToFormat(QImage::Format_RGBA8888)
                                .mirrored();
    glGenTextures(1, &compassTexture);
    glBindTexture(GL_TEXTURE_2D, compassTexture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 compassImage.width(),
                 compassImage.height(),
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 (void*)compassImage.bits()
                );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}


void
Compass::initShaders() {
    bool bResult = true;
    bResult &= compassProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/compass.vert");
    bResult &= compassProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/compass.frag");
    bResult &= compassProgram.link();

    if(!bResult) {
        perror("Unable to init Compass Shaders()...exiting");
        exit(EXIT_FAILURE);
    }
}


void
Compass::draw(const QMatrix4x4 projectionMatrix, const QMatrix4x4 viewMatrix) {
    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    modelMatrix.translate(0.5*pParent->height(), 0.5*pParent->height(), 0.0);
    modelMatrix.scale(0.9*pParent->height(), 0.9*pParent->height(), 1.0);
    QVector3D euler = angle.toEulerAngles();
    modelMatrix.rotate(euler.z(), QVector3D(0.0, 0.0, -1.0));

    glBindTexture(GL_TEXTURE_2D, compassTexture);
    glBindBuffer(GL_ARRAY_BUFFER, compassVertexBuf);
    compassProgram.bind();

    compassProgram.setUniformValue("mvp_matrix", projectionMatrix*viewMatrix*modelMatrix);
    int vertexLocation = compassProgram.attributeLocation("vertexPosition");
    compassProgram.enableAttributeArray(vertexLocation);
    compassProgram.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    glDisable(GL_CULL_FACE); // Enable back face culling
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 6 vertices -> 2 triangles
}
