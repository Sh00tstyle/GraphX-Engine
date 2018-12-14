#include "Model.h"

#include <iostream>

#include "../Engine/Mesh.h"
#include "../Engine/Vertex.h"

Model::Model() {
}

Model::~Model() {
	for(unsigned int i = 0; i < _meshes.size(); i++) {
		delete _meshes[i];
	}
}

void Model::draw() {
	for(unsigned int i = 0; i < _meshes.size(); i++) {
		_meshes[i]->draw();
	}
}

Model* Model::LoadModel(std::string path) {
	Model* model = new Model();

	model->filepath = path;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		delete model;
		return nullptr;
	}

	_ProcessNode(scene->mRootNode, scene, model);

	return model;
}

void Model::_ProcessNode(aiNode* node, const aiScene* scene, Model* model) {
	//process all meshes of the node
	for(unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		model->_meshes.push_back(_ProcessMesh(mesh, scene));
	}

	//process all meshes of the children
	for(unsigned int i = 0; i < node->mNumChildren; i++) {
		_ProcessNode(node->mChildren[i], scene, model);
	}

}

Mesh* Model::_ProcessMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> newVertices;
	std::vector<unsigned int> newIndices;

	for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		glm::vec3 vector;

		//vertices
		if(mesh->mVertices != nullptr) {
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.position = vector;
		} else {
			vertex.position = glm::vec3(0.0f);
		}
		

		//normals
		if(mesh->mNormals != nullptr) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		} else {
			vertex.normal = glm::vec3(0.0f);
		}

		//uv
		if(mesh->mTextureCoords[0] != nullptr) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.uv = vec;
		} else {
			vertex.uv = glm::vec2(0.0f);
		}

		//tangents
		if(mesh->mTangents != nullptr) { //we only need to check tangent, cause bitangents exist automatically if tangents exist (check docs)
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.tangent = vector;

			//bitangents
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.bitangent = vector;
		} else {
			vertex.tangent = glm::vec3(0.0f);
			vertex.bitangent = glm::vec3(0.0f);
		}

		newVertices.push_back(vertex); //add vertex to the vector
	}

	//now walk through each of the mesh's faces and retrieve the corresponding vertex indices
	for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		//retrieve all indices of the face and store them in the indices vector
		for(unsigned int j = 0; j < face.mNumIndices; j++) newIndices.push_back(face.mIndices[j]);
	}

	return new Mesh(newVertices, newIndices);
}