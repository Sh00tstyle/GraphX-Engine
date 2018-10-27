#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Mesh;

class Model {
	public:
		~Model();

		static Model* LoadModel(std::string path);

		void draw();

	private:
		Model();
		
		std::vector<Mesh*> _meshes;

		static void _ProcessNode(aiNode* node, const aiScene* scene, Model* model);
		static Mesh* _ProcessMesh(aiMesh* mesh, const aiScene* scene);
};

#endif