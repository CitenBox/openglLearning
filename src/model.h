#pragma once

#include <vector>
#include <string>
#include "shader.h"
#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;

class Model 
{
    public:
    vector<Texture> texturesLoaded;
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;

    Model(char *path, bool gammaCorrection = false);
    void draw(Shader &shader);	

    void loadModel(string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
    unsigned textureFromFile(const char *path, const string &directory, bool gamma = false);
};