#include "shader.h"

Shader::Shader(std::string vertexPath, std::string fragmentPath) {
	//retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	//ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		//open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		//read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		//close file handlers
		vShaderFile.close();
		fShaderFile.close();

		//convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

	} catch(std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char * fShaderCode = fragmentCode.c_str();

	//compile shaders
	unsigned int vertex, fragment;

	//vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	_checkCompileErrors(vertex, "VERTEX");

	//fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	_checkCompileErrors(fragment, "FRAGMENT");

	//shader Program
	_id = glCreateProgram();
	glAttachShader(_id, vertex);
	glAttachShader(_id, fragment);
	glLinkProgram(_id);
	_checkCompileErrors(_id, "PROGRAM");

	//delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::Shader(std::string vertexPath, std::string geometryPath, std::string fragmentPath) {
	//retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string geometryCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream gShaderFile;
	std::ifstream fShaderFile;

	//ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		//open files
		vShaderFile.open(vertexPath);
		gShaderFile.open(geometryPath);
		fShaderFile.open(fragmentPath);

		std::stringstream vShaderStream, gShaderStream, fShaderStream;

		//read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		gShaderStream << gShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		//close file handlers
		vShaderFile.close();
		gShaderFile.close();
		fShaderFile.close();

		//convert stream into string
		vertexCode = vShaderStream.str();
		geometryCode = gShaderStream.str();
		fragmentCode = fShaderStream.str();

	} catch(std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* gShaderCode = geometryCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//compile shaders
	unsigned int vertex, geometry, fragment;

	//vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	_checkCompileErrors(vertex, "VERTEX");

	//geometry shader
	geometry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry, 1, &gShaderCode, NULL);
	glCompileShader(geometry);
	_checkCompileErrors(geometry, "GEOMETRY");

	//fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	_checkCompileErrors(fragment, "FRAGMENT");

	//shader Program
	_id = glCreateProgram();
	glAttachShader(_id, vertex);
	glAttachShader(_id, geometry);
	glAttachShader(_id, fragment);
	glLinkProgram(_id);
	_checkCompileErrors(_id, "PROGRAM");

	//delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(geometry);
	glDeleteShader(fragment);
}

void Shader::use() {
	glUseProgram(_id);
}

void Shader::setBool(std::string name, bool value) {
	glUniform1i(glGetUniformLocation(_id, name.c_str()), (int)value);
}

void Shader::setInt(std::string name, int value) {
	glUniform1i(glGetUniformLocation(_id, name.c_str()), value);
}

void Shader::setFloat(std::string name, float value) {
	glUniform1f(glGetUniformLocation(_id, name.c_str()), value);
}

void Shader::setVec2(std::string name, float x, float y) {
	glUniform2f(glGetUniformLocation(_id, name.c_str()), x, y);
}

void Shader::setVec2(std::string name, glm::vec2 value) {
	glUniform2f(glGetUniformLocation(_id, name.c_str()), value.x, value.y);
}

void Shader::setVec3(std::string name, float x, float y, float z) {
	glUniform3f(glGetUniformLocation(_id, name.c_str()), x, y, z);
}

void Shader::setVec3(std::string name, glm::vec3 value) {
	glUniform3f(glGetUniformLocation(_id, name.c_str()), value.x, value.y, value.z);
}

void Shader::setMat4(std::string name, glm::mat4 value) {
	glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::_checkCompileErrors(unsigned int shader, std::string type) {
	int success;
	char infoLog[1024];
	if(type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	} else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if(!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}