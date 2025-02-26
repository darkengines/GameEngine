#pragma once

#include "../../Systems/StorageSystem.hpp"
#include "../Components/Bone.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../Models/BoneSpatial.hpp"

namespace drk::Animations::Systems {
	class BoneSpatialSystem
		: public drk::Systems::StorageSystem<Models::BoneSpatial, Spatials::Components::Spatial<Components::Bone>> {
	public:
		BoneSpatialSystem(
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		void
		update(Models::BoneSpatial& model, const Spatials::Components::Spatial<Components::Bone>& boneSpatial) override;
		void propagateChanges();
	};
}