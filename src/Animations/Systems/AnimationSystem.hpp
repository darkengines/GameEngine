#include <entt/entt.hpp>
#include "../../Objects/Components/ObjectMesh.hpp"
#include "../Components/AnimationState.hpp"
#include "../Components/AnimationVertexBufferView.hpp"
#include "../../Meshes/Components/Mesh.hpp"
#include "../../Meshes/Components/MeshBufferView.hpp"
#include "../../Devices/BufferViewState.hpp"
#include "../../Devices/DeviceContext.hpp"
#include "../../Devices/Device.hpp"
#include "../../Engine/EngineState.hpp"
#include "../../Engine/DescriptorSetAllocator.hpp"
#include "../../Engine/DescriptorSetLayoutCache.hpp"

namespace drk::Animations::Systems {
	class AnimationSystem {
	protected:
		entt::registry& registry;
		Devices::DeviceContext& deviceContext;
		Engine::EngineState& engineState;
		std::vector<Devices::Buffer> buffers;
		Engine::DescriptorSetAllocator& descriptorSetAllocator;
		Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache;
	public:
		AnimationSystem(
			entt::registry& registry,
			Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState
		) :
			registry(registry),
			deviceContext(deviceContext),
			engineState(engineState),
			descriptorSetAllocator(descriptorSetAllocator),
			descriptorSetLayoutCache(descriptorSetLayoutCache) {}
		~AnimationSystem() {
			for (const auto buffer : buffers) {
				Devices::Device::destroyBuffer(deviceContext.Allocator, buffer);
			}
		}
		void storeMeshes() {
			auto view = registry.view<
				Objects::Components::ObjectMesh,
				Components::AnimationState
			>(entt::exclude<Components::AnimationVertexBufferView>);
			std::vector<Devices::BufferViewState<entt::entity>> meshBufferViews;
			view.each([&](
				entt::entity objectMeshEntity,
				const Objects::Components::ObjectMesh& objectMesh,
				const Components::AnimationState& animationState
				) {
					const auto& [mesh, meshBufferView] = registry.get<
						Meshes::Components::Mesh,
						Meshes::Components::MeshBufferView
					>(objectMesh.meshEntity);
					meshBufferViews.emplace_back(meshBufferView.VertexBufferView, objectMeshEntity);
				});
			auto bufferUploadResult = Devices::Device::cloneBufferViews(
				deviceContext.PhysicalDevice,
				deviceContext.device,
				deviceContext.GraphicQueue,
				deviceContext.CommandPool,
				deviceContext.Allocator,
				meshBufferViews,
				vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst
			);
			buffers.push_back(bufferUploadResult.buffer);
			for (const auto& bufferViewState : bufferUploadResult.bufferViewStates) {
				registry.emplace<Components::AnimationVertexBufferView>(bufferViewState.state, bufferViewState.bufferView);
			}
			vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{
				.
			}
			descriptorSetLayoutCache.get()
			descriptorSetAllocator.allocateDescriptorSets()
		}
	};
}