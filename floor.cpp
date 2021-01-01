#include "floor.h"
#include <QImage>


Floor::Floor()
{

}


Floor::~Floor() {

}


void
Floor::initGeometry() {

}


void
Floor::initTextures() {
    const QImage floorImage = QImage(":/Pavimento.jpg")
                             .convertToFormat(QImage::Format_RGBA8888)
                             .mirrored();
    glGenTextures(1, &floorTexture);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

}
