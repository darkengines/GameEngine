#include "FrustumSystem.hpp"
#include <entt/entt.hpp>
#include "../Models/FrustumVertex.hpp"
#include "../Models/Vertex.hpp"
#include "../../Scenes/Draws/SceneDraw.hpp"
#include "../../Cameras/Models/Camera.hpp"
#include "../../Cameras/Components/Camera.hpp"
#include "../Pipelines/FrustumPipeline.hpp"
#include "../Components/Draw.hpp"
#include "../Components/HasDraw.hpp"
#include "../../Spatials/Models/Spatial.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../Models/FrustumDraw.hpp"
#include "../../Common/Components/Dirty.hpp"

namespace drk::Frustums::Systems {
	FrustumSystem::FrustumSystem(
		Engine::EngineState& engineState,
		entt::registry& registry,
		const Devices::DeviceContext& deviceContext
	) : StorageSystem<Models::Frustum, Components::Frustum>(engineState, registry),
		deviceContext(deviceContext) {
		createResources();
	}
	FrustumSystem::~FrustumSystem() {
		Devices::Device::destroyBuffer(deviceContext.Allocator, vertexBufferView.buffer);
		Devices::Device::destroyBuffer(deviceContext.Allocator, indexBufferView.buffer);
	}
	void FrustumSystem::update(Models::Frustum& frustumModel, const Components::Frustum& frustumComponent) {
		frustumModel.leftPlane = frustumComponent.leftPlane;
		frustumModel.rightPlane = frustumComponent.rightPlane;
		frustumModel.farPlane = frustumComponent.farPlane;
		frustumModel.nearPlane = frustumComponent.nearPlane;
		frustumModel.bottomPlane = frustumComponent.bottomPlane;
		frustumModel.topPlane = frustumComponent.topPlane;
		frustumModel.leftTopNear = frustumComponent.leftTopNear;
		frustumModel.rightTopNear = frustumComponent.rightTopNear;
		frustumModel.leftBottomNear = frustumComponent.leftBottomNear;
		frustumModel.rightBottomNear = frustumComponent.rightBottomNear;
		frustumModel.leftTopFar = frustumComponent.leftTopFar;
		frustumModel.rightTopFar = frustumComponent.rightTopFar;
		frustumModel.leftBottomFar = frustumComponent.leftBottomFar;
		frustumModel.rightBottomFar = frustumComponent.rightBottomFar;
	}
	void FrustumSystem::createResources() {
		glm::vec4 green = {0.0, 1.0, 0.0, 1.0};
		std::vector<drk::Frustums::Models::Vertex> frustumVertices{
			{
				.position = Models::FrustumVertex::Near | Models::FrustumVertex::Top | Models::FrustumVertex::Left,
				.diffuseColor = green
			},
			{
				.position = Models::FrustumVertex::Near | Models::FrustumVertex::Top | Models::FrustumVertex::Right,
				.diffuseColor = green
			},
			{
				.position = Models::FrustumVertex::Near | Models::FrustumVertex::Bottom | Models::FrustumVertex::Left,
				.diffuseColor = green
			},
			{
				.position = Models::FrustumVertex::Near | Models::FrustumVertex::Bottom | Models::FrustumVertex::Right,
				.diffuseColor = green
			},
			{
				.position = Models::FrustumVertex::Far | Models::FrustumVertex::Top | Models::FrustumVertex::Left,
				.diffuseColor = green
			},
			{
				.position = Models::FrustumVertex::Far | Models::FrustumVertex::Top | Models::FrustumVertex::Right,
				.diffuseColor = green
			},
			{
				.position = Models::FrustumVertex::Far | Models::FrustumVertex::Bottom | Models::FrustumVertex::Left,
				.diffuseColor = green
			},
			{
				.position = Models::FrustumVertex::Far | Models::FrustumVertex::Bottom | Models::FrustumVertex::Right,
				.diffuseColor = green
			}
		};
		std::vector<uint32_t> frustumIndices{0, 1, 1, 3, 3, 2, 2, 0, 4, 5, 5, 7, 7, 6, 6, 4, 0, 4, 1, 5, 2, 6, 3, 7};
		auto vertexUploadResult = Devices::Device::uploadBuffers<Models::Vertex>(
			deviceContext.PhysicalDevice,
			deviceContext.device,
			deviceContext.GraphicQueue,
			deviceContext.CommandPool,
			deviceContext.Allocator,
			{frustumVertices},
			vk::BufferUsageFlagBits::eVertexBuffer,
			fmt::format("{0}.VertexBuffer", typeid(FrustumSystem).name()).c_str()
		);

		auto indexUploadResult = Devices::Device::uploadBuffers<uint32_t>(
			deviceContext.PhysicalDevice,
			deviceContext.device,
			deviceContext.GraphicQueue,
			deviceContext.CommandPool,
			deviceContext.Allocator,
			{frustumIndices},
			vk::BufferUsageFlagBits::eIndexBuffer, fmt::format("{0}.IndexBuffer", typeid(FrustumSystem).name()).c_str()
		);

		vertexBufferView = vertexUploadResult.bufferViews[0];
		indexBufferView = indexUploadResult.bufferViews[0];
	}
	void FrustumSystem::updateDraw(entt::entity drawEntity, int drawIndex) {
		const auto& draw = registry.get<Components::Draw>(drawEntity);
		auto& frameState = engineState.getCurrentFrameState();
		//todo: optimization - fetch uniform store in parent scope and give as argument
		auto& drawStore = frameState.getUniformStore<Models::FrustumDraw>();
		const auto& frustumItemLocation = drawStore.get(drawIndex);

		const auto& [frustumStoreItem, spatial] = registry.get<
			Stores::StoreItem<Models::Frustum>,
			Stores::StoreItem<Spatials::Models::Spatial>
		>(draw.frustumEntity);
		const auto& cameraStoreItem = registry.get<Stores::StoreItem<Cameras::Models::Camera>>(engineState.cameraEntity);

		auto frameIndex = engineState.getFrameIndex();
		frustumItemLocation.pItem->frustumStoreItemLocation = frustumStoreItem.frameStoreItems[frameIndex];
		frustumItemLocation.pItem->spatialStoreItemLocation = spatial.frameStoreItems[frameIndex];
		frustumItemLocation.pItem->cameraStoreItemLocation = cameraStoreItem.frameStoreItems[frameIndex];
	}
	void FrustumSystem::processDirty() {

	}
	void FrustumSystem::emitDraws() {
		const auto& camera = registry.get<Cameras::Components::Camera>(engineState.cameraEntity);
		const auto frustumEntities = registry.view<
			Components::Frustum
		>(entt::exclude<Components::HasDraw>);

		frustumEntities.each(
			[&](
				entt::entity frustumEntity,
				const Components::Frustum& frustum
			) {
				if (frustumEntity == engineState.cameraEntity)
					return;
				Scenes::Draws::SceneDraw draw = {
					.drawSystem = this,
					.pipelineTypeIndex = std::type_index(typeid(Pipelines::FrustumPipeline)),
					.indexBufferView = indexBufferView,
					.vertexBufferView = vertexBufferView,
					.hasTransparency = false,
					.depth = 0.0f,
				};
				Components::Draw Draw = {
					.frustumEntity = frustumEntity,
					.spatialEntity = frustumEntity,
					.cameraEntity = engineState.cameraEntity,
				};
				auto entity = registry.create();
				registry.emplace_or_replace<Scenes::Draws::SceneDraw>(entity, std::move(draw));
				registry.emplace_or_replace<Components::Draw>(entity, std::move(Draw));
				registry.emplace_or_replace<Graphics::SynchronizationState<Scenes::Draws::SceneDraw>>(
					entity,
					engineState.getFrameCount());

				registry.emplace<Components::HasDraw>(frustumEntity);
			}
		);
	}
}