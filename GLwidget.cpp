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
    , texture(nullptr)
{
}


GLWidget::~GLWidget() {
    makeCurrent();
    delete texture;
    delete geometries;
    doneCurrent();
}


QSize
GLWidget::minimumSizeHint() const {
    return QSize(50, 50);
}


QSize
GLWidget::sizeHint() const {
    return QSize(400, 400);
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
    glEnable(GL_CULL_FACE);  // Enable back face culling
    geometries = new GeometryEngine;
}


void
GLWidget::initShaders() {
    if(!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();
    if(!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();
    if(!program.link())
        close();
    if(!program.bind()) // Bind shader pipeline for use
        close();
}


void
GLWidget::initTextures() {
    texture = new QOpenGLTexture(QImage(":/cube.png").mirrored());
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setWrapMode(QOpenGLTexture::Repeat);
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
    texture->bind();

    // Camera matrix
    viewMatrix.setToIdentity();
    viewMatrix.lookAt(QVector3D(camera->EyeX(),    camera->EyeY(),    camera->EyeZ()),    // Camera position in World Space
                      QVector3D(camera->CenterX(), camera->CenterY(), camera->CenterZ()), // Looking at the origin
                      QVector3D(camera->UpX(),     camera->UpY(),     camera->UpZ())      // Head is up (set to 0,-1,0 to look upside-down)
                     );

    QMatrix4x4 model;
    model.setToIdentity();
    model.translate(0.0, 0.0, -5.0);
    model.rotate(rotation);
    program.setUniformValue("mvp_matrix", projection * viewMatrix * model);
    program.setUniformValue("texture", 0);
    geometries->drawCubeGeometry(&program);
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
    update();
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
