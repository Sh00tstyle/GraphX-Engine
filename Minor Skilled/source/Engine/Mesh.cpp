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

	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::_setupMesh() {
	//generate vertex array and buffer objects
	_VAO = new VertexArray();
	_VBO = new Buffer();
	_EBO = new Buffer();

	//buffer data
	_VAO->bind(); //bind, so it can store all configurations done from here

	_VBO->bind(GL_ARRAY_BUFFER);
	_VBO->bufferData(GL_ARRAY_BUFFER, &_vertices[0], _vertices.size() * sizeof(Vertex)); //fill the vertex buffer object with vertex data

	_EBO->bind(GL_ELEMENT_ARRAY_BUFFER);
	_EBO->bufferData(GL_ELEMENT_ARRAY_BUFFER, &_indices[0], _indices.size() * sizeof(unsigned int)); //fill the element bufer with index data

	//vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	//vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	//vertex uvs
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	//vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

	//vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

	glBindVertexArray(0); //unbind
}