
#include "LineSystem.hpp"
#include "../Objects/Models/Object.hpp"
#include "../Materials/Models/Material.hpp"
#include "../Cameras/Components/Camera.hpp"
#include "../Spatials/Components/Spatial.hpp"
#include "Components/LineDraw.hpp"
#include "Models/LineDraw.hpp"
#include "Models/LineVertex.hpp"
#include "../Scenes/Draws/SceneDraw.hpp"
#include "LinePipeline.hpp"
#include <typeindex>

namespace drk::Lines {
	LineSystem::LineSystem(
		Engine::EngineState& engineState,
		entt::registry& registry,
		Devices::DeviceContext& deviceContext
	)
		: Systems::System<Models::Line, Components::Line>(engineState, registry), deviceContext(deviceContext) {
		CreateResources();
	}
	LineSystem::~LineSystem() {
		deviceContext.DestroyBuffer(lineIndexBufferView.buffer);
		deviceContext.DestroyBuffer(lineVertexBufferView.buffer);
	}
	void LineSystem::Update(Models::Line& model, const Components::Line& line) {
		const auto& materialModel = registry.get<Stores::StoreItem<Materials::Models::Material>>(line.materialEntity);
		model.materialItemLocation = materialModel.frameStoreItems[engineState.getFrameIndex()];
	}
	void LineSystem::CreateResources() {
		std::string lineMeshName = "Line";
		Models::LineVertex lineOriginVertex{
			.position = glm::vec4(0.0, 0.0, 0.0, 1.0),
			.diffuseColor = glm::vec4(0.0, 1.0, 0.0, 1.0),
			.textureCoordinates = glm::vec2(0.0, 0.0),
		};
		Models::LineVertex lineEndVertex{
			.position = glm::vec4(1.0, 0.0, 0.0, 1.0),
			.diffuseColor = glm::vec4(0.0, 1.0, 0.0, 1.0),
			.textureCoordinates = glm::vec2(0.0, 0.0),
		};
		vk::DeviceSize vertexOffset = 0;
		vk::DeviceSize indexOffset = 0;
		std::vector<Models::LineVertex> lineVertices{lineOriginVertex, lineEndVertex};
		auto vertexResult = Devices::Device::uploadBuffers<Models::LineVertex>(
			deviceContext.PhysicalDevice,
			deviceContext.device,
			deviceContext.GraphicQueue,
			deviceContext.CommandPool,
			deviceContext.Allocator,
			{lineVertices},
			vk::BufferUsageFlagBits::eVertexBuffer
		);
		auto vertexBuffer = vertexResult.buffer;
		lineVertexBufferView = {
			.buffer = vertexBuffer,
			.byteOffset = vertexOffset,
			.byteLength = sizeof(Lines::Models::LineVertex) * lineVertices.size()
		};
		std::vector<unsigned int> lineIndices{0u, 1u};
		auto indexResult = Devices::Device::uploadBuffers<unsigned int>(
			deviceContext.PhysicalDevice,
			deviceContext.device,
			deviceContext.GraphicQueue,
			deviceContext.CommandPool,
			deviceContext.Allocator,
			{lineIndices},
			vk::BufferUsageFlagBits::eIndexBuffer
		);
		auto indexBuffer = indexResult.buffer;
		lineIndexBufferView = {
			.buffer = indexBuffer,
			.byteOffset = indexOffset,
			.byteLength = sizeof(uint32_t) * lineIndices.size()
		};
	}
	void LineSystem::UpdateDraw(entt::entity drawEntity, int drawIndex) {
		const auto& lineDraw = registry.get<Models::LineDraw>(drawEntity);
		auto& frameState = engineState.getCurrentFrameState();
		//todo: optimization - fetch uniform store in parent scope and give as argument
		auto& drawStore = frameState.getUniformStore<Models::LineDraw>();
		const auto& lineItemLocation = drawStore.get(drawIndex);
		lineItemLocation.pItem->lineItemLocation.storeIndex = lineDraw.lineItemLocation.storeIndex;
		lineItemLocation.pItem->lineItemLocation.itemIndex = lineDraw.lineItemLocation.itemIndex;
		lineItemLocation.pItem->objectItemLocation.storeIndex = lineDraw.objectItemLocation.storeIndex;
		lineItemLocation.pItem->objectItemLocation.itemIndex = lineDraw.objectItemLocation.itemIndex;
	}
	bool LineSystem::EmitDraws() {
		auto lineEntities = registry.view<
			Stores::StoreItem<Models::Line>,
			Components::Line, Spatials::Components::Spatial,
			Stores::StoreItem<Objects::Models::Object>>(entt::exclude<Models::LineDraw>);
		auto hasEntities = lineEntities.begin() != lineEntities.end();
		if (hasEntities) {
			auto cameraEntity = engineState.CameraEntity;
			auto camera = registry.get<Cameras::Components::Camera>(cameraEntity);
			lineEntities.each(
				[&](
					entt::entity lineEntity,
					auto& lineStoreItem,
					auto& line,
					auto& spatial,
					auto& objectStoreItem
				) {
					const auto& lineStoreItemLocation = lineStoreItem.frameStoreItems[engineState.getFrameIndex()];
					const auto& objectStoreItemLocation = objectStoreItem.frameStoreItems[engineState.getFrameIndex()];
					const auto& material = registry.get<std::shared_ptr<Materials::Components::Material>>(line.materialEntity);

					Scenes::Draws::SceneDraw draw = {
						.drawSystem = this,
						.pipelineTypeIndex = std::type_index(typeid(LinePipeline)),
						.indexBufferView = lineIndexBufferView,
						.vertexBufferView = lineVertexBufferView,
						.hasTransparency = material->hasTransparency,
						.depth = glm::distance(camera.absolutePosition, spatial.absolutePosition)
					};
					Models::LineDraw lineDraw = {
						.lineItemLocation = lineStoreItemLocation,
						.objectItemLocation = objectStoreItemLocation
					};

					//auto entity = registry.create();
					registry.emplace<Scenes::Draws::SceneDraw>(lineEntity, draw);
					registry.emplace<Models::LineDraw>(lineEntity, lineDraw);
					registry.emplace<Graphics::SynchronizationState<Scenes::Draws::SceneDraw>>(
						lineEntity,
						engineState.getFrameCount()
					);
				}
			);
		}
		return hasEntities;
	}

	Draws::DrawVertexBufferInfo LineSystem::GetVertexBufferInfo(entt::entity drawEntity) {
		return Draws::DrawVertexBufferInfo{2, 0, 0};
	}
}
