#include <entt/entt.hpp>
#include "../../Objects/Components/ObjectMesh.hpp"
#include "../Components/AnimationState.hpp"
#include "../Components/AnimationVertexBufferView.hpp"
#include "../../Meshes/Components/Mesh.hpp"
#include "../../Meshes/Components/MeshBufferView.hpp"
#include "../../Devices/BufferViewState.hpp"
#include "../../Devices/DeviceContext.hpp"
#include "../../Devices/Device.hpp"

namespace drk::Animations::Systems {
	class AnimationSystem {
	protected:
		entt::registry& registry;
		Devices::DeviceContext& deviceContext;
		std::vector<Devices::Buffer> buffers;
	public:
		AnimationSystem(
			entt::registry& registry,
			Devices::DeviceContext& deviceContext
		) : registry(registry), deviceContext(deviceContext) {}
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
		}
	};
}