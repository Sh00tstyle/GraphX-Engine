#include <fstream>
#include <sstream>
#include <iostream>

#include <glad\glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"
#include "Shader.h"

Mesh::Mesh(std::vector<Vertex> pVertices, std::vector<unsigned int> pIndices, std::vector<Texture> pTextures) {
	vertices = pVertices;
	indices = pIndices;
	textures = pTextures;

	_setupMesh(); //initializes the mesh
}

void Mesh::draw(Shader * shader) {
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	for(unsigned int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i); //activate the correct texture unit

		std::string number;
		std::string name = textures[i].type;
		if(name == "texture_diffuse") number = std::to_string(diffuseNr++);
		else if(name == "texture_specular") number = std::to_string(specularNr++);
		else if(name == "texture_height") number = std::to_string(heightNr++);

		shader->setFloat((name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glActiveTexture(GL_TEXTURE0);

	//draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::_setupMesh() {
	//generate vertex array and buffer objects
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	//buffer data
	glBindVertexArray(VAO); //bind, so it can store all configurations done from here

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW); //fill the vertex buffer object with vertex data

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW); //fill the element bufer with index data

	//vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	//vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	//vertex uvs
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

	glBindVertexArray(0); //unbind
}
