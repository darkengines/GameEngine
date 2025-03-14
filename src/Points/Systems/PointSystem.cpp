
#include "PointSystem.hpp"
#include "../../Nodes/Models/Node.hpp"
#include "../../Materials/Models/Material.hpp"
#include "../../Cameras/Components/Camera.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../Components/PointDraw.hpp"
#include "../Models/PointDraw.hpp"
#include "../../Scenes/Draws/SceneDraw.hpp"
#include "../Pipelines/PointPrimitivePipeline.hpp"
#include <typeindex>

namespace drk::Points::Systems {
	PointSystem::PointSystem(
		Engine::EngineState& engineState,
		entt::registry& registry,
		Devices::DeviceContext& deviceContext
	)
		: drk::Systems::StorageSystem<Models::Point, Components::Point>(engineState, registry), deviceContext(deviceContext) {
		createResources();
	}
	PointSystem::~PointSystem() {
		deviceContext.DestroyBuffer(pointIndexBufferView.buffer);
		deviceContext.DestroyBuffer(pointVertexBufferView.buffer);
	}
	void PointSystem::update(Models::Point& model, const Components::Point& point) {
		const auto& materialModel = registry.get<Stores::StoreItem<Materials::Models::Material>>(point.materialEntity);
		model.materialItemLocation = materialModel.frameStoreItems[engineState.getFrameIndex()];
	}
	void PointSystem::createResources() {
		std::string pointMeshName = "Point";
		Models::PointVertex pointVertex{
			.position = glm::vec4(0.0, 0.0, 0.0, 1.0),
			.diffuseColor = glm::vec4(0.0, 1.0, 0.0, 1.0),
			.textureCoordinates = glm::vec2(0.0, 0.0),
		};
		vk::DeviceSize vertexOffset = 0;
		vk::DeviceSize indexOffset = 0;
		std::vector<Models::PointVertex> pointVertices{pointVertex};
		auto vertexResult = Devices::Device::uploadBuffers<Models::PointVertex>(
			deviceContext.PhysicalDevice,
			deviceContext.device,
			deviceContext.GraphicQueue,
			deviceContext.CommandPool,
			deviceContext.Allocator,
			{pointVertices},
			vk::BufferUsageFlagBits::eVertexBuffer, 
			fmt::format("{0}.VertexBuffer", typeid(PointSystem).name()).c_str()
		);
		auto vertexBuffer = vertexResult.buffer;
		pointVertexBufferView = {
			.buffer = vertexBuffer,
			.byteOffset = vertexOffset,
			.byteLength = sizeof(Points::Models::PointVertex)
		};
		std::vector<unsigned int> pointIndices{0u};
		auto indexResult = Devices::Device::uploadBuffers<unsigned int>(
			deviceContext.PhysicalDevice,
			deviceContext.device,
			deviceContext.GraphicQueue,
			deviceContext.CommandPool,
			deviceContext.Allocator,
			{pointIndices},
			vk::BufferUsageFlagBits::eIndexBuffer, fmt::format("{0}.IndexBuffer", typeid(PointSystem).name()).c_str()
		);
		auto indexBuffer = indexResult.buffer;
		pointIndexBufferView = {
			.buffer = indexBuffer,
			.byteOffset = indexOffset,
			.byteLength = sizeof(uint32_t)
		};
	}
	void PointSystem::updateDraw(entt::entity drawEntity, int drawIndex) {
		const auto& pointDraw = registry.get<Models::PointDraw>(drawEntity);
		auto& frameState = engineState.getCurrentFrameState();
		//todo: optimization - fetch uniform store in parent scope and give as argument
		auto& drawStore = frameState.getUniformStore<Models::PointDraw>();
		const auto& pointItemLocation = drawStore.get(drawIndex);
		pointItemLocation.pItem->pointItemLocation.storeIndex = pointDraw.pointItemLocation.storeIndex;
		pointItemLocation.pItem->pointItemLocation.itemIndex = pointDraw.pointItemLocation.itemIndex;
		pointItemLocation.pItem->objectItemLocation.storeIndex = pointDraw.objectItemLocation.storeIndex;
		pointItemLocation.pItem->objectItemLocation.itemIndex = pointDraw.objectItemLocation.itemIndex;
	}
	void PointSystem::emitDraws() {
		auto pointEntities = registry.view<
			Stores::StoreItem<Models::Point>,
			Components::Point,
			Spatials::Components::Spatial<Spatials::Components::Absolute>,
			Stores::StoreItem<Nodes::Models::Node>
		>(entt::exclude<Models::PointDraw>);

		auto cameraEntity = engineState.cameraEntity;
		const auto& [camera, cameraSpatial] = registry.get<Cameras::Components::Camera, Spatials::Components::Spatial<Spatials::Components::Absolute>>(cameraEntity);
		pointEntities.each(
			[&](
				entt::entity pointEntity,
				auto& pointStoreItem,
				auto& point,
				auto& spatial,
				auto& objectStoreItem
			) {
				const auto& pointStoreItemLocation = pointStoreItem.frameStoreItems[engineState.getFrameIndex()];
				const auto& objectStoreItemLocation = objectStoreItem.frameStoreItems[engineState.getFrameIndex()];
				const auto& material = registry.get<Materials::Components::Material>(point.materialEntity);

				Scenes::Draws::SceneDraw draw = {
					.drawSystem = this,
					.pipelineTypeIndex = std::type_index(typeid(Pipelines::PointPrimitivePipeline)),
					.indexBufferView = pointIndexBufferView,
					.vertexBufferView = pointVertexBufferView,
					.hasTransparency = material.hasTransparency,
					.depth = glm::distance(cameraSpatial.position, spatial.position)
				};
				Models::PointDraw pointDraw = {
					.pointItemLocation = pointStoreItemLocation,
					.objectItemLocation = objectStoreItemLocation
				};

				//auto entity = registry.create();
				registry.emplace<Scenes::Draws::SceneDraw>(pointEntity, draw);
				registry.emplace<Models::PointDraw>(pointEntity, pointDraw);
				registry.emplace<Graphics::SynchronizationState<Scenes::Draws::SceneDraw>>(
					pointEntity,
					engineState.getFrameCount());
			}
		);
	}
}
