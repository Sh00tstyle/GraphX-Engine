#include "Model.h"

#include "../Engine/Mesh.h"

Model::Model(std::string path) {
	_loadModel(path);
}

Model::~Model() {
	for(unsigned int i = 0; i < _meshes.size(); i++) {
		delete _meshes[i];
	}
}

void Model::draw() {
}

void Model::_loadModel(std::string path) {
}
