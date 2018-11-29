#ifndef MESH_H
#define MESH_H

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Engine/Vertex.h"

class VertexArray;
class Buffer;

class Mesh {
	public:
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
		~Mesh();

		void draw();

	private:
		std::vector<Vertex> _vertices;
		std::vector<unsigned int> _indices;

		VertexArray* _VAO;
		Buffer* _VBO;
		Buffer* _EBO;

		void _setupMesh();

};

#endif