#include "Mesh.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Engine/VertexArray.h"
#include "../Engine/Buffer.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices):_vertices(vertices), _indices(indices) {
	_setupMesh();
}

Mesh::~Mesh() {
	delete _VAO;
	delete _VBO;
	delete _EBO;
}

void Mesh::draw() {
	_VAO->bind();

	_VAO->drawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
	VertexArray::Unbind();
}

void Mesh::_setupMesh() {
	//generate vertex array and buffer objects
	_VAO = new VertexArray();
	_VBO = new Buffer(GL_ARRAY_BUFFER);
	_EBO = new Buffer(GL_ELEMENT_ARRAY_BUFFER);

	//buffer data
	_VAO->bind(); //bind, so it can store all configurations done from here

	_VBO->bind();
	_VBO->bufferData(&_vertices[0], _vertices.size() * sizeof(Vertex)); //fill the vertex buffer object with vertex data

	_EBO->bind();
	_EBO->bufferData(&_indices[0], _indices.size() * sizeof(unsigned int)); //fill the element bufer with index data

	//vertex positions
	_VAO->setAttribute(0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);

	//vertex normals
	_VAO->setAttribute(1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	//vertex uvs
	_VAO->setAttribute(2, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	//vertex tangent
	_VAO->setAttribute(3, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

	//vertex bitangent
	_VAO->setAttribute(4, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

	VertexArray::Unbind(); //unbind
}