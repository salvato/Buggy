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

#include "dashboardwidget.h"
#include <QMouseEvent>
#include <math.h>


DashboardWidget::DashboardWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , QOpenGLFunctions()
    , zNear(0.1)
    , zFar(1300.0)
{ 
}


DashboardWidget::~DashboardWidget() {
    makeCurrent();
    doneCurrent();
}


QSize
DashboardWidget::minimumSizeHint() const {
    return QSize(60, 60);
}


QSize
DashboardWidget::sizeHint() const {
    return QSize(800, 800);
}


void
DashboardWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.1, 0.1, 0.3, 1);
    initShaders();
    initTextures();
    glDisable(GL_DEPTH_TEST); // Enable depth buffer
}


void
DashboardWidget::initShaders() {
    bool bResult = true;
    bResult &= floorProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/floor.vert");
    bResult &= floorProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/floor.frag");
    bResult &= floorProgram.link();
    if(!bResult) {
        perror("Unble to initShaders()...exiting");
        exit(EXIT_FAILURE);
    }
}


void
DashboardWidget::initTextures() {
    const QImage floorImage = QImage(":/Pavimento.jpg").mirrored();
    floorTexture = new QOpenGLTexture(floorImage);
    floorTexture->setMinificationFilter(QOpenGLTexture::Nearest);
    floorTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    floorTexture->setWrapMode(QOpenGLTexture::Repeat);
}


void
DashboardWidget::initFloorGeometry() {
    QVector3D vertices[] =
    {
        QVector3D(-0.5f, -0.5f, 0.0f),
        QVector3D( 0.5f,  0.5f, 0.0f),
        QVector3D( 0.5f, -0.5f, 0.0f),

        QVector3D( 0.5f,  0.5f, 0.0f),
        QVector3D(-0.5f, -0.5f, 0.0f),
        QVector3D(-0.5f,  0.5f, 0.0f)
    };
    glGenBuffers(1, &floorVertexBuf);
    glBindBuffer(GL_ARRAY_BUFFER, floorVertexBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}


void
DashboardWidget::resizeGL(int w, int h) {
    aspect = qreal(w) / qreal(h ? h : 1);
    orthoMatrix.setToIdentity();
    orthoMatrix.ortho(0, w, 0, h, zNear, zFar);
}


void
DashboardWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Floor
    modelMatrix.setToIdentity();
    modelMatrix.translate(width()/2, height()/2, height()/2);
    modelMatrix.scale(100.0, 100.0, 100.0);
    // Bind shader pipeline for use
    floorProgram.bind();
    floorProgram.setUniformValue("mvp_matrix", orthoMatrix*modelMatrix);
    glDisable(GL_CULL_FACE); // Disable back face culling
    floorTexture->bind();
    drawFloor(&floorProgram);
}


void
DashboardWidget::drawFloor(QOpenGLShaderProgram *program) {
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer("a_position", GL_FLOAT, 0, 3, sizeof(float));
    program->bind();
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, floorVertexBuf);
    glVertexAttribPointer(0,        // attribute: Must match the layout in the shader.
                          3,        // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          3*sizeof(float),        // stride
                          nullptr   // array buffer offset
    );
    glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 6 vertices -> 2 triangles
    glDisableVertexAttribArray(0);
}
