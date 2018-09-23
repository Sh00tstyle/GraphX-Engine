#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <glm\glm.hpp>

class Shader;

struct Vertex {
	//vertex properties
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh {
	public:
		//mesh data
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		unsigned int VAO; //vertex array object, stores configurations done after binding it

		Mesh(std::vector<Vertex> pVertices, std::vector<unsigned int> pIndices, std::vector<Texture> pTextures);

		void draw(Shader* shader);
		void drawInstanced(Shader* shader, unsigned int amount);

	private:
		unsigned int VBO; //vertex buffer object, stores the vertex data
		unsigned int EBO; //element buffer object, stores indices for indexed drawing

		void _setupMesh();
};

#endif