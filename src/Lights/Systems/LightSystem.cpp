
#include "LightSystem.hpp"

namespace drk::Lights::Systems {

	void LightSystem::Update(Models::Light& lightModel, const Components::Light& lightComponent) {
		lightModel.ambientColor = lightComponent.ambientColor;
		lightModel.specularColor = lightComponent.diffuseColor;
		lightModel.specularColor = lightComponent.specularColor;
	}
}
