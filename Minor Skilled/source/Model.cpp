#include "Model.h"
#include "Mesh.h"
#include "Shader.h"

#include "stb_image.h"

Model::Model(std::string filepath, bool gamma) : gammaCorrection(gamma) {
	_loadModel(filepath); //load the model
}

void Model::draw(Shader * shader) {
	for(unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i]->draw(shader);
	}
}

void Model::_loadModel(std::string filepath) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	directory = filepath.substr(0, filepath.find_last_of('/'));

	_processNode(scene->mRootNode, scene);
}

void Model::_processNode(aiNode * node, const aiScene * scene) {
	//process all meshes of the node
	for(unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(_processMesh(mesh, scene));
	}

	//process all meshes of the children
	for(unsigned int i = 0; i < node->mNumChildren; i++) {
		_processNode(node->mChildren[i], scene);
	}
}

Mesh* Model::_processMesh(aiMesh * mesh, const aiScene * scene) {
	std::vector<Vertex> newVertices;
	std::vector<unsigned int> newIndices;
	std::vector<Texture> newTextures;

	for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		glm::vec3 vector;

		//positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		//normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;

		//uv
		if(mesh->mTextureCoords[0]) //does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.uv = vec;
		} else
			vertex.uv = glm::vec2(0.0f, 0.0f);

		//tangent
		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		vertex.tangent = vector;

		//bitangent
		vector.x = mesh->mBitangents[i].x;
		vector.y = mesh->mBitangents[i].y;
		vector.z = mesh->mBitangents[i].z;
		vertex.bitangent = vector;

		newVertices.push_back(vertex); //add vertex to the vector
	}

	//now walk through each of the mesh's faces and retrieve the corresponding vertex indices
	for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		//retrieve all indices of the face and store them in the indices vector
		for(unsigned int j = 0; j < face.mNumIndices; j++) newIndices.push_back(face.mIndices[j]);
	}

	// process materials
	if(mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		//diffuse maps
		std::vector<Texture> diffuseMaps = _loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		newTextures.insert(newTextures.end(), diffuseMaps.begin(), diffuseMaps.end());

		//specular maps
		std::vector<Texture> specularMaps = _loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		newTextures.insert(newTextures.end(), specularMaps.begin(), specularMaps.end());

		//normal maps
		std::vector<Texture> normalMaps = _loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		newTextures.insert(newTextures.end(), normalMaps.begin(), normalMaps.end());

		//height maps
		std::vector<Texture> heightMaps = _loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		newTextures.insert(newTextures.end(), heightMaps.begin(), heightMaps.end());
	}

	return new Mesh(newVertices, newIndices, newTextures);
}

std::vector<Texture> Model::_loadMaterialTextures(aiMaterial * material, aiTextureType type, std::string typeName) {
	std::vector<Texture> textures;

	for(unsigned int i = 0; i < material->GetTextureCount(type); i++) {
		aiString str;
		material->GetTexture(type, i, &str);

		bool skip = false;

		//check if the texture has been loaded already
		for(unsigned int j = 0; j < texturesLoaded.size(); j++) {
			if(std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0) { //???
				textures.push_back(texturesLoaded[j]);
				skip = true;
				break;
			}
		}

		//load texture if it hasnt been already
		if(!skip) {
			Texture texture;
			texture.id = _textureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();

			textures.push_back(texture);
			texturesLoaded.push_back(texture);
		}
	}

	return textures;
}

unsigned int Model::_textureFromFile(const char * filepath, const std::string & directory, bool gamma) {
	std::string filename = std::string(filepath);
	filename = directory + '/' + filename;

	//create opengl texture object
	unsigned int textureID;
	glGenTextures(1, &textureID);

	//load texture from file
	int width, height, nrComponents;
	unsigned char* textureData = stbi_load(filepath, &width, &height, &nrComponents, 0);

	if(textureData) {
		//identify format
		GLenum format;
		if(nrComponents == 1) format = GL_RED;
		else if(nrComponents == 3) format = GL_RGB;
		else if(nrComponents == 4) format = GL_RGBA;

		//load texture into opengl
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);

		//set texture filter options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(textureData); //free memory

	} else {
		std::cout << "Texture failed to load at path: " << filepath << std::endl;
		stbi_image_free(textureData); //free memory
	}

	return textureID; //texture id
}