#ifndef TEXTUREMATERIAL_H
#define TEXTUREMATERIAL_H

#include "../Engine/Material.h"

class Texture;

class TextureMaterial : public Material {
public:
	TextureMaterial(Texture* diffuseMap, Texture* specularMap, Texture* normalMap);
	~TextureMaterial();

	Texture* getDiffuseMap();
	Texture* getSpecularMap();
	Texture* getNormalMap();
	void setDiffuseMap(Texture* diffuseMap);
	void setSpecularMap(Texture* specularMap);
	void setNormalMap(Texture* normalMap);

	virtual void draw();

private:
	Texture* _diffuseMap;
	Texture* _specularMap;
	Texture* _normalMap;

};

#endif