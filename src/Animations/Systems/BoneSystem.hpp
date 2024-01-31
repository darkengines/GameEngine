#pragma once

#include "../Models/Bone.hpp"
#include "../Components/Bone.hpp"
#include "../../Systems/System.hpp"
#include "../../Engine/EngineState.hpp"
#include <entt/entt.hpp>

namespace drk::Animations::Systems {
	class BoneSystem : public drk::Systems::System<Models::Bone, Components::Bone> {
	public:
		BoneSystem(
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		void update(Models::Bone& boneModel, const Components::Bone& boneComponent) override;
	};
}