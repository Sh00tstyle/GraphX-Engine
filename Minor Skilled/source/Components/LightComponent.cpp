#include "LightComponent.h"

#include "../Engine/Node.h"

const unsigned int LightComponent::LightAmount = 10;

LightComponent::LightComponent(LightType type):Component(ComponentType::Light), lightType(type) {
}

LightComponent::~LightComponent() {
}

void LightComponent::update() {
}

GLLight LightComponent::toGLLight(glm::vec3& position) {
	GLLight newGLLight = GLLight();

	//fill with data based on the component
	newGLLight.position = glm::vec4(position.x, position.y, position.z, 1.0f);
	newGLLight.direction = glm::vec4(lightDirection.x, lightDirection.y, lightDirection.z, 1.0f);

	newGLLight.ambient = glm::vec4(lightAmbient.x, lightAmbient.y, lightAmbient.z, 1.0f);
	newGLLight.diffuse = glm::vec4(lightDiffuse.x, lightDiffuse.y, lightDiffuse.z, 1.0f);
	newGLLight.specular = glm::vec4(lightSpecular.x, lightSpecular.y, lightSpecular.z, 1.0f);

	newGLLight.type = (GLint)lightType;

	newGLLight.constant = (GLfloat)constantAttenuation;
	newGLLight.linear = (GLfloat)linearAttenuation;
	newGLLight.quadratic = (GLfloat)quadraticAttenuation;
	newGLLight.innerCutoff = (GLfloat)innerCutoff;
	newGLLight.outerCutoff = (GLfloat)outerCutoff;

	newGLLight.padding = glm::vec2(0.0f, 0.0f); //used to fill up the memory

	return newGLLight;
}
