#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <mesh.h>

#include <QString>
#include <QVector>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>


unsigned int
TextureFromFile(const QString &path, const QString &directory, bool gamma=false);


class Model : protected QOpenGLExtraFunctions
{
public:
    Model(QString const &path, bool gamma = false);
    void Draw(QOpenGLShaderProgram* shader);

public:
    QVector<Texture> textures_loaded;
    QVector<Mesh>    meshes;
    QString          directory;
    bool             gammaCorrection;

private:
    void loadModel(QString const &path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    QVector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, QString typeName);
};
