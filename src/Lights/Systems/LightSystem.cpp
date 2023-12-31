
#include "LightSystem.hpp"

namespace drk::Lights::Systems {

	void LightSystem::update(Models::Light& lightModel, const Components::Light& lightComponent) {
		lightModel.ambientColor = lightComponent.ambientColor;
		lightModel.diffuseColor = lightComponent.diffuseColor;
		lightModel.specularColor = lightComponent.specularColor;
	}
}
