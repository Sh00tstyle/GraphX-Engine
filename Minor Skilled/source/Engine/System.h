#ifndef SYSTEM_H
#define SYSTEM_H

#include <bitset>

class System {
	public:
		~System();

		virtual void update();
		virtual void render();

	protected:
		System(std::bitset<8> requirements);

		std::bitset<8> _requirements;

};

#endif