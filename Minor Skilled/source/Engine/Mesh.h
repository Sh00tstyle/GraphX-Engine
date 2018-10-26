#ifndef MESH_H
#define MESH_H

#include <vector>

#include "../Engine/Vertex.h"

class Mesh {
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	~Mesh();

	void draw();

private:
	std::vector<Vertex> _vertices;
	std::vector<unsigned int> _indices;

	unsigned int _VAO;
	unsigned int _VBO;
	unsigned int _EBO;

	void _setupMesh();

};

#endif