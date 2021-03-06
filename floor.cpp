#include "floor.h"
#include <QImage>


Floor::Floor(QWidget* parent)
    : pParent(parent)
{
    floorTexture = -1;
    floorVertexBuf = -1;
    initializeOpenGLFunctions();
    initGeometry();
    initTextures();
    initShaders();
}


Floor::~Floor() {
    glDeleteBuffers(1, &floorVertexBuf);
}


void
Floor::initGeometry() {
    QVector3D vertices[] = {
        QVector3D(-8.0f,  0.0f, -8.0f),
        QVector3D( 8.0f,  0.0f, -8.0f),
        QVector3D( 8.0f,  0.0f,  8.0f),

        QVector3D( 8.0f,  0.0f,  8.0f),
        QVector3D(-8.0f,  0.0f,  8.0f),
        QVector3D(-8.0f,  0.0f, -8.0f)
    };
    glGenBuffers(1, &floorVertexBuf);
    glBindBuffer(GL_ARRAY_BUFFER, floorVertexBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}


void
Floor::initTextures() {
    const QImage floorImage = QImage(":/Pavimento.jpg")
                             .convertToFormat(QImage::Format_RGBA8888);
    glGenTextures(1, &floorTexture);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 floorImage.width(),
                 floorImage.height(),
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 (void*)floorImage.bits()
                );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}


void
Floor::initShaders() {
    bool bResult = true;
    bResult &= floorProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/floor.vert");
    bResult &= floorProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/floor.frag");
    bResult &= floorProgram.link();
    if(!bResult) {
        perror("Unble to init Floor Shaders()...exiting");
        exit(EXIT_FAILURE);
    }
}


void
Floor::draw(const QMatrix4x4 projectionMatrix, const QMatrix4x4 viewMatrix) {
    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    modelMatrix.translate(0.0, 0.0, 0.0);
    modelMatrix.scale(10.0, 1.0, 10.0);

    // Bind shader pipeline for use
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glBindBuffer(GL_ARRAY_BUFFER, floorVertexBuf);
    floorProgram.bind();

    floorProgram.setUniformValue("mvp_matrix", projectionMatrix*viewMatrix*modelMatrix);
    int vertexLocation = floorProgram.attributeLocation("vertexPosition");
    floorProgram.enableAttributeArray(vertexLocation);
    floorProgram.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, 3*sizeof(float));

    glDisable(GL_CULL_FACE); // Enable back face culling
    glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 6 vertices -> 2 triangles

    floorProgram.disableAttributeArray(vertexLocation);
}
