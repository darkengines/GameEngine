#include "../../Devices/DeviceContext.hpp"
#include "../../Devices/Device.hpp"
#include "../../Engine/EngineState.hpp"
#include "../Resources/AnimationResourceManager.hpp"
#include "../Pipelines/SkinningPipeline.hpp"

namespace drk::Animations::Systems {
	class SkinnedMeshInstanceSystem {
	protected:
		entt::registry& registry;
		Devices::DeviceContext& deviceContext;
		Engine::EngineState& engineState;
		std::vector<Devices::Buffer> buffers;
		Engine::DescriptorSetAllocator& descriptorSetAllocator;
		Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache;
		Animations::Resources::AnimationResourceManager& animationResourceManager;
		Pipelines::SkinningPipeline& skinningPipeline;
	public:
		SkinnedMeshInstanceSystem(
			entt::registry& registry,
			Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			Animations::Resources::AnimationResourceManager& animationResourceManager,
			Pipelines::SkinningPipeline& skinningPipeline
		) :
			registry(registry),
			deviceContext(deviceContext),
			engineState(engineState),
			descriptorSetAllocator(descriptorSetAllocator),
			descriptorSetLayoutCache(descriptorSetLayoutCache),
			animationResourceManager(animationResourceManager),
			skinningPipeline(skinningPipeline) {}
		~SkinnedMeshInstanceSystem() {
			for (const auto buffer: buffers) {
				Devices::Device::destroyBuffer(deviceContext.Allocator, buffer);
			}
		}


	};
}