#ifndef LIGHTCOMPONENT_H
#define LIGHTCOMPONENT_H

#include <glm/glm.hpp>

#include "../Engine/Component.h"
#include "../Utility/LightType.h"

class LightComponent : public Component {
	public:
		LightComponent();
		~LightComponent();

		LightType lightType;
		glm::vec3 lightColor;
		glm::vec3 lightDirection;
		float constantAttenuation;
		float linearAttenuation;
		float quadraticAttenuation;
		float innerCutoff;
		float outerCutoff;

};

#endif