#include "../Models/Bone.hpp"
#include "../Components/Bone.hpp"
#include "../../Systems/System.hpp"

namespace drk::Animations::Systems {
	class BoneSystem : public drk::Systems::System<Models::Bone, Components::Bone> {
		void update(Models::Bone& boneModel, const Components::Bone& boneComponent) override;
		void mamadou() {
			registry.view<Components::Bone>().each([](entt::entity boneEntity, const Components::Bone& bone) {
				for (const auto weight : bone.weights) {
					weight.
				}
				});
		}
	};
}