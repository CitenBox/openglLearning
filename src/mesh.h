#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

#define MAX_BONE_INFLUENCE 4

using namespace std;

struct Vertex{
    glm::vec3 position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture{
    unsigned id;
    string type;
    string path;
};

class Shader;
class Mesh {
    public:

    vector<Vertex> vertices;
    vector<unsigned> indices;
    vector<Texture> textures;

    unsigned VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned> indices, vector<Texture> textures);

    void draw(Shader& shader);

    private:

    unsigned VBO, EBO;

    void setupMesh();
    
};
