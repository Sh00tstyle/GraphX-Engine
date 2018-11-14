#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> //include glad to get all the required OpenGL headers
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
	public:
		Shader(std::string vertexPath, std::string fragmentPath);
		Shader(std::string vertexPath, std::string geometryPath, std::string fragmentPath); //overloaded constructor for geometry shader support

		void use();

		void setBool(std::string name, bool value);
		void setInt(std::string name, int value);
		void setFloat(std::string name, float value);
		void setVec2(std::string name, float x, float y);
		void setVec2(std::string name, glm::vec2 value);
		void setVec3(std::string name, float x, float y, float z);
		void setVec3(std::string name, glm::vec3 value);
		void setMat4(std::string name, glm::mat4 value);

		void setUniformBlockBinding(std::string name, unsigned int index);
		void setShaderStorageBlockBinding(std::string name, unsigned int index);

	private:
		unsigned int _id;

		void _checkCompileErrors(unsigned int shader, std::string type);
};

#endif