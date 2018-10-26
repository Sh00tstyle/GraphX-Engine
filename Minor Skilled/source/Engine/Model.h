#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

class Mesh;

class Model {
	public:
		Model(std::string path);
		~Model();

		void draw();

	private:
		std::vector<Mesh*> _meshes;

		void _loadModel(std::string path);

};

#endif