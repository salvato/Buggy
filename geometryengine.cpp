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
    : sObjPath(QString())
    , cubeVertexBuf(QOpenGLBuffer::VertexBuffer)
    , cubeIndexBuf(QOpenGLBuffer::IndexBuffer)
{
    initializeOpenGLFunctions();
    // Generate needed VBOs
    cubeVertexBuf.create();
    cubeIndexBuf.create();

    glGenBuffers(1, &floorVertexBuf);
    // Initializes geometries and transfers them to VBOs
    if(!loadObj(sObjPath, vertices, uvs, normals)) {
      qDebug() << "Impossible to decode obj file" << sObjPath;
      exit(-1);
    }
    initBuggyGeometry();
    initCubeGeometry();
    initFloorGeometry();
}


GeometryEngine::~GeometryEngine() {
    cubeVertexBuf.destroy();
    cubeIndexBuf.destroy();
}


bool
GeometryEngine::loadObj(QString path,
                        QVector<QVector3D> &out_vertices,
                        QVector<QVector2D> &out_uvs,
                        QVector<QVector3D> &out_normals)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Impossible to open the file !" << path;
        return false;
    }
    QVector<unsigned int> vertexIndices, uvIndices, normalIndices;
    QVector<QVector3D> temp_vertices;
    QVector<QVector2D> temp_uvs;
    QVector<QVector3D> temp_normals;
    float x, y, z;

    QByteArray line;
    QString string;
    QStringList stringVals, stringTriples;

    while(!file.atEnd()) {
        line = file.readLine();
        // parse the line
        if(line.startsWith("vt")) {// is the texture coordinate of one vertex
            string = QString(line.mid(3));
            stringVals = string.split(" ");
            if(stringVals.size() != 2) {
                qDebug() << "File can't be read by our simple parser : (Try exporting with other options\n";
                return false;
            }
            x = stringVals.at(0).toFloat();
            y = stringVals.at(1).toFloat();
            temp_uvs.append(QVector2D(x, y));
        }
        else if(line.startsWith("vn")) {// is the normal of one vertex
            string = QString(line.mid(3));
            stringVals = string.split(" ");
            if(stringVals.size() != 3) {
                qDebug() << "File can't be read by our simple parser : (Try exporting with other options\n";
                return false;
            }
            x = stringVals.at(0).toFloat();
            y = stringVals.at(1).toFloat();
            z = stringVals.at(2).toFloat();
            temp_normals.append(QVector3D(x, y, z));
        }
        else if(line.startsWith("v")) {// Is a vertex
            string = QString(line.mid(2));
            stringVals = string.split(" ");
            if(stringVals.size() != 3) {
                qDebug() << "File can't be read by our simple parser : (Try exporting with other options\n";
                return false;
            }
            x = stringVals.at(0).toFloat();
            y = stringVals.at(1).toFloat();
            z = stringVals.at(2).toFloat();
            temp_vertices.append(QVector3D(x, y, z));
        }
        else if(line.startsWith("f")) {// is a face
            string = QString(line.mid(2));
            stringTriples = string.split(" ");
            if(stringTriples.size() != 3) {
                qDebug() << "File can't be read by our simple parser : (Try exporting with other options\n";
                return false;
            }
            for(int i=0; i<3; i++) {
                stringVals = stringTriples.at(i).split("/");
                if(stringVals.size() != 3) {
                    qDebug() << "File can't be read by our simple parser : (Try exporting with other options\n";
                    return false;
                }
                vertexIndices.append(stringVals.at(0).toFloat());
                if(stringVals.at(1) != "")
                    uvIndices    .append(stringVals.at(1).toFloat());
                if(stringVals.at(2) != "")
                    normalIndices.append(stringVals.at(2).toFloat());
            }
        }
        // else
        // Probably a comment skip the rest of the line
    }
    file.close();
    // For each vertex of each triangle
    for(int i=0; i<vertexIndices.size(); i++) {
        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int normalIndex = normalIndices[i];
        // Get the attributes thanks to the index
        QVector3D vertex = temp_vertices[ vertexIndex-1 ];
        QVector3D normal = temp_normals[ normalIndex-1 ];
        // Put the attributes in buffers
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
        vertexbuffer.create();
        vertexbuffer.bind();
        vertexbuffer.allocate(vertices.data(), vertices.size()*sizeof(QVector3D));
    }
    if(normals.size() > 0) { // Transfer normal data to VBO
        normalbuffer.create();
        normalbuffer.bind();
        normalbuffer.allocate(normals.data(), normals.size()*sizeof(QVector3D));
    }
    if(uvs.size() > 0) { // Transfer uv data to VBO
        uvbuffer.create();
        uvbuffer.bind();
        uvbuffer.allocate(uvs.data(), uvs.size()*sizeof(QVector2D));
    }
}



void
GeometryEngine::initFloorGeometry() {
    QVector3D vertices[] = {
        QVector3D(-1.0f,  0.0f, -1.0f),
        QVector3D( 1.0f,  0.0f,  1.0f),
        QVector3D( 1.0f,  0.0f, -1.0f),

        QVector3D( 1.0f,  0.0f,  1.0f),
        QVector3D(-1.0f,  0.0f, -1.0f),
        QVector3D(-1.0f,  0.0f,  1.0f)
    };
    glBindBuffer(GL_ARRAY_BUFFER, floorVertexBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}


void
GeometryEngine::initCubeGeometry() {
    // For cube we would need only 8 vertices but we have to
    // duplicate vertex for each face because texture coordinate
    // is different.
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

    // Indices for drawing cube faces using triangle strips.
    // Triangle strips can be connected by duplicating indices
    // between the strips. If connecting strips have opposite
    // vertex order then last index of the first strip and first
    // index of the second strip needs to be duplicated. If
    // connecting strips have same vertex order then only last
    // index of the first strip needs to be duplicated.
    GLushort indices[] = {
         0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
         4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
         8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
    };

    // Transfer vertex data to VBO 0
    cubeVertexBuf.bind();
    cubeVertexBuf.allocate(vertices, 24*sizeof(VertexData));

    // Transfer index data to VBO 1
    cubeIndexBuf.bind();
    cubeIndexBuf.allocate(indices, 34*sizeof(GLushort));
}


void
GeometryEngine::drawCube(QOpenGLShaderProgram *program) {
    // Tell OpenGL which VBOs to use
    cubeVertexBuf.bind();
    cubeIndexBuf.bind();

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
    cubeVertexBuf.bind();
    cubeIndexBuf.bind();

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
    glVertexAttribPointer(0,        // attribute 0.
                                    // No particular reason for 0, but must match
                                    // the layout in the shader.
                          3,        // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0,        // stride
                          (void*)0  // array buffer offset
    );
    glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 6 vertices -> 2 triangles
    glDisableVertexAttribArray(0);
}
