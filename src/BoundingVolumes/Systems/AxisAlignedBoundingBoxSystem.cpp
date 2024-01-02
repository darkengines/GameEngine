#include "AxisAlignedBoundingBoxSystem.hpp"
#include "../../Objects/Components/ObjectMesh.hpp"
#include "../Models/Vertex.hpp"

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
}