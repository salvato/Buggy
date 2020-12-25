/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "GLwidget.h"
#include <QMouseEvent>
#include <math.h>


GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , geometries(nullptr)
    , cubeTexture(nullptr)
    , zNear(0.1)
    , zFar(1300.0)
    , carPosition(QVector3D(0.0, 0.0, 0.0))
{ 
    m_trackBalls[0] = TrackBall(0.05f,  QVector3D(0, 1, 0), TrackBall::Sphere);
    m_trackBalls[1] = TrackBall(0.005f, QVector3D(0, 0, 1), TrackBall::Sphere);
    m_trackBalls[2] = TrackBall(0.0f,   QVector3D(0, 1, 0), TrackBall::Plane);
    distExp = 600;

    camera.Set(QVector3D(0.0,  -30.0, -30.0), // Eye (Position of the Camera)
               QVector3D(0.0,  0.0,   0.0),   // Center
               QVector3D(0.0,  1.0,   0.0));  // Up Vector
    camera.FieldOfView(60.0);
    camera.MouseMode(CGrCamera::PITCHYAW);
    camera.Gravity(true);
}


GLWidget::~GLWidget() {
    makeCurrent();
    delete cubeTexture;
    delete geometries;
    doneCurrent();
}


QSize
GLWidget::minimumSizeHint() const {
    return QSize(300, 300);
}


QSize
GLWidget::sizeHint() const {
    return QSize(800, 800);
}


void
GLWidget::setCarPosition(double x, double y, double z) {
    carPosition = QVector3D(x, y, z);
}


void
GLWidget::setCarPosition(QVector3D position) {
    carPosition = position;
}


void
GLWidget::setRotation(float q0, float q1, float q2, float q3) {
    rotation = QQuaternion(q0, q1, q2, q3);
}


void
GLWidget::setRotation(QQuaternion newRotation) {
    rotation = newRotation;
}


void
GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.1, 0.1, 0.4, 1);
    initShaders();
    initTextures();
    glEnable(GL_DEPTH_TEST); // Enable depth buffer
    geometries = new GeometryEngine;
}


void
GLWidget::initShaders() {
    bool bResult;
    bResult  = buggyProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/buggy.vert");
    bResult &= buggyProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/buggy.frag");
    bResult &= buggyProgram.link();

    bResult &= cubeProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/cube.vert");
    bResult &= cubeProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/cube.frag");
    bResult &= cubeProgram.link();

    bResult &= floorProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/floor.vert");
    bResult &= floorProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/floor.frag");
    bResult &= floorProgram.link();

    bResult &= roomProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/room.vert");
    bResult &= roomProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/room.frag");
    bResult &= roomProgram.link();
/*
    bResult &= modelProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/model.vert");
    bResult &= modelProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/model.frag");
    bResult &= modelProgram.link();
*/
    if(!bResult) {
        perror("Unble to initShaders()...exiting");
        exit(EXIT_FAILURE);
    }
}


void
GLWidget::initTextures() {
    const QImage cubeImage = QImage(":/cube.png").mirrored();
    cubeTexture = new QOpenGLTexture(cubeImage);
    cubeTexture->setMinificationFilter(QOpenGLTexture::Nearest);
    cubeTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    cubeTexture->setWrapMode(QOpenGLTexture::Repeat);

    const QImage floorImage = QImage(":/Pavimento.jpg").mirrored();
    floorTexture = new QOpenGLTexture(floorImage);
    floorTexture->setMinificationFilter(QOpenGLTexture::Nearest);
    floorTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    floorTexture->setWrapMode(QOpenGLTexture::Repeat);

    // Cubemap Textures MUST have the same size !!!
    const QImage posx = QImage(":/Pietra.jpg").convertToFormat(QImage::Format_RGBA8888);
    const QImage negx = QImage(":/Pietra.jpg").convertToFormat(QImage::Format_RGBA8888);
    const QImage posy = QImage(":/Pietra.jpg").convertToFormat(QImage::Format_RGBA8888);
    const QImage negy = QImage(":/Pavimento.jpg").convertToFormat(QImage::Format_RGBA8888);
    const QImage posz = QImage(":/Pietra.jpg").convertToFormat(QImage::Format_RGBA8888);
    const QImage negz = QImage(":/Pietra.jpg").convertToFormat(QImage::Format_RGBA8888);

    glGenTextures(1, &roomTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, roomTexture);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, posx.width(), posx.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*) posx.bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, posx.width(), posx.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*) posy.bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, posx.width(), posx.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*) posz.bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, posx.width(), posx.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*) negx.bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, posx.width(), posx.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*) negy.bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, posx.width(), posx.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*) negz.bits());
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
}


void
GLWidget::resizeGL(int w, int h) {
    aspect = qreal(w) / qreal(h ? h : 1);
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(camera.FieldOfView(), aspect, zNear, zFar);
}


void
GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Camera matrix
    viewMatrix.setToIdentity();
    viewMatrix.lookAt(camera.Eye(),
                      camera.Center(),
                      camera.Up());

    // Floor Model matrix
    modelMatrix.setToIdentity();
    modelMatrix.translate(1.0, 0.0, 0.0);
    modelMatrix.scale(10.0, 1.0, 10.0);

    // Bind shader pipeline for use
    floorProgram.bind();
    floorProgram.setUniformValue("mvp_matrix", projectionMatrix*viewMatrix*modelMatrix);

    glDisable(GL_CULL_FACE); // Disable back face culling
    floorTexture->bind();
//    geometries->drawFloor(&floorProgram);

    // Room model Matrix
    modelMatrix.setToIdentity();
    modelMatrix.scale(500.0, 500.0, 500.0);
    modelMatrix.translate(0.0, 0.0, 0.0);
    glDisable(GL_CULL_FACE);  // Disable back face culling

    roomProgram.bind();
    roomProgram.setUniformValue("mvp_matrix", projectionMatrix*viewMatrix*modelMatrix);

    glBindTexture(GL_TEXTURE_CUBE_MAP, roomTexture);
//    geometries->drawRoom(&roomProgram);

    // Buggy Model matrix
    modelMatrix.setToIdentity();
    modelMatrix.translate(carPosition+QVector3D(0.0, 1.0, 0.0));
    modelMatrix.rotate(rotation);

    // Bind shader pipeline for use
    cubeProgram.bind();
    cubeProgram.setUniformValue("mvp_matrix", projectionMatrix*viewMatrix*modelMatrix);

    glEnable(GL_CULL_FACE); // Enable back face culling
    cubeTexture->bind();
    geometries->drawCube(&cubeProgram);

//    geometries->drawBuggy(&buggyProgram);
}


QPointF
GLWidget::pixelPosToViewPos(const QPointF& p) {
    return QPointF(2.0*float(p.x())/width()-1.0,
                   1.0-2.0*float(p.y())/height());
}


void
GLWidget::mousePressEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::RightButton) {
        camera.MouseDown(event->x(), event->y());
        camera.MouseMode(CGrCamera::ROLLMOVE);
        event->accept();
    }
    else if(event->buttons() & Qt::LeftButton) {
        camera.MouseDown(event->x(), event->y());
        event->accept();
    }
/*
    if(event->buttons() & Qt::LeftButton) {
        m_trackBalls[0].move(pixelPosToViewPos(event->pos()), m_trackBalls[2].rotation().conjugated());
        event->accept();
    }
    else {
        m_trackBalls[0].release(pixelPosToViewPos(event->pos()), m_trackBalls[2].rotation().conjugated());
    }

    if(event->buttons() & Qt::RightButton) {
        m_trackBalls[1].move(pixelPosToViewPos(event->pos()), m_trackBalls[2].rotation().conjugated());
        event->accept();
    }
    else {
        m_trackBalls[1].release(pixelPosToViewPos(event->pos()), m_trackBalls[2].rotation().conjugated());
    }

    if(event->buttons() & Qt::MiddleButton) {
        m_trackBalls[2].move(pixelPosToViewPos(event->pos()), QQuaternion());
        event->accept();
    }
    else {
        m_trackBalls[2].release(pixelPosToViewPos(event->pos()), QQuaternion());
    }
*/
    update();
}


void
GLWidget::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() & Qt::RightButton) {
        camera.MouseMode(CGrCamera::PITCHYAW);
        event->accept();
    }
    else if(event->button() & Qt::LeftButton) {
        camera.MouseMode(CGrCamera::PITCHYAW);
        event->accept();
    }
/*
    if(event->button() == Qt::LeftButton) {
        m_trackBalls[0].release(pixelPosToViewPos(event->pos()), m_trackBalls[2].rotation().conjugated());
        event->accept();
    }

    if(event->button() == Qt::RightButton) {
        m_trackBalls[1].release(pixelPosToViewPos(event->pos()), m_trackBalls[2].rotation().conjugated());
        event->accept();
    }

    if(event->button() == Qt::MiddleButton) {
        m_trackBalls[2].release(pixelPosToViewPos(event->pos()), QQuaternion());
        event->accept();
    }
*/
    update();
}


void
GLWidget::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton) {
        camera.MouseMove(event->x(), event->y());
        event->accept();
    }
    else if(event->buttons() & Qt::RightButton) {
        camera.MouseMove(event->x(), event->y());
        event->accept();
    }
/*
    if(event->buttons() & Qt::LeftButton) {
        m_trackBalls[0].move(pixelPosToViewPos(event->pos()), m_trackBalls[2].rotation().conjugated());
        event->accept();
    }
    else {
        m_trackBalls[0].release(pixelPosToViewPos(event->pos()), m_trackBalls[2].rotation().conjugated());
    }

    if(event->buttons() & Qt::RightButton) {
        m_trackBalls[1].move(pixelPosToViewPos(event->pos()), m_trackBalls[2].rotation().conjugated());
        event->accept();
    }
    else {
        m_trackBalls[1].release(pixelPosToViewPos(event->pos()), m_trackBalls[2].rotation().conjugated());
    }

    if(event->buttons() & Qt::MiddleButton) {
        m_trackBalls[2].move(pixelPosToViewPos(event->pos()), QQuaternion());
        event->accept();
    }
    else {
        m_trackBalls[2].release(pixelPosToViewPos(event->pos()), QQuaternion());
    }
*/
    update();
}


void
GLWidget::wheelEvent(QWheelEvent* event) {
    QPoint numDegrees = event->angleDelta();
    if(!numDegrees.isNull()) {
        camera.MouseDown(0, 0);
        camera.MouseMode(CGrCamera::ROLLMOVE);
        camera.MouseMove(0, -numDegrees.y());
        camera.MouseMode(CGrCamera::PITCHYAW);
//        QVector3D oldCenter = QVector3D(camera.Center()[0], camera.Center()[1], camera.Center()[2]);
//        //oldCenter.setX(oldCenter.x()+numDegrees.ry()/10.0);
//        oldCenter.setZ(oldCenter.z()+numDegrees.ry()/10.0);
//        camera.SetCenter(oldCenter);
        event->accept();
    }
/*
    distExp += event->angleDelta().x();
    if (distExp < -8 * 120)
        distExp = -8 * 120;
    if (distExp > 10 * 120)
        distExp = 10 * 120;
    event->accept();
*/
    update();
}
