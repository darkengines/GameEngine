
#pragma once

#include "../../Systems/System.hpp"
#include "../Models/Light.hpp"
#include "../Components/Light.hpp"

namespace drk::Lights::Systems {
	class LightSystem: public drk::Systems::System<
		Models::Light,
		Components::Light
	> {
		void Update(Models::Light& lightModel, const Components::Light& lightComponent) override;
	};
}