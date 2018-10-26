#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices):_vertices(vertices), _indices(indices) {
	_setupMesh();
}

Mesh::~Mesh() {
}

void Mesh::draw() {
	//TODO
}

void Mesh::_setupMesh() {
	//TODO
}