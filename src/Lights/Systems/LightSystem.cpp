
#include "LightSystem.hpp"

namespace drk::Lights::Systems {

	void LightSystem::Update(Models::Light& lightModel, const Components::Light& lightComponent) {
		lightModel.ambientColor = lightComponent.ambientColor;
		lightModel.diffuseColor = lightComponent.diffuseColor;
		lightModel.specularColor = lightComponent.specularColor;
	}
}
