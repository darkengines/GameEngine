#include "MeshSystem.hpp"
#include "../Components/HasShadowDraw.hpp"
#include "../Components/MeshResource.hpp"
#include "../Models/Mesh.hpp"
#include "../../Graphics/SynchronizationState.hpp"
#include "../../Materials/Models/Material.hpp"
#include "../Models/MeshDraw.hpp"
#include "../Models/ShadowMeshDraw.hpp"
#include "../../Scenes/Draws/SceneDraw.hpp"
#include "../../Scenes/Draws/ShadowSceneDraw.hpp"
#include "../Pipelines/MeshPipeline.hpp"
#include "../Pipelines/ShadowMeshPipeline.hpp"
#include "../Components/MeshDraw.hpp"
#include "../Components/ShadowMeshDraw.hpp"
#include "../../Materials/Systems/MaterialSystem.hpp"
#include <algorithm>
#include "../../Objects/Components/Dirty.hpp"
#include "../../Objects/Components/ObjectMesh.hpp"
#include "../../Lights/Components/PointLight.hpp"
#include "../../Lights/Components/DirectionalLight.hpp"
#include "../../Lights/Components/Spotlight.hpp"
#include "../../Lights/Models/PointLight.hpp"
#include "../../Lights/Models/DirectionalLight.hpp"
#include "../../Lights/Models/Spotlight.hpp"
#include "../../Lights/Components/LightPerspectiveCollection.hpp"

namespace drk::Meshes::Systems {

	MeshSystem::MeshSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	)
		: System(engineState, registry), deviceContext(deviceContext) {}

	void MeshSystem::update(Models::Mesh& model, const Components::Mesh& mesh) {
		auto materialStoreItem = registry.get<Stores::StoreItem<Materials::Models::Material>>(mesh.materialEntity);
		const auto& frameStoreItem = materialStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.materialItemLocation.storeIndex = frameStoreItem.pStore->descriptorArrayElement;
		model.materialItemLocation.itemIndex = frameStoreItem.index;
	}

	void MeshSystem::uploadMeshes() {
		auto meshEntities = registry.view<std::shared_ptr<Components::MeshResource>>(entt::exclude<Components::MeshBufferView>);
		std::vector<entt::entity> processedMeshEntities;
		std::vector<std::shared_ptr<Meshes::Components::MeshResource>> meshes;
		meshEntities.each(
			[&meshes, &processedMeshEntities](const auto meshEntity, const auto& meshInfo) {
				processedMeshEntities.push_back(meshEntity);
				meshes.push_back(meshInfo);
			}
		);
		if (!meshes.empty()) {
			const auto result = engineState.uploadMeshes(meshes);
			for (auto meshIndex = 0u; meshIndex < processedMeshEntities.size(); meshIndex++) {
				registry.emplace<Components::MeshBufferView>(
					processedMeshEntities[meshIndex],
					result.meshes[meshIndex]
				);
			}
		}
	}
	void MeshSystem::updateDraw(entt::entity drawEntity, int drawIndex) {
		const auto& meshDraw = registry.get<Components::MeshDraw>(drawEntity);
		auto& frameState = engineState.getCurrentFrameState();
		//todo: optimization - fetch uniform store in parent scope and give as argument
		auto& drawStore = frameState.getUniformStore<Models::MeshDraw>();
		const auto& meshItemLocation = drawStore.get(drawIndex);
		meshItemLocation.pItem->meshItemLocation.storeIndex = meshDraw.meshItemLocation.storeIndex;
		meshItemLocation.pItem->meshItemLocation.itemIndex = meshDraw.meshItemLocation.itemIndex;
		meshItemLocation.pItem->objectItemLocation.storeIndex = meshDraw.objectItemLocation.storeIndex;
		meshItemLocation.pItem->objectItemLocation.itemIndex = meshDraw.objectItemLocation.itemIndex;
		meshItemLocation.pItem->cameraItemLocation = meshDraw.cameraItemLocation;
	}
	void MeshSystem::emitDraws() {
		const auto& [camera, cameraStoreItem] = registry.get<
			Cameras::Components::Camera,
			Stores::StoreItem<Cameras::Models::Camera>
		>(engineState.cameraEntity);
		auto objectMeshEntities = registry.view<Objects::Components::ObjectMesh>(entt::exclude<Components::MeshDraw>);
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
			Components::MeshDraw meshDraw = {
				.meshResource = meshResource,
				.meshBufferView = meshBufferView,
				.meshItemLocation = meshStoreItemLocation,
				.objectItemLocation = objectStoreItemLocation,
				.cameraItemLocation = cameraStoreItemLocation
			};
			auto entity = registry.create();
			registry.emplace_or_replace<Scenes::Draws::SceneDraw>(objectMeshEntity, std::move(draw));
			registry.emplace_or_replace<Components::MeshDraw>(objectMeshEntity, std::move(meshDraw));
			registry.emplace_or_replace<Graphics::SynchronizationState<Scenes::Draws::SceneDraw>>(objectMeshEntity, engineState.getFrameCount());
			});
	}
	void MeshSystem::processDirtyDraws() {
		if (registry.any_of<Objects::Components::Dirty<Spatials::Components::Spatial>>(engineState.cameraEntity)) {
			auto camera = registry.get<Cameras::Components::Camera>(engineState.cameraEntity);
			auto view = registry.view<
				Objects::Components::ObjectMesh,
				Components::MeshDraw,
				Scenes::Draws::SceneDraw
			>();
			view.each([&](
				entt::entity objectMeshEntity,
				const Objects::Components::ObjectMesh& objectMesh,
				const Components::MeshDraw& meshDraw,
				Scenes::Draws::SceneDraw& sceneDraw
				) {
					const auto& spatial = registry.get<Spatials::Components::Spatial>(objectMesh.objectEntity);
					sceneDraw.depth = glm::distance(camera.absolutePosition, spatial.absolutePosition);
					registry.emplace_or_replace<Graphics::SynchronizationState<Scenes::Draws::SceneDraw>>(objectMeshEntity, engineState.getFrameCount());
				});
		}
	}
	Draws::Components::DrawVertexBufferInfo MeshSystem::getVertexBufferInfo(entt::entity drawEntity) {
		auto meshDraw = registry.get<Components::MeshDraw>(drawEntity);
		Draws::Components::DrawVertexBufferInfo bufferInfo{
			static_cast<uint32_t>(meshDraw.meshResource->indices.size()),
				static_cast<uint32_t>(meshDraw.meshBufferView.IndexBufferView.byteOffset / sizeof(VertexIndex)),
				static_cast<int32_t>(meshDraw.meshBufferView.VertexBufferView.byteOffset / sizeof(Vertex))
		};
		return bufferInfo;
	}
	entt::entity
		MeshSystem::copyMeshEntity(const entt::registry& source, entt::registry& destination, entt::entity sourceEntity) {
		auto mesh = source.get<Components::Mesh>(sourceEntity);
		auto meshResource = source.try_get<std::shared_ptr<Components::MeshResource>>(sourceEntity);
		auto meshBufferView = source.try_get<Components::MeshBufferView>(sourceEntity);

		auto destinationEntity = destination.create();
		Components::Mesh detinationMesh{
			.materialEntity = Materials::Systems::MaterialSystem::copyMaterialEntity(source, destination, mesh.materialEntity)
		};
		destination.emplace<Components::Mesh>(destinationEntity, detinationMesh);
		if (meshBufferView != nullptr) {
			destination.emplace<Components::MeshBufferView>(destinationEntity, *meshBufferView);
		}
		if (meshResource != nullptr) {
			destination.emplace<std::shared_ptr<Components::MeshResource>>(destinationEntity, *meshResource);
		}

		return destinationEntity;
	}
}
