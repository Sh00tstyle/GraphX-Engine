#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Shader;
class Mesh;
struct Texture;

class Model {
	public:
		Model(std::string filepath, bool gamma = false);

		void draw(Shader* shader);

	private:
		std::vector<Texture> texturesLoaded;
		std::vector<Mesh*> meshes;
		std::string directory;
		bool gammaCorrection;

		void _loadModel(std::string filepath);
		void _processNode(aiNode* node, const aiScene* scene);
		Mesh* _processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> _loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);

		unsigned int _textureFromFile(const char* filepath, const std::string &directory, bool gamma = false);
};

#endif