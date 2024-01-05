#include "AxisAlignedBoundingBoxSystem.hpp"
#include "../../Objects/Components/ObjectMesh.hpp"
#include "../Models/Vertex.hpp"
#include "../../Cameras/Components/Camera.hpp"
#include "../../Cameras/Models/Camera.hpp"
#include "../Components/Draw.hpp"
#include "../../Objects/Models/Object.hpp"
#include "../Pipelines/BoundingVolumePipeline.hpp"
#include "../../Scenes/Draws/SceneDraw.hpp"
#include "../Components/HasDraw.hpp"
#include "../Models/BoundingVolumeDraw.hpp"

namespace drk::BoundingVolumes::Systems {
	AxisAlignedBoundingBoxSystem::AxisAlignedBoundingBoxSystem(
		Engine::EngineState& engineState,
		entt::registry& registry,
		Devices::DeviceContext& deviceContext
	) : System<
			Models::AxisAlignedBoundingBox, 
			Components::AxisAlignedBoundingBox,
			Objects::Components::ObjectMesh
	>(engineState, registry), deviceContext(deviceContext) {
		createResources();
	}
	void AxisAlignedBoundingBoxSystem::update(
		Models::AxisAlignedBoundingBox& axisAlignedBoundingBoxModel,
		const Components::AxisAlignedBoundingBox& axisAlignedBoundingBox,
		const Objects::Components::ObjectMesh& objectMesh
	) {
		axisAlignedBoundingBoxModel.center = axisAlignedBoundingBox.absoluteCenter;
		axisAlignedBoundingBoxModel.extent = axisAlignedBoundingBox.absoluteExtent;
	}
	void AxisAlignedBoundingBoxSystem::updateDraw(entt::entity drawEntity, int drawIndex) {
		const auto& draw = registry.get<Components::Draw>(drawEntity);
		auto& frameState = engineState.getCurrentFrameState();
		//todo: optimization - fetch uniform store in parent scope and give as argument
		auto& drawStore = frameState.getUniformStore<Models::BoundingVolumeDraw>();
		const auto& boundingVolumeItemLocation = drawStore.get(drawIndex);
		boundingVolumeItemLocation.pItem->boundingVolumeStoreItemLocation.storeIndex = draw.boundingVolumeStoreItemLocation.storeIndex;
		boundingVolumeItemLocation.pItem->boundingVolumeStoreItemLocation.itemIndex = draw.boundingVolumeStoreItemLocation.itemIndex;
		boundingVolumeItemLocation.pItem->cameraStoreItemLocation.storeIndex = draw.cameraStoreItemLocation.storeIndex;
		boundingVolumeItemLocation.pItem->cameraStoreItemLocation.itemIndex = draw.cameraStoreItemLocation.itemIndex;
	}

	void AxisAlignedBoundingBoxSystem::createResources() {
		glm::vec4 green = { 0.0, 1.0, 0.0, 1.0 };
		std::vector<drk::BoundingVolumes::Models::Vertex> cubeVertices{
			{
				.position = { 1.0, 1.0, 1.0, 1.0 },
				.diffuseColor = green
			}, {
				.position = { -1.0, 1.0, 1.0, 1.0 },
				.diffuseColor = green
			}, {
				.position = { -1.0, 1.0, -1.0, 1.0 },
				.diffuseColor = green
			}, {
				.position = { 1.0, 1.0, -1.0, 1.0 },
				.diffuseColor = green
			}, {
				.position = { 1.0, -1.0, 1.0, 1.0 },
				.diffuseColor = green
			}, {
				.position = { -1.0, -1.0, 1.0, 1.0 },
				.diffuseColor = green
			}, {
				.position = { -1.0, -1.0, -1.0, 1.0 },
				.diffuseColor = green
			}, {
				.position = { 1.0, -1.0, -1.0, 1.0 },
				.diffuseColor = green
			}
		};
		std::vector<uint32_t> cubeIndices{ 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7 };
		auto vertexUploadResult = Devices::Device::uploadBuffers<drk::BoundingVolumes::Models::Vertex>(
			deviceContext.PhysicalDevice,
			deviceContext.device,
			deviceContext.GraphicQueue,
			deviceContext.CommandPool,
			deviceContext.Allocator,
			{ cubeVertices },
			vk::BufferUsageFlagBits::eVertexBuffer
		);

		auto indexUploadResult = Devices::Device::uploadBuffers<uint32_t>(
			deviceContext.PhysicalDevice,
			deviceContext.device,
			deviceContext.GraphicQueue,
			deviceContext.CommandPool,
			deviceContext.Allocator,
			{ cubeIndices },
			vk::BufferUsageFlagBits::eIndexBuffer
		);

		vertexBufferView = vertexUploadResult.bufferViews[0];
		indexBufferView = indexUploadResult.bufferViews[0];
	}
	AxisAlignedBoundingBoxSystem::~AxisAlignedBoundingBoxSystem() {
		Devices::Device::destroyBuffer(deviceContext.Allocator, vertexBufferView.buffer);
		Devices::Device::destroyBuffer(deviceContext.Allocator, indexBufferView.buffer);
	}
	void AxisAlignedBoundingBoxSystem::processDirty() {
		auto view = registry.view<
			Components::AxisAlignedBoundingBox,
			Objects::Components::ObjectMesh,
			Objects::Components::Dirty<Spatials::Components::Spatial>
		>();
		view.each([this](
			entt::entity entity,
			Components::AxisAlignedBoundingBox& axisAlignedBoundingBox,
			const Objects::Components::ObjectMesh& objectMesh
			) {
				auto spatial = registry.get<Spatials::Components::Spatial>(objectMesh.objectEntity);
				axisAlignedBoundingBox.inplaceTransform(spatial.absoluteModel);
				registry.emplace_or_replace<Graphics::SynchronizationState<Models::AxisAlignedBoundingBox>>(entity, engineState.getFrameCount());
			});
	}

	void AxisAlignedBoundingBoxSystem::emitDraws() {
		const auto& [camera, cameraStoreItem] = registry.get<
			Cameras::Components::Camera,
			Stores::StoreItem<Cameras::Models::Camera>
		>(engineState.cameraEntity);
		auto objectMeshEntities = registry.view<
			Objects::Components::ObjectMesh,
			Stores::StoreItem<BoundingVolumes::Models::AxisAlignedBoundingBox>
		>(entt::exclude<Components::HasDraw>);
		const auto& cameraStoreItemLocation = cameraStoreItem.frameStoreItems[engineState.getFrameIndex()];

		objectMeshEntities.each([&](
			entt::entity objectMeshEntity,
			const Objects::Components::ObjectMesh& objectMesh,
			const Stores::StoreItem<BoundingVolumes::Models::AxisAlignedBoundingBox>& axisAlignedBoundingBoxStoreItem
			) {
				Scenes::Draws::SceneDraw draw = {
					.drawSystem = this,
					.pipelineTypeIndex = std::type_index(typeid(Pipelines::BoundingVolumePipeline)),
					.indexBufferView = indexBufferView,
					.vertexBufferView = vertexBufferView,
					.hasTransparency = false,
					.depth = 0.0f,
				};
				Components::Draw Draw = {
					.boundingVolumeStoreItemLocation = axisAlignedBoundingBoxStoreItem.frameStoreItems[engineState.getFrameIndex()],
					.cameraStoreItemLocation = cameraStoreItemLocation
				};
				auto entity = registry.create();
				registry.emplace_or_replace<Scenes::Draws::SceneDraw>(entity, std::move(draw));
				registry.emplace_or_replace<Components::Draw>(entity, std::move(Draw));
				registry.emplace_or_replace<Graphics::SynchronizationState<Scenes::Draws::SceneDraw>>(entity, engineState.getFrameCount());

				registry.emplace<Components::HasDraw>(objectMeshEntity);
			});
	}
}