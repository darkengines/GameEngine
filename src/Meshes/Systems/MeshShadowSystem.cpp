#include "../../Cameras/Models/Camera.hpp"
#include "../../Cameras/Models/Camera.hpp"
#include "../../Lights/Components/DirectionalLight.hpp"
#include "../../Lights/Components/LightPerspectiveCollection.hpp"
#include "../../Lights/Components/PointLight.hpp"
#include "../../Lights/Components/Spotlight.hpp"
#include "../../Lights/Models/DirectionalLight.hpp"
#include "../../Lights/Models/LightPerspective.hpp"
#include "../../Lights/Models/PointLight.hpp"
#include "../../Lights/Models/Spotlight.hpp"
#include "../../Objects/Components/Dirty.hpp"
#include "../../Objects/Components/ObjectMesh.hpp"
#include "../../Objects/Components/ObjectMesh.hpp"
#include "../../Objects/Models/Object.hpp"
#include "../../Scenes/Draws/ShadowSceneDraw.hpp"
#include "../../Spatials/Models/Spatial.hpp"
#include "../../Stores/Models/StoreItemLocation.hpp"
#include "../../Stores/StoreItemLocation.hpp"
#include "../Components/HasShadowDraw.hpp"
#include "../Components/Mesh.hpp"
#include "../Components/ShadowMeshDraw.hpp"
#include "../Models/Mesh.hpp"
#include "../Models/ShadowMeshDraw.hpp"
#include "../Pipelines/ShadowMeshPipeline.hpp"
#include "MeshShadowSystem.hpp"

namespace drk::Meshes::Systems {
	MeshShadowSystem::MeshShadowSystem(
		entt::registry& registry,
		Engine::EngineState& engineState
	) :
		registry(registry),
		engineState(engineState)
	{}
	void MeshShadowSystem::updateDraw(entt::entity shadowDrawEntity, int drawIndex) {
		const auto& meshDraw = registry.get<Components::ShadowMeshDraw>(shadowDrawEntity);
		auto& frameState = engineState.getCurrentFrameState();
		//todo: optimization - fetch uniform store in parent scope and give as argument
		auto& drawStore = frameState.getUniformStore<Models::ShadowMeshDraw>();
		const auto& meshItemLocation = drawStore.get(drawIndex);
		meshItemLocation.pItem->meshItemLocation.storeIndex = meshDraw.meshItemLocation.storeIndex;
		meshItemLocation.pItem->meshItemLocation.itemIndex = meshDraw.meshItemLocation.itemIndex;
		meshItemLocation.pItem->objectItemLocation.storeIndex = meshDraw.objectItemLocation.storeIndex;
		meshItemLocation.pItem->objectItemLocation.itemIndex = meshDraw.objectItemLocation.itemIndex;
		meshItemLocation.pItem->cameraItemLocation = meshDraw.cameraItemLocation;
		meshItemLocation.pItem->LightPerspectiveItemLocation = meshDraw.lightPerspectiveItemLocation;
		meshItemLocation.pItem->LightPerspectiveSpatialItemLocation = meshDraw.lightPerspectiveSpatialItemLocation;
	}

	void MeshShadowSystem::updateDraws() {
	}
	void MeshShadowSystem::emitDraws() {
		const auto& [camera, cameraStoreItem] = registry.get<
			Cameras::Components::Camera,
			Stores::StoreItem<Cameras::Models::Camera>
		>(engineState.cameraEntity);
		const auto& cameraStoreItemLocation = cameraStoreItem.frameStoreItems[engineState.getFrameIndex()];
		auto objectMeshEntities = registry.view<Objects::Components::ObjectMesh>(entt::exclude<Components::HasShadowDraw>);
		auto pointLightView = registry.view<
			Lights::Components::PointLight,
			Stores::StoreItem<Lights::Models::PointLight>,
			Lights::Components::LightPerspectiveCollection,
			Stores::StoreItem<Spatials::Models::Spatial>
		>();
		auto directionalLightView = registry.view<
			Lights::Components::DirectionalLight,
			Stores::StoreItem<Lights::Models::DirectionalLight>,
			Lights::Components::LightPerspective,
			Stores::StoreItem<Lights::Models::LightPerspective>,
			Stores::StoreItem<Spatials::Models::Spatial>
		>();
		auto spotlightView = registry.view<
			Lights::Components::Spotlight,
			Stores::StoreItem<Lights::Models::Spotlight>,
			Lights::Components::LightPerspective,
			Stores::StoreItem<Lights::Models::LightPerspective>,
			Stores::StoreItem<Spatials::Models::Spatial>
		>();

		objectMeshEntities.each([&](entt::entity objectMeshEntity, const Objects::Components::ObjectMesh& objectMesh) {
			const auto& [mesh, pMeshResource, meshBufferView, meshStoreItem] = registry.get<
				Meshes::Components::Mesh,
				std::shared_ptr<Meshes::Components::MeshResource>,
				Meshes::Components::MeshBufferView,
				Stores::StoreItem<Meshes::Models::Mesh>
			>(objectMesh.meshEntity);
			const auto& [objectStoreItem, spatial] = registry.get<
				Stores::StoreItem<Objects::Models::Object>,
				Spatials::Components::Spatial
			>(objectMesh.objectEntity);
			const auto& pMaterial = registry.get<std::shared_ptr<Materials::Components::Material>>(mesh.materialEntity);
			auto currentFrameIndex = engineState.getFrameIndex();
			const auto& objectStoreItemLocation = objectStoreItem.frameStoreItems[currentFrameIndex];
			const auto& meshStoreItemLocation = meshStoreItem.frameStoreItems[currentFrameIndex];
			pointLightView.each([&](
				entt::entity pointLightEntity,
				const Lights::Components::PointLight& pointLight,
				const Stores::StoreItem<Lights::Models::PointLight>& pointLightStoreItem,
				const Lights::Components::LightPerspectiveCollection& lightPerspectiveCollection,
				const Stores::StoreItem<Spatials::Models::Spatial>& lightPerspectiveSpatialStoreItem
				) {
					for (const auto& lightPerspectiveEntity : lightPerspectiveCollection.lightPerspectives) {
						const auto& [lightPerspective, lightPerspectiveStoreItem] = registry.get<
							Lights::Components::LightPerspective,
							Stores::StoreItem<Lights::Models::LightPerspective>
						>(lightPerspectiveEntity);
						const auto& lightPerspectiveStoreItemLocation = lightPerspectiveStoreItem.frameStoreItems[engineState.getFrameIndex()];
						const auto& lightPerspectiveSpatialStoreItemLocation = lightPerspectiveSpatialStoreItem.frameStoreItems[engineState.getFrameIndex()];
						ProcessObjectEntity(
							objectMeshEntity,
							pointLightEntity,
							lightPerspectiveEntity,
							lightPerspective,
							camera,
							spatial,
							*pMaterial,
							meshBufferView,
							pMeshResource,
							objectStoreItemLocation,
							meshStoreItemLocation,
							cameraStoreItemLocation,
							lightPerspectiveStoreItemLocation,
							lightPerspectiveSpatialStoreItemLocation
						);
					}
				});

			directionalLightView.each([&](
				entt::entity directionalLightEntity,
				const Lights::Components::DirectionalLight& directionalLight,
				const Stores::StoreItem<Lights::Models::DirectionalLight>& directionalLightStoreItem,
				const Lights::Components::LightPerspective& lightPerspective,
				const Stores::StoreItem<Lights::Models::LightPerspective>& lightPerspectiveStoreItem,
				const Stores::StoreItem<Spatials::Models::Spatial>& lightPerspectiveSpatialStoreItem
				) {
					const auto& lightPerspectiveStoreItemLocation = lightPerspectiveStoreItem.frameStoreItems[engineState.getFrameIndex()];
					const auto& lightPerspectiveSpatialStoreItemLocation = lightPerspectiveSpatialStoreItem.frameStoreItems[engineState.getFrameIndex()];
					ProcessObjectEntity(
						objectMeshEntity,
						directionalLightEntity,
						directionalLightEntity,
						lightPerspective,
						camera,
						spatial,
						*pMaterial,
						meshBufferView,
						pMeshResource,
						objectStoreItemLocation,
						meshStoreItemLocation,
						cameraStoreItemLocation,
						lightPerspectiveStoreItemLocation,
						lightPerspectiveSpatialStoreItemLocation
					);

				});

			spotlightView.each([&](
				entt::entity spotlightEntity,
				const Lights::Components::Spotlight& spotlight,
				const Stores::StoreItem<Lights::Models::Spotlight>& spotlightStoreItem,
				const Lights::Components::LightPerspective& lightPerspective,
				const Stores::StoreItem<Lights::Models::LightPerspective>& lightPerspectiveStoreItem,
				const Stores::StoreItem<Spatials::Models::Spatial>& lightPerspectiveSpatialStoreItem
				) {
					const auto& lightPerspectiveStoreItemLocation = lightPerspectiveStoreItem.frameStoreItems[engineState.getFrameIndex()];
					const auto& lightPerspectiveSpatialStoreItemLocation = lightPerspectiveSpatialStoreItem.frameStoreItems[engineState.getFrameIndex()];
					ProcessObjectEntity(
						objectMeshEntity,
						spotlightEntity,
						spotlightEntity,
						lightPerspective,
						camera,
						spatial,
						*pMaterial,
						meshBufferView,
						pMeshResource,
						objectStoreItemLocation,
						meshStoreItemLocation,
						cameraStoreItemLocation,
						lightPerspectiveStoreItemLocation,
						lightPerspectiveSpatialStoreItemLocation
					);

				});
			registry.emplace<Components::HasShadowDraw>(objectMeshEntity);
			});
	}

	entt::entity MeshShadowSystem::ProcessObjectEntity(
		entt::entity objectMeshEntity,
		entt::entity lightEntity,
		entt::entity lightPerspectiveEntity,
		const Lights::Components::LightPerspective& perspective,
		const Cameras::Components::Camera& camera,
		const Spatials::Components::Spatial& spatial,
		const Materials::Components::Material& material,
		const Meshes::Components::MeshBufferView& meshBufferView,
		std::shared_ptr<Meshes::Components::MeshResource> pMeshResource,
		const Stores::Models::StoreItemLocation& objectStoreItemLocation,
		const Stores::Models::StoreItemLocation& meshStoreItemLocation,
		const Stores::Models::StoreItemLocation& cameraStoreItemLocation,
		const Stores::Models::StoreItemLocation& lightPerspectiveStoreItemLocation,
		const Stores::Models::StoreItemLocation& lightPerspectiveSpatialStoreItemLocation
	) {
		Scenes::Draws::ShadowSceneDraw draw = {
			.drawSystem = this,
			.pipelineTypeIndex = std::type_index(typeid(Pipelines::ShadowMeshPipeline)),
			.indexBufferView = meshBufferView.IndexBufferView,
			.vertexBufferView = meshBufferView.VertexBufferView,
			.lightEntity = lightEntity,
			.scissor = perspective.shadowMapRect,
			.lightPerspectiveEntity = lightPerspectiveEntity,
			.hasTransparency = material.hasTransparency,
			.depth = glm::distance(camera.absolutePosition, spatial.absolutePosition),
		};
		Components::ShadowMeshDraw meshDraw = {
			.meshResource = pMeshResource,
			.meshBufferView = meshBufferView,
			.meshItemLocation = meshStoreItemLocation,
			.objectItemLocation = objectStoreItemLocation,
			.cameraItemLocation = cameraStoreItemLocation,
			.lightPerspectiveItemLocation = lightPerspectiveStoreItemLocation,
			.lightPerspectiveSpatialItemLocation = lightPerspectiveSpatialStoreItemLocation
		};

		auto objectMeshPerspectiveEntity = registry.create();
		registry.emplace_or_replace<Scenes::Draws::ShadowSceneDraw>(objectMeshPerspectiveEntity, std::move(draw));
		registry.emplace_or_replace<Components::ShadowMeshDraw>(objectMeshPerspectiveEntity, std::move(meshDraw));
		registry.emplace_or_replace<Graphics::SynchronizationState<Scenes::Draws::ShadowSceneDraw>>(objectMeshPerspectiveEntity, engineState.getFrameCount());

		return objectMeshPerspectiveEntity;
	}
}