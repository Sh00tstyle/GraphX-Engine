#ifndef GLLIGHT_H
#define GLLIGHT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

struct GLLight {
	public:
		glm::vec4 position;
		glm::vec4 direction;

		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;

		GLint type;

		GLfloat constant;
		GLfloat linear;
		GLfloat quadratic;
		GLfloat innerCutoff;
		GLfloat outerCutoff;

		glm::vec2 padding;
};

#endif
