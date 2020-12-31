#include "car.h"

#include <QFile>
#include <QtMath>
#include <QMatrix4x4>


// 12 CPR Quadrature Encoder
// Motor Gear Ratio 1:9
// Quadrature encoder mode 3 (x4 mode)


Car::Car()
{
    sObjPath = QString("../Buggy/Car/Car3.obj");
    // Initializes OpenGL function resolution for the current context
    initializeOpenGLFunctions();

    pModel = new Model(sObjPath);

    initGeometry();
    initShaders();

    wheelDiameter         = 0.69; // in dm
    wheelsDistance        = 2.0;  // in dm
    wheelToCenterDistance = 0.5*wheelsDistance; // in dm
    pulsesPerRevolution   = 12*4*9;
    StartingPosition      = QVector3D(0.0, 0.0, 0.0);
    startingAngle         = 0.0;
    Reset(0, 0);
}


Car::~Car() {
    glDeleteBuffers(1, &cubeVertexBuf);
    glDeleteBuffers(1, &cubeIndexBuf);
    glDeleteBuffers(1, &floorVertexBuf);
    glDeleteBuffers(1, &buggyVertexBuf);
    glDeleteBuffers(1, &buggyUvBuf);
    glDeleteBuffers(1, &buggyNormalBuf);
    glDeleteBuffers(1, &cubeTexture);
}


void
Car::initGeometry() {
    // init Buggy Geometry
    // Generate needed VBOs
    glGenBuffers(1, &buggyVertexBuf);
    glGenBuffers(1, &buggyUvBuf);
    glGenBuffers(1, &buggyNormalBuf);
    // Initializes geometries and transfers them to VBOs
//    if(loadObj(sObjPath, vertices, uvs, normals)) {
//        qDebug() << "Car3.obj Correctly loaded";
//    }
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

    // init Cube Geometry() {
    // Generate needed VBOs
    glGenBuffers(1, &cubeVertexBuf);
    glGenBuffers(1, &cubeIndexBuf);
    glGenBuffers(1, &floorVertexBuf);
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
    initTextures();
}


void
Car::initTextures() {
    const QImage cubeImage = QImage(":/cube.png").mirrored();
    glGenTextures(1, &cubeTexture);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 cubeImage.width(),
                 cubeImage.height(),
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 (void*)cubeImage.bits()
                );
    glGenerateMipmap(GL_TEXTURE_2D);
}


void
Car::initShaders() {
    bool bResult;
    bResult  = buggyProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/buggy.vert");
    bResult &= buggyProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/buggy.frag");
    bResult &= buggyProgram.link();

    bResult &= cubeProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/cube.vert");
    bResult &= cubeProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/cube.frag");
    bResult &= cubeProgram.link();
    if(!bResult) {
        perror("Unble to init Car Shaders()...exiting");
        exit(EXIT_FAILURE);
    }
}


bool
Car::loadObj() {
    QFile file(sObjPath);
    if(!file.exists()) {
        qDebug() << "File" << file.fileName() << "Not Found";
        return false;
    }
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Unable to open file:" << sObjPath;
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
        vertices.append(vertex);
        normals .append(normal);
    }
    if(!temp_uvs.isEmpty()) {
        // For each vertex of each triangle
        for(int i=0; i<vertexIndices.size(); i++) {
            // Get the indices of its attributes
            unsigned int uvIndex = uvIndices[i];
            // Get the attributes thanks to the index
            QVector2D uv = temp_uvs[ uvIndex-1 ];
            // Put the attributes in buffers
            uvs.append(uv);
        }
    }
    return true;
}


void
Car::draw(const QMatrix4x4 projectionMatrix, const QMatrix4x4 viewMatrix) {
    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    modelMatrix.translate(Position+QVector3D(0.0, 1.0, 0.0));
    modelMatrix.rotate(QQuaternion::fromAxisAndAngle(QVector3D(0.0, 1.0, 0.0), qRadiansToDegrees(carAngle)));
    cubeProgram.bind();
    cubeProgram.setUniformValue("projection_matrix", projectionMatrix);
    cubeProgram.setUniformValue("view_matrix", viewMatrix);
    cubeProgram.setUniformValue("model_matrix", modelMatrix);
    glEnable(GL_CULL_FACE); // Enable back face culling

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
    pModel->Draw(program);
*/

    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuf); // Tell OpenGL which VBOs to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndexBuf);

    quintptr offset = 0; // Offset for position
    int vertexLocation = cubeProgram.attributeLocation("vertexPosition");
    cubeProgram.enableAttributeArray(vertexLocation);
    cubeProgram.setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D); // Offset for texture coordinate
    int texcoordLocation = cubeProgram.attributeLocation("textureCoord");
    cubeProgram.enableAttributeArray(texcoordLocation);
    cubeProgram.setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
}



double
Car::FromPulsesToPath(const int pulses) {
    double path = (double(pulses)/double(pulsesPerRevolution))*M_PI*wheelDiameter;
    return path;
}


double
Car::FromPathToAngle(const double path) {
    double angle = fmod(path/wheelToCenterDistance, 2.0*M_PI);
    return angle;
}


void
Car::Move(const int rightPulses, const int leftPulses) {
    QMatrix4x4 transform;
    double rightAngle = FromPathToAngle(FromPulsesToPath(rightPulses-lastRPulses));
    double leftAngle  = FromPathToAngle(FromPulsesToPath(leftPulses -lastLPulses));

    lastRPulses = rightPulses;
    lastLPulses = leftPulses;

    // First Let Only the Right Wheel to Move
    xL = Position.x() - wheelToCenterDistance * cos(carAngle);
    zL = Position.z() + wheelToCenterDistance * sin(carAngle);

    transform.setToIdentity();

    transform.translate(xL, 0.0, zL);
    transform.rotate(qRadiansToDegrees(rightAngle), QVector3D(0.0, 1.0, 0.0));
    transform.translate(-xL, 0.0, -zL);
    Position = transform*Position;
    carAngle += rightAngle;

    // Then Move only the Left Wheel
    xR = Position.x() + wheelToCenterDistance * cos(carAngle);
    zR = Position.z() - wheelToCenterDistance * sin(carAngle);

    transform.setToIdentity();

    transform.translate(xR, 0.0, zR);
    transform.rotate(qRadiansToDegrees(-leftAngle), QVector3D(0.0, 1.0, 0.0));
    transform.translate(-xR, 0.0, -zR);
    Position = transform*Position;
    carAngle -= leftAngle;
    carAngle = fmod(carAngle, 2.0*M_PI);
/*
    qDebug()
             << "Angle =" << qRadiansToDegrees(carAngle)
             << "xL=" << xL
             << "zL=" << zL
             << "xR=" << xR
             << "zR=" << zR
             << "X:" << Position.x()
             << "Z:" << Position.z();
*/
}


void
Car::Reset(const int rightPulses, const int leftPulses) {
    lastRPulses = rightPulses;
    lastLPulses = leftPulses;

    Position = StartingPosition;
    carAngle = startingAngle;
}


void
Car::Reset() {
    Position = StartingPosition;
    carAngle = startingAngle;
}


void
Car::Reset(const QVector3D initialPosition, const double degrees) {
    StartingPosition = initialPosition;
    Position = StartingPosition;
    carAngle = qDegreesToRadians(degrees);
    startingAngle = carAngle;
}


void
Car::SetPosition(const QVector3D initialPosition) {
    StartingPosition = initialPosition;
    Position = StartingPosition;
}


void
Car::SetAngle(const double degrees) {
    carAngle = qDegreesToRadians(degrees);
    startingAngle = carAngle;
}


QVector3D Car::GetPosition() {
    return Position;
}


QQuaternion
Car::GetRotation() {
    return QQuaternion::fromAxisAndAngle(QVector3D(0.0, 1.0, 0.0), qRadiansToDegrees(carAngle));
}
