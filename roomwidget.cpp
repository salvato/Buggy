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

#include <roomwidget.h>
#include <car.h>
#include <floor.h>
#include <QMouseEvent>
#include <math.h>


QT_FORWARD_DECLARE_CLASS(Car)


RoomWidget::RoomWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , QOpenGLFunctions()
    , geometries(nullptr)
    , zNear(0.1)
    , zFar(1300.0)
{
    QVector3D eyePos    = QVector3D(0.0, 30.0, -30.0);
    QVector3D centerPos = QVector3D(0.0,  0.0,   0.0);
    QVector3D upVector  = QVector3D(0.0,  1.0,   0.0);

    camera.Set(eyePos, centerPos, upVector);
    camera.FieldOfView(60.0);
    camera.MouseMode(CGrCamera::PITCHYAW);
    camera.Gravity(false);
}


RoomWidget::~RoomWidget() {
    makeCurrent();
    delete geometries;
    doneCurrent();
}


QSize
RoomWidget::minimumSizeHint() const {
    return QSize(300, 300);
}


QSize
RoomWidget::sizeHint() const {
    return QSize(800, 800);
}


void
RoomWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.1, 0.1, 0.3, 1);
    initShaders();
    initTextures();
    glEnable(GL_DEPTH_TEST); // Enable depth buffer
    pCar = new Car();
    pFloor = new Floor();

    geometries = new GeometryEngine;
}


void
RoomWidget::initShaders() {
    bool bResult = true;

    bResult &= roomProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/room.vert");
    bResult &= roomProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/room.frag");
    bResult &= roomProgram.link();
    if(!bResult) {
        perror("Unble to initShaders()...exiting");
        exit(EXIT_FAILURE);
    }
}


void
RoomWidget::initTextures() {
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
RoomWidget::resizeGL(int w, int h) {
    aspect = qreal(w) / qreal(h ? h : 1);
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(camera.GetFieldOfView(), aspect, zNear, zFar);
    orthoMatrix.setToIdentity();
    orthoMatrix.ortho(0, w, 0, h, zNear, zFar);
}


void
RoomWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Camera matrix
    viewMatrix.setToIdentity();
    //viewMatrix.lookAt(camera.Eye(), camera.Center(), camera.Up());
    viewMatrix.lookAt(camera.Eye(), pCar->GetPosition(), camera.Up());

    pFloor->draw(projectionMatrix, viewMatrix);
    pCar->draw(projectionMatrix, viewMatrix);

/*
    // Room
    modelMatrix.setToIdentity();
    modelMatrix.scale(500.0, 500.0, 500.0);
    modelMatrix.translate(0.0, 0.0, 0.0);
    glDisable(GL_CULL_FACE);  // Disable back face culling

    roomProgram.bind();
    roomProgram.setUniformValue("mvp_matrix", projectionMatrix*viewMatrix*modelMatrix);

    glBindTexture(GL_TEXTURE_CUBE_MAP, roomTexture);
//    geometries->drawRoom(&roomProgram);
*/
}


void
RoomWidget::mousePressEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::RightButton) {
        camera.MouseDown(event->x(), event->y());
        camera.MouseMode(CGrCamera::ROLLMOVE);
        event->accept();
    }
    else if(event->buttons() & Qt::LeftButton) {
        camera.MouseDown(event->x(), event->y());
        event->accept();
    }

    update();
}


void
RoomWidget::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() & Qt::RightButton) {
        camera.MouseMode(CGrCamera::PITCHYAW);
        event->accept();
    }
    else if(event->button() & Qt::LeftButton) {
        camera.MouseMode(CGrCamera::PITCHYAW);
        event->accept();
    }

    update();
}


void
RoomWidget::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() & Qt::LeftButton) {
        camera.MouseMove(event->x(), event->y());
        event->accept();
    }
    else if(event->buttons() & Qt::RightButton) {
        camera.MouseMove(event->x(), event->y());
        event->accept();
    }

    update();
}


void
RoomWidget::wheelEvent(QWheelEvent* event) {
    QPoint numDegrees = event->angleDelta();
    if(!numDegrees.isNull()) {
        camera.MouseDown(0, 0);
        camera.MouseMode(CGrCamera::ROLLMOVE);
        camera.MouseMove(0, -numDegrees.y());
        camera.MouseMode(CGrCamera::PITCHYAW);

        event->accept();
    }

    update();
}
