#pragma once

#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QVector>
#include <QVector2D>
#include <QVector3D>


struct Vertex {
    QVector3D Position;
    QVector3D Normal;
    QVector2D TexCoords;
    QVector3D Tangent;
    QVector3D Bitangent;
};


struct Texture {
    unsigned int id;
    QString type;
    QString path;
    Texture(unsigned int _id, QString _type, QString _path) {
        id = _id;
        type = _type;
        path = _path;
    }
    Texture() {
        id = -1;
        type = QString();
        path = QString();
    }
};


class Mesh : protected QOpenGLExtraFunctions {

public:
    Mesh(QVector<Vertex> vertices,
         QVector<unsigned int> indices,
         QVector<Texture> textures);
    void Draw(QOpenGLShaderProgram* shader);

public:
    // mesh Data
    QVector<Vertex>       vertices;
    QVector<unsigned int> indices;
    QVector<Texture>      textures;
    unsigned int VAO;

private:
    void setupMesh();

private:
    unsigned int VBO, EBO;
};
