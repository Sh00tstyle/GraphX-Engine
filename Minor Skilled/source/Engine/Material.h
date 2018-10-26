#ifndef MATERIAL_H
#define MATERIAL_H

class Shader;

class Material {
public:
	~Material();

	virtual void draw() = 0;

protected:
	Material();

	static Shader* _shader;

};

#endif