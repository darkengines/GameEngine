#include "AxisAlignedBoundingBoxSystem.hpp"
#include "../../Objects/Components/ObjectMesh.hpp"
#include "../Models/Vertex.hpp"
#include "../../Cameras/Components/Camera.hpp"
#include "../../Cameras/Models/Camera.hpp"
#include "../Components/Draw.hpp"

namespace drk::BoundingVolumes::Systems {
	AxisAlignedBoundingBoxSystem::AxisAlignedBoundingBoxSystem(
		Engine::EngineState& engineState,
		entt::registry& registry,
		Devices::DeviceContext& deviceContext
	) : System(engineState, registry), deviceContext(deviceContext) {
		createResources();
	}
	void AxisAlignedBoundingBoxSystem::update(
		Models::AxisAlignedBoundingBox& axisAlignedBoundingBoxModel,
		const Components::AxisAlignedBoundingBox& axisAlignedBoundingBox
	) {
		axisAlignedBoundingBoxModel.center = axisAlignedBoundingBox.center;
		axisAlignedBoundingBoxModel.extent = axisAlignedBoundingBox.extent;
	}
	void AxisAlignedBoundingBoxSystem::createResources() {
		glm::vec4 green = { 0.0, 1.0, 0.0, 1.0 };
		std::vector<drk::BoundingVolumes::Models::Vertex> cubeVertices{
			{
				.position = { 0.5, 0.5, 0.5, 1.0 },
				.diffuseColor = green
			}, {
				.position = { -0.5, 0.5, 0.5, 1.0 },
				.diffuseColor = green
			}, {
				.position = { -0.5, 0.5, -0.5, 1.0 },
				.diffuseColor = green
			}, {
				.position = { 0.5, 0.5, -0.5, 1.0 },
				.diffuseColor = green
			}, {
				.position = { 0.5, -0.5, 0.5, 1.0 },
				.diffuseColor = green
			}, {
				.position = { -0.5, -0.5, 0.5, 1.0 },
				.diffuseColor = green
			}, {
				.position = { -0.5, -0.5, -0.5, 1.0 },
				.diffuseColor = green
			}, {
				.position = { 0.5, -0.5, -0.5, 1.0 },
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
				axisAlignedBoundingBox.transform(spatial.absoluteModel);
			});
	}

	void AxisAlignedBoundingBoxSystem::emitDraws() {
		/*const auto& [camera, cameraStoreItem] = registry.get<
			Cameras::Components::Camera,
			Stores::StoreItem<Cameras::Models::Camera>
		>(engineState.cameraEntity);
		auto objectMeshEntities = registry.view<Objects::Components::ObjectMesh>(entt::exclude<Components::Draw>);
		const auto& cameraStoreItemLocation = cameraStoreItem.frameStoreItems[engineState.getFrameIndex()];

		objectMeshEntities.each([&](entt::entity objectMeshEntity, const Objects::Components::ObjectMesh& objectMesh) {
			const auto& [mesh, meshResource, meshBufferView, meshStoreItem] = registry.get<
				Meshes::Components::Mesh,
				std::shared_ptr<Meshes::Components::MeshResource>,
				Meshes::Components::MeshBufferView,
				Stores::StoreItem<Meshes::Models::Mesh>
			>(objectMesh.meshEntity);
			const auto& [objectStoreItem, spatial] = registry.get<
				Stores::StoreItem<Objects::Models::Object>,
				Spatials::Components::Spatial
			>(objectMesh.objectEntity);
			auto& material = registry.get<std::shared_ptr<Materials::Components::Material>>(mesh.materialEntity);
			const auto& objectStoreItemLocation = objectStoreItem.frameStoreItems[engineState.getFrameIndex()];
			const auto& meshStoreItemLocation = meshStoreItem.frameStoreItems[engineState.getFrameIndex()];
			Scenes::Draws::SceneDraw draw = {
				.drawSystem = this,
				.pipelineTypeIndex = std::type_index(typeid(Pipelines::MeshPipeline)),
				.indexBufferView = meshBufferView.IndexBufferView,
				.vertexBufferView = meshBufferView.VertexBufferView,
				.hasTransparency = material->hasTransparency,
				.depth = glm::distance(camera.absolutePosition, spatial.absolutePosition),
			};
			Components::Draw Draw = {
				.meshResource = meshResource,
				.meshBufferView = meshBufferView,
				.meshItemLocation = meshStoreItemLocation,
				.objectItemLocation = objectStoreItemLocation,
				.cameraItemLocation = cameraStoreItemLocation
			};
			auto entity = registry.create();
			registry.emplace_or_replace<Scenes::Draws::SceneDraw>(objectMeshEntity, std::move(draw));
			registry.emplace_or_replace<Components::Draw>(objectMeshEntity, std::move(Draw));
			registry.emplace_or_replace<Graphics::SynchronizationState<Scenes::Draws::SceneDraw>>(objectMeshEntity, engineState.getFrameCount());
			});

		return true;*/
	}
}