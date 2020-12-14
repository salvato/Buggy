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


GLWidget::GLWidget(CGrCamera* myCamera, QWidget *parent)
    : QOpenGLWidget(parent)
    , geometries(nullptr)
    , camera(myCamera)
    , cubeTexture(nullptr)
    , roomTexture(nullptr)
    , zNear(0.1)
    , zFar(1300.0)
{
}


GLWidget::~GLWidget() {
    makeCurrent();
    delete cubeTexture;
    delete geometries;
    doneCurrent();
}


QSize
GLWidget::minimumSizeHint() const {
    return QSize(50, 50);
}


QSize
GLWidget::sizeHint() const {
    return QSize(800, 800);
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
    glClearColor(0, 0, 0, 1);
    initShaders();
    initTextures();
    glEnable(GL_DEPTH_TEST); // Enable depth buffer
    geometries = new GeometryEngine;
}


void
GLWidget::initShaders() {
    if(!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/cube.vert")) {
        perror("Missing Cube Vertex Shader");
        close();
    }
    if(!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/cube.frag")) {
        perror("Missing Cube Fragment Shader");
        close();
    }
    if(!program.link()) {
        perror("Cube Shader linking Error");
        close();
    }
    if(!program.bind()) {// Bind shader pipeline for use
        perror("Cube Shader binding Error");
        close();
    }

    if(!mProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/room.vert")) {
        perror("Missing Room Vertex Shader");
        close();
    }
    if(!mProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/room.frag")) {
        perror("Missing Room Fragment Shader");
        close();
    }
    if(!mProgram.link()) {
        perror("Room Shader linking Error");
        close();
    }
}


void
GLWidget::initTextures() {
    const QImage cubeImage = QImage(":/cube.png").mirrored();
    cubeTexture = new QOpenGLTexture(cubeImage);
    cubeTexture->setMinificationFilter(QOpenGLTexture::Nearest);
    cubeTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    cubeTexture->setWrapMode(QOpenGLTexture::Repeat);

    // Cubemap Textures MUST have the same size !!!
    const QImage posx = QImage(":/Pietra.jpg").convertToFormat(QImage::Format_RGBA8888);
    const QImage negx = QImage(":/Pietra.jpg").convertToFormat(QImage::Format_RGBA8888);
    const QImage posy = QImage(":/Pietra.jpg").convertToFormat(QImage::Format_RGBA8888);
    const QImage negy = QImage(":/Pavimento.jpg").convertToFormat(QImage::Format_RGBA8888);
    const QImage posz = QImage(":/Pietra.jpg").convertToFormat(QImage::Format_RGBA8888);
    const QImage negz = QImage(":/Pietra.jpg").convertToFormat(QImage::Format_RGBA8888);

    roomTexture = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);
    roomTexture->setSize(posx.width(), posx.height(), posx.depth());
    roomTexture->setFormat(QOpenGLTexture::RGBA8_UNorm);
    roomTexture->allocateStorage();

    roomTexture->setData(0, 0, QOpenGLTexture::CubeMapPositiveX,
                         QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                         posx.constBits(), Q_NULLPTR);
    roomTexture->setData(0, 0, QOpenGLTexture::CubeMapPositiveY,
                         QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                         posy.constBits(), Q_NULLPTR);
    roomTexture->setData(0, 0, QOpenGLTexture::CubeMapPositiveZ,
                         QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                         posz.constBits(), Q_NULLPTR);
    roomTexture->setData(0, 0, QOpenGLTexture::CubeMapNegativeX,
                         QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                         negx.constBits(), Q_NULLPTR);
    roomTexture->setData(0, 0, QOpenGLTexture::CubeMapNegativeY,
                         QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                         negy.constBits(), Q_NULLPTR);
    roomTexture->setData(0, 0, QOpenGLTexture::CubeMapNegativeZ,
                         QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                         negz.constBits(), Q_NULLPTR);

    roomTexture->setMinificationFilter(QOpenGLTexture::Nearest);
    roomTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    roomTexture->setWrapMode(QOpenGLTexture::Repeat);
}


void
GLWidget::resizeGL(int w, int h) {
    aspect = qreal(w) / qreal(h ? h : 1);
    projection.setToIdentity();
    projection.perspective(camera->FieldOfView(), aspect, zNear, zFar);
}


void
GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Camera matrix
    viewMatrix.setToIdentity();
    viewMatrix.lookAt(camera->Eye(), camera->Center(), camera->Up());
    // Room model Matrix
    model.setToIdentity();
    model.scale(5.0, 5.0, 5.0);
    model.translate(0.0, 1.0, 0.0);
    model.rotate(rotation);
    glDisable(GL_CULL_FACE);  // Disable back face culling

    mProgram.bind();
    mProgram.setUniformValue("mvp_matrix", projection*viewMatrix*model);

    roomTexture->bind();
    geometries->drawRoom(&mProgram);

    // Buggy Model matrix
    model.setToIdentity();
    model.translate(0.0, 1.0, 0.0);
    model.rotate(rotation);

    // Bind shader pipeline for use
    program.bind();
    program.setUniformValue("mvp_matrix", projection*viewMatrix*model);

    glEnable(GL_CULL_FACE);  // Enable back face culling
    cubeTexture->bind();
    geometries->drawCube(&program);
}


void
GLWidget::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::RightButton) {
        camera->MouseDown(event->x(), event->y());
        camera->MouseMode(CGrCamera::ROLLMOVE);
        event->accept();
    } else if (event->buttons() & Qt::LeftButton) {
        camera->MouseDown(event->x(), event->y());
        event->accept();
    }
}


void
GLWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() & Qt::RightButton) {
        camera->MouseMode(CGrCamera::PITCHYAW);
        event->accept();
    } else if (event->button() & Qt::LeftButton) {
        camera->MouseMode(CGrCamera::PITCHYAW);
        event->accept();
    }
    update();
}


void
GLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        camera->MouseMove(event->x(), event->y());
        event->accept();
    } else if (event->buttons() & Qt::RightButton) {
        camera->MouseMove(event->x(), event->y());
        event->accept();
    }
    update();
}


void
GLWidget::wheelEvent(QWheelEvent* event) {
    QPoint numDegrees = event->angleDelta();
    if (!numDegrees.isNull()) {
        camera->MouseDown(0, 0);
        camera->MouseMode(CGrCamera::ROLLMOVE);
        camera->MouseMove(0, -numDegrees.y());
        camera->MouseMode(CGrCamera::PITCHYAW);
        event->accept();
        update();
    }
}
