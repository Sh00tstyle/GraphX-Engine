#ifndef SYSTEM_H
#define SYSTEM_H

#include <glm/glm.hpp>

#include <bitset>
#include <vector>

class Entity;

class System {
	public:
		~System();

		virtual void update();
		virtual void render(glm::mat4 cameraModel, glm::mat4 projectionMatrix);

	protected:
		System(std::bitset<8> requirements);

		std::bitset<8> _requirements;

};

#endif