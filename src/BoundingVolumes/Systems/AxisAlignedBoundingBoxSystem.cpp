#include "AxisAlignedBoundingBoxSystem.hpp"
#include "../Models/Vertex.hpp"
#include "../../Cameras/Components/Camera.hpp"
#include "../../Cameras/Models/Camera.hpp"
#include "../Components/Draw.hpp"
#include "../../Nodes/Models/Node.hpp"
#include "../Pipelines/BoundingVolumePipeline.hpp"
#include "../../Scenes/Draws/SceneDraw.hpp"
#include "../Components/HasDraw.hpp"
#include "../Models/BoundingVolumeDraw.hpp"
#include "../../Meshes/Components/Mesh.hpp"

namespace drk::BoundingVolumes::Systems {
	AxisAlignedBoundingBoxSystem::AxisAlignedBoundingBoxSystem(
		Engine::EngineState& engineState,
		entt::registry& registry,
		const Devices::DeviceContext& deviceContext
	) : System<
		Models::AxisAlignedBoundingBox,
		Components::AxisAlignedBoundingBox,
		Nodes::Components::NodeMesh
	>(engineState, registry), deviceContext(deviceContext) {
		createResources();
	}
	void AxisAlignedBoundingBoxSystem::update(
		Models::AxisAlignedBoundingBox& axisAlignedBoundingBoxModel,
		const Components::AxisAlignedBoundingBox& axisAlignedBoundingBox,
		const Nodes::Components::NodeMesh& nodeMesh
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

		auto& meshStoreItem = registry.get<Stores::StoreItem<Models::AxisAlignedBoundingBox>>(draw.boundingVolumeEntity);
		auto& cameraStoreItem = registry.get<Stores::StoreItem<Cameras::Models::Camera>>(engineState.cameraEntity);

		auto frameIndex = engineState.getFrameIndex();
		boundingVolumeItemLocation.pItem->boundingVolumeStoreItemLocation = meshStoreItem.frameStoreItems[frameIndex];
		boundingVolumeItemLocation.pItem->cameraStoreItemLocation = cameraStoreItem.frameStoreItems[frameIndex];
	}

	void AxisAlignedBoundingBoxSystem::createResources() {
		glm::vec4 green = {0.0, 1.0, 0.0, 1.0};
		std::vector<drk::BoundingVolumes::Models::Vertex> cubeVertices{
			{
				.position = {1.0, 1.0, 1.0, 1.0},
				.diffuseColor = green
			},
			{
				.position = {-1.0, 1.0, 1.0, 1.0},
				.diffuseColor = green
			},
			{
				.position = {-1.0, 1.0, -1.0, 1.0},
				.diffuseColor = green
			},
			{
				.position = {1.0, 1.0, -1.0, 1.0},
				.diffuseColor = green
			},
			{
				.position = {1.0, -1.0, 1.0, 1.0},
				.diffuseColor = green
			},
			{
				.position = {-1.0, -1.0, 1.0, 1.0},
				.diffuseColor = green
			},
			{
				.position = {-1.0, -1.0, -1.0, 1.0},
				.diffuseColor = green
			},
			{
				.position = {1.0, -1.0, -1.0, 1.0},
				.diffuseColor = green
			}
		};
		std::vector<uint32_t> cubeIndices{0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};
		auto vertexUploadResult = Devices::Device::uploadBuffers<drk::BoundingVolumes::Models::Vertex>(
			deviceContext.PhysicalDevice,
			deviceContext.device,
			deviceContext.GraphicQueue,
			deviceContext.CommandPool,
			deviceContext.Allocator,
			{cubeVertices},
			vk::BufferUsageFlagBits::eVertexBuffer, "AxisAlignedBoundingBox.Resources.Vertices"
		);

		auto indexUploadResult = Devices::Device::uploadBuffers<uint32_t>(
			deviceContext.PhysicalDevice,
			deviceContext.device,
			deviceContext.GraphicQueue,
			deviceContext.CommandPool,
			deviceContext.Allocator,
			{cubeIndices},
			vk::BufferUsageFlagBits::eIndexBuffer, "AxisAlignedBoundingBox.Resources.Indices"
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
			Nodes::Components::NodeMesh,
			Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Relative>>
		>();
		view.each(
			[this](
				entt::entity entity,
				Components::AxisAlignedBoundingBox& axisAlignedBoundingBox,
				const Nodes::Components::NodeMesh& nodeMesh
			) {
				auto spatial = registry.get<Spatials::Components::Spatial<Spatials::Components::Absolute>>(
					nodeMesh.nodeEntity
				);
				axisAlignedBoundingBox.inplaceTransform(spatial.model);
				registry.emplace_or_replace<Graphics::SynchronizationState<Models::AxisAlignedBoundingBox>>(
					entity,
					engineState.getFrameCount());
			}
		);
	}

	void AxisAlignedBoundingBoxSystem::emitDraws() {
		const auto& camera = registry.get<Cameras::Components::Camera>(engineState.cameraEntity);
		auto objectMeshEntities = registry.view<
			Nodes::Components::NodeMesh,
			Meshes::Components::Mesh
		>(entt::exclude<Components::HasDraw>);

		objectMeshEntities.each(
			[&](
				entt::entity objectMeshEntity,
				Nodes::Components::NodeMesh nodeMesh,
				const Meshes::Components::Mesh& mesh
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
					.boundingVolumeEntity = objectMeshEntity,
					.cameraEntity = engineState.cameraEntity
				};
				auto entity = registry.create();
				registry.emplace_or_replace<Scenes::Draws::SceneDraw>(entity, std::move(draw));
				registry.emplace_or_replace<Components::Draw>(entity, std::move(Draw));
				registry.emplace_or_replace<Graphics::SynchronizationState<Scenes::Draws::SceneDraw>>(
					entity,
					engineState.getFrameCount());

				registry.emplace<Components::HasDraw>(objectMeshEntity);
			}
		);
	}
}