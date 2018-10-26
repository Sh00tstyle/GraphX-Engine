#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include "../Engine/Component.h"

#include "../Engine/Material.h"
#include "../Engine/Model.h"

class RenderComponent : public Component {
	public:
		RenderComponent();
		~RenderComponent();

		Model* model;
		Material* material;

};

#endif