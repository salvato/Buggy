#include <model.h>

#include <QVector3D>
#include <QVector2D>
#include <QImage>


unsigned int
TextureFromFile(const QString &path, const QString &directory, bool gamma) {
    Q_UNUSED(gamma)

    QString filename = QString(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    const QImage textureImage = QImage(filename).mirrored().convertToFormat(QImage::Format_ARGB32);
    if((textureImage.width() > 0) && (textureImage.height() > 0)) {
        GLenum format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     format,
                     textureImage.width(),
                     textureImage.height(),
                     0,
                     format,
                     GL_UNSIGNED_BYTE,
                     textureImage.bits());
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        qDebug() << "Texture failed to load at path: " << path;
    }

    return textureID;
}


Model::Model(QString const &path, bool gamma)
    : QOpenGLExtraFunctions()
    , gammaCorrection(gamma)
{
    initializeOpenGLFunctions();
    loadModel(path);
}


void
Model::Draw(QOpenGLShaderProgram* shader) {
    for(int i=0; i<meshes.size(); i++)
        meshes[i].Draw(shader);
}


void
Model::loadModel(QString const &path) {
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.toLatin1(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {// if is Not Zero
        qDebug() << "ERROR::ASSIMP:: " << importer.GetErrorString();
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.mid(0, path.lastIndexOf('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
}


void
Model::processNode(aiNode *node, const aiScene *scene) {
    // process each mesh located at the current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        // the node object only contains indices to index
        // the actual objects in the scene.
        // the scene contains all the data,
        // node is just to keep stuff organized.
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any)
    // we then recursively process each of the children nodes
    for(unsigned int i=0; i<node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh
Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    QVector<Vertex>       vertices;
    QVector<unsigned int> indices;
    QVector<Texture>      textures;

    // walk through each of the mesh's vertices
    for(unsigned int i=0; i<mesh->mNumVertices; i++) {
        Vertex vertex;
        QVector3D vector;

        vector.setX(mesh->mVertices[i].x);
        vector.setY(mesh->mVertices[i].y);
        vector.setZ(mesh->mVertices[i].z);
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals()) {
            vector.setX(mesh->mNormals[i].x);
            vector.setY(mesh->mNormals[i].y);
            vector.setZ(mesh->mNormals[i].z);
            vertex.Normal = vector;
        }
        // texture coordinates
        if(mesh->mTextureCoords[0]) {// does the mesh contain texture coordinates?
            QVector2D vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.setX(mesh->mTextureCoords[0][i].x);
            vec.setY(mesh->mTextureCoords[0][i].y);
            vertex.TexCoords = vec;
            // tangent
            vector.setX(mesh->mTangents[i].x);
            vector.setY(mesh->mTangents[i].y);
            vector.setZ(mesh->mTangents[i].z);
            vertex.Tangent = vector;
            // bitangent
            vector.setX(mesh->mBitangents[i].x);
            vector.setY(mesh->mBitangents[i].y);
            vector.setZ(mesh->mBitangents[i].z);
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = QVector2D(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now walk through each of the mesh's faces
    // and retrieve the corresponding vertex indices.
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders.
    // Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging
    // from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    QVector<Texture> diffuseMaps = loadMaterialTextures(material,
                                                        aiTextureType_DIFFUSE,
                                                        "texture_diffuse");
    textures.append(diffuseMaps);

    // 2. specular maps
    QVector<Texture> specularMaps = loadMaterialTextures(material,
                                                         aiTextureType_SPECULAR,
                                                         "texture_specular");

    textures.append(specularMaps);

    QVector<Texture> normalMaps = loadMaterialTextures(material,
                                                       aiTextureType_HEIGHT,
                                                       "texture_normal");
    textures.append(normalMaps);

    QVector<Texture> heightMaps = loadMaterialTextures(material,
                                                       aiTextureType_AMBIENT,
                                                       "texture_height");
    textures.append(heightMaps);

    return Mesh(vertices, indices, textures);
}


// checks all material textures of a given type
// and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
QVector<Texture>
Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, QString typeName) {
    QVector<Texture> textures;
    for(unsigned int i=0; i<mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue
        // to next iteration: skip loading a new texture
        bool skip = false;
        for(int j=0; j<textures_loaded.size(); j++) {
            if(QString(str.C_Str()) == textures_loaded[j].path) {
                textures.push_back(textures_loaded[j]);
                skip = true; // skip textures with the same filepath already loaded
                break;
            }
        }
        if(!skip) {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id   = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.append(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

