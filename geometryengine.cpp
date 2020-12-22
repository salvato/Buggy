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

#include "geometryengine.h"

#include <QFile>
#include <QVector2D>
#include <QVector3D>

struct
VertexData {
    QVector3D position;
    QVector2D texCoord;
};


GeometryEngine::GeometryEngine()
    : sObjPath(QString("../Buggy/Car/Car3.obj"))
{
    // Initializes OpenGL function resolution for the current context
    initializeOpenGLFunctions();

    // Generate needed VBOs
    glGenBuffers(1, &cubeVertexBuf);
    glGenBuffers(1, &cubeIndexBuf);
    glGenBuffers(1, &floorVertexBuf);
    glGenBuffers(1, &buggyVertexBuf);
    glGenBuffers(1, &buggyUvBuf);
    glGenBuffers(1, &buggyNormalBuf);

    pCar = new Model(sObjPath);

//    // Initializes geometries and transfers them to VBOs
//    if(loadObj(sObjPath, vertices, uvs, normals)) {
//        qDebug() << "Car3.obj Correctly loaded";
//        initBuggyGeometry();
//    }
    initCubeGeometry();
    initFloorGeometry();
}


GeometryEngine::~GeometryEngine() {
    glDeleteBuffers(1, &cubeVertexBuf);
    glDeleteBuffers(1, &cubeIndexBuf);
    glDeleteBuffers(1, &floorVertexBuf);
    glDeleteBuffers(1, &buggyVertexBuf);
    glDeleteBuffers(1, &buggyUvBuf);
    glDeleteBuffers(1, &buggyNormalBuf);
}


bool
GeometryEngine::loadObj(QString path,
                        QVector<QVector3D> &out_vertices,
                        QVector<QVector2D> &out_uvs,
                        QVector<QVector3D> &out_normals)
{
    QFile file(path);
    if(!file.exists()) {
        qDebug() << "File" << file.fileName() << "Not Found";
        return false;
    }
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Unable to open file:" << path;
        return false;
    }
    QVector<unsigned int> vertexIndices, uvIndices, normalIndices;
    QVector<QVector3D> temp_vertices;
    QVector<QVector2D> temp_uvs;
    QVector<QVector3D> temp_normals;
    float x, y, z;
    float xMin=__FLT_MAX__, xMax=-__FLT_MAX__;
    float yMin=__FLT_MAX__, yMax=-__FLT_MAX__;
    float zMin=__FLT_MAX__, zMax=-__FLT_MAX__;

    QString line;
    QStringList stringVals, stringTriples;

    while(!file.atEnd()) {
        line = QString(file.readLine()).remove('\r').remove('\n');
        // parse the line
        if(line.startsWith("vt")) {// is the texture coordinate of one vertex
            stringVals = line.split(" ", Qt::SkipEmptyParts);
            if(stringVals.size() < 3) {
                qDebug() << "vt: File can't be read by our simple parser: Try exporting with other options";
                return false;
            }
            x = stringVals.at(1).toFloat();
            y = stringVals.at(2).toFloat();
            temp_uvs.append(QVector2D(x, y));
        }
        else if(line.startsWith("vn")) {// is the normal of one vertex
            stringVals = QString(line).split(" ", Qt::SkipEmptyParts);
            if(stringVals.size() != 4) {
                qDebug() << "vn: File can't be read by our simple parser: Try exporting with other options";
                return false;
            }
            x = stringVals.at(1).toFloat();
            y = stringVals.at(2).toFloat();
            z = stringVals.at(3).toFloat();
            temp_normals.append(QVector3D(x, y, z));
        }
        else if(line.startsWith("v")) {// Is a vertex
            stringVals = QString(line).split(" ", Qt::SkipEmptyParts);
            if(stringVals.size() != 4) {
                qDebug() << "v: File can't be read by our simple parser: Try exporting with other options";
                return false;
            }
            x = stringVals.at(1).toFloat();
            y = stringVals.at(2).toFloat();
            z = stringVals.at(3).toFloat();
            temp_vertices.append(QVector3D(x, y, z));
            if(x<xMin) xMin = x;
            if(y<yMin) yMin = y;
            if(z<zMin) zMin = z;
            if(x>xMax) xMax = x;
            if(y>yMax) yMax = y;
            if(z>zMax) zMax = z;
        }
        else if(line.startsWith("f")) {// is a face
            stringTriples = QString(line).split(" ", Qt::SkipEmptyParts);
            if(stringTriples.size() < 4) {
                qDebug() << "f File can't be read by our simple parser: Try exporting with other options";
                return false;
            }
            for(int i=1; i<4; i++) {
                stringVals = stringTriples.at(i).split("/");
                if(stringVals.size() != 3) {
                    qDebug() << "f2: File can't be read by our simple parser: Try exporting with other options";
                    return false;
                }
                vertexIndices.append(stringVals.at(0).toFloat());
                if(stringVals.at(1) != "")
                    uvIndices.append(stringVals.at(1).toFloat());
                if(stringVals.at(2) != "")
                    normalIndices.append(stringVals.at(2).toFloat());
            }
        }
         else // Probably a comment skip the rest of the line
            qDebug() << line;
    }
    file.close();
    //qDebug() << xMax << xMin << yMax << yMin << zMax << zMin;
    float dx = xMax-xMin;
    float dy = yMax-yMin;
    float dz = zMax-zMin;
    for(int i=0; i<temp_vertices.size(); i++) {
        temp_vertices[i].setX(((temp_vertices[i].x()-xMin)/dx)-0.5);
        temp_vertices[i].setY((temp_vertices[i].y()-yMin)/dy);
        temp_vertices[i].setZ(((temp_vertices[i].z()-zMin)/dz)-0.5);
    }
    // For each vertex of each triangle
    for(int i=0; i<vertexIndices.size(); i++) {
        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int normalIndex = normalIndices[i];
        // Get the attributes thanks to the index
        QVector3D vertex = temp_vertices[vertexIndex-1];
        QVector3D normal = temp_normals[normalIndex-1];
        out_vertices.append(vertex);
        out_normals .append(normal);
    }
    if(!temp_uvs.isEmpty()) {
        // For each vertex of each triangle
        for(int i=0; i<vertexIndices.size(); i++) {
            // Get the indices of its attributes
            unsigned int uvIndex = uvIndices[i];
            // Get the attributes thanks to the index
            QVector2D uv = temp_uvs[ uvIndex-1 ];
            // Put the attributes in buffers
            out_uvs.append(uv);
        }
    }
    return true;
}


void
GeometryEngine::initBuggyGeometry() {
    if(vertices.size() > 0) { // Transfer vertex data to VBO
        glBindBuffer(GL_ARRAY_BUFFER, buggyVertexBuf);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(QVector3D), vertices.data(), GL_STATIC_DRAW);
        if(normals.size() > 0) { // Transfer normal data to VBO
            glBindBuffer(GL_ARRAY_BUFFER, buggyNormalBuf);
            glBufferData(GL_ARRAY_BUFFER,  normals.size()*sizeof(QVector3D), normals.data(), GL_STATIC_DRAW);
        }
        if(uvs.size() > 0) { // Transfer uv data to VBO
            glBindBuffer(GL_TEXTURE_BUFFER, buggyUvBuf);
            glBufferData(GL_TEXTURE_BUFFER, uvs.size()*sizeof(QVector2D), uvs.data(), GL_STATIC_DRAW);
        }
    }
}


void
GeometryEngine::initFloorGeometry() {
    QVector3D vertices[] = {
        QVector3D(-8.0f,  0.0f, -8.0f),
        QVector3D( 8.0f,  0.0f,  8.0f),
        QVector3D( 8.0f,  0.0f, -8.0f),

        QVector3D( 8.0f,  0.0f,  8.0f),
        QVector3D(-8.0f,  0.0f, -8.0f),
        QVector3D(-8.0f,  0.0f,  8.0f)
    };
    glBindBuffer(GL_ARRAY_BUFFER, floorVertexBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}


void
GeometryEngine::initCubeGeometry() {
    VertexData vertices[] = {
        // Vertex data for face 0
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.0f,  0.0f)}, // v0
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.0f)}, // v1
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.0f,  0.5f)}, // v2
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v3

        // Vertex data for face 1
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D( 0.0f, 0.5f)}, // v4
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.33f, 0.5f)}, // v5
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.0f,  1.0f)}, // v6
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.33f, 1.0f)}, // v7

        // Vertex data for face 2
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.5f)}, // v8
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(1.0f,  0.5f)}, // v9
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.66f, 1.0f)}, // v10
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(1.0f,  1.0f)}, // v11

        // Vertex data for face 3
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.0f)}, // v12
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(1.0f,  0.0f)}, // v13
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.66f, 0.5f)}, // v14
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(1.0f,  0.5f)}, // v15

        // Vertex data for face 4
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.33f, 0.0f)}, // v16
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.0f)}, // v17
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v18
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.66f, 0.5f)}, // v19

        // Vertex data for face 5
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v20
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.66f, 0.5f)}, // v21
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.33f, 1.0f)}, // v22
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.66f, 1.0f)}  // v23
    };
    GLushort indices[] = {
         0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
         4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
         8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
    };
    glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuf);
    glBufferData(GL_ARRAY_BUFFER, 24*sizeof(VertexData), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndexBuf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 34*sizeof(GLushort), indices, GL_STATIC_DRAW);
}


void
GeometryEngine::drawBuggy(QOpenGLShaderProgram *program) {
/*
    glBindBuffer(GL_ARRAY_BUFFER, buggyVertexBuf);
    int vertexLocation = program->attributeLocation("qt_Vertex");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    if(uvs.size() > 0) {
        glBindBuffer(GL_TEXTURE_BUFFER, buggyUvBuf);
        int texcoordLocation = program->attributeLocation("qt_MultiTexCoord0");
        program->enableAttributeArray(texcoordLocation);
        program->setAttributeBuffer(texcoordLocation, GL_FLOAT, 0, 2, sizeof(QVector2D));
    }

    if(normals.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, buggyNormalBuf);
        int normcoordLocation = program->attributeLocation("vertexNormal_modelspace");
        program->enableAttributeArray(normcoordLocation);
        program->setAttributeBuffer(normcoordLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));
    }
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
*/
    pCar->Draw(program);
}


void
GeometryEngine::drawCube(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndexBuf);

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
}


void
GeometryEngine::drawRoom(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndexBuf);

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer("a_position", GL_FLOAT, 0, 3, sizeof(VertexData));

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
}


void
GeometryEngine::drawFloor(QOpenGLShaderProgram *program) {
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer("a_position", GL_FLOAT, 0, 3, sizeof(VertexData));
    program->bind();
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, floorVertexBuf);
    glVertexAttribPointer(0,        // attribute: Must match the layout in the shader.
                          3,        // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0,        // stride
                          nullptr   // array buffer offset
    );
    glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 6 vertices -> 2 triangles
    glDisableVertexAttribArray(0);
}
