#include "MeshSystem.hpp"
#include "Components/MeshDrawCollection.hpp"
#include "Components/MeshResource.hpp"
#include "Models/Mesh.hpp"
#include "../Graphics/SynchronizationState.hpp"
#include "../Materials/Models/Material.hpp"
#include "Models/MeshDraw.hpp"
#include "Models/ShadowMeshDraw.hpp"
#include "../Scenes/Draws/SceneDraw.hpp"
#include "../Scenes/Draws/ShadowSceneDraw.hpp"
#include "../Meshes/Pipelines/MeshPipeline.hpp"
#include "../Meshes/Pipelines/ShadowMeshPipeline.hpp"
#include "Components/MeshDraw.hpp"
#include "Components/ShadowMeshDraw.hpp"
#include "../Materials/MaterialSystem.hpp"
#include <algorithm>
#include "../Objects/Dirty.hpp"
#include "../Lights/Components/PointLight.hpp"
#include "../Lights/Components/DirectionalLight.hpp"
#include "../Lights/Components/Spotlight.hpp"
#include "../Lights/Models/PointLight.hpp"
#include "../Lights/Models/DirectionalLight.hpp"
#include "../Lights/Models/Spotlight.hpp"
#include "../Lights/Components/LightPerspectiveCollection.hpp"

namespace drk::Meshes {

	MeshSystem::MeshSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	)
		: System(engineState, registry), deviceContext(deviceContext) {}

	void MeshSystem::Update(Models::Mesh& model, const Components::Mesh& mesh) {
		auto materialStoreItem = registry.get<Stores::StoreItem<Materials::Models::Material>>(mesh.materialEntity);
		const auto& frameStoreItem = materialStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.materialItemLocation.storeIndex = frameStoreItem.pStore->descriptorArrayElement;
		model.materialItemLocation.itemIndex = frameStoreItem.index;
	}

	void MeshSystem::UploadMeshes() {
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
			const auto result = engineState.UploadMeshes(meshes);
			for (auto meshIndex = 0u; meshIndex < processedMeshEntities.size(); meshIndex++) {
				registry.emplace<Components::MeshBufferView>(
					processedMeshEntities[meshIndex],
					result.meshes[meshIndex]
				);
			}
		}
	}

	void MeshSystem::AddMeshSystem(entt::registry& registry) {
		registry.on_construct<Components::Mesh>().connect<MeshSystem::OnMeshConstruct>();
	}

	void MeshSystem::RemoveMeshSystem(entt::registry& registry) {
		registry.on_construct<Components::Mesh>().disconnect<MeshSystem::OnMeshConstruct>();
	}

	void MeshSystem::OnMeshConstruct(entt::registry& registry, entt::entity meshEntity) {

	}
	void MeshSystem::UpdateDraw(entt::entity drawEntity, int drawIndex) {
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
	void MeshSystem::UpdateShadowDraw(entt::entity shadowDrawEntity, int drawIndex) {
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
	bool MeshSystem::EmitDraws() {
		auto objectEntities = registry.view<Stores::StoreItem<Objects::Models::Object>, Meshes::MeshGroup, Spatials::Components::Spatial>(entt::exclude<Components::MeshDrawCollection>);
		auto hasEntities = objectEntities.begin() != objectEntities.end();
		if (hasEntities) {
			auto cameraEntity = engineState.CameraEntity;
			const auto& [camera, cameraStoreItem] = registry.get<Cameras::Components::Camera, Stores::StoreItem<Cameras::Models::Camera>>(cameraEntity);
			const auto& cameraStoreItemLocation = cameraStoreItem.frameStoreItems[engineState.getFrameIndex()];
			objectEntities.each(
				[&](
					entt::entity objectEntity,
					auto& objectStoreItem,
					auto& meshGroup,
					auto& spatial
					) {
						const auto& objectStoreItemLocation = objectStoreItem.frameStoreItems[engineState.getFrameIndex()];
						Components::MeshDrawCollection meshDrawCollection;
						for (const auto& meshEntity : meshGroup.meshEntities) {
							auto& mesh = registry.get<Components::Mesh>(meshEntity);
							auto meshResource = registry.get<std::shared_ptr<Meshes::Components::MeshResource>>(meshEntity);
							auto& material = registry.get<std::shared_ptr<Materials::Components::Material>>(mesh.materialEntity);
							const Meshes::Components::MeshBufferView& meshBufferView = registry.get<Meshes::Components::MeshBufferView>(
								meshEntity
							);
							const Stores::StoreItem<Meshes::Models::Mesh> meshStoreItem = registry.get<Stores::StoreItem<Meshes::Models::Mesh>>(
								meshEntity
							);
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
							meshDrawCollection.meshDrawEntities.push_back(entity);
							registry.emplace_or_replace<Scenes::Draws::SceneDraw>(entity, std::move(draw));
							registry.emplace_or_replace<Components::MeshDraw>(entity, std::move(meshDraw));
							registry.emplace_or_replace<Graphics::SynchronizationState<Scenes::Draws::SceneDraw>>(entity, engineState.getFrameCount());
						}
						registry.emplace<Components::MeshDrawCollection>(objectEntity, std::move(meshDrawCollection));
				}
			);
		}
		return hasEntities;
	}
	bool MeshSystem::EmitShadowDraws() {
		auto objectEntities = registry.view<Stores::StoreItem<Objects::Models::Object>, Meshes::MeshGroup, Spatials::Components::Spatial>(entt::exclude<Components::ShadowMeshDrawCollection>);
		auto hasEntities = objectEntities.begin() != objectEntities.end();
		if (hasEntities) {
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

			const auto& [camera, cameraStoreItem] = registry.get<Cameras::Components::Camera, Stores::StoreItem<Cameras::Models::Camera>>(engineState.CameraEntity);
			const auto& cameraStoreItemLocation = cameraStoreItem.frameStoreItems[engineState.getFrameIndex()];

			objectEntities.each([&](
				entt::entity objectEntity,
				auto& objectStoreItem,
				auto& meshGroup,
				auto& spatial
				) {
					Components::ShadowMeshDrawCollection shadowMeshDrawCollection;
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
									objectEntity,
									objectStoreItem,
									meshGroup,
									spatial,
									camera,
									cameraStoreItemLocation,
									pointLightEntity,
									lightPerspectiveEntity,
									lightPerspective,
									lightPerspectiveStoreItemLocation,
									lightPerspectiveSpatialStoreItemLocation,
									shadowMeshDrawCollection
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
								objectEntity,
								objectStoreItem,
								meshGroup,
								spatial,
								camera,
								cameraStoreItemLocation,
								directionalLightEntity,
								directionalLightEntity,
								lightPerspective,
								lightPerspectiveStoreItemLocation,
								lightPerspectiveSpatialStoreItemLocation,
								shadowMeshDrawCollection
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
								objectEntity,
								objectStoreItem,
								meshGroup,
								spatial,
								camera,
								cameraStoreItemLocation,
								spotlightEntity,
								spotlightEntity,
								lightPerspective,
								lightPerspectiveStoreItemLocation,
								lightPerspectiveSpatialStoreItemLocation,
								shadowMeshDrawCollection
							);

						});
					registry.emplace<Components::ShadowMeshDrawCollection>(objectEntity, std::move(shadowMeshDrawCollection));
				});
		}
		return hasEntities;
	}
	void MeshSystem::ProcessDirtyDraws() {
		if (registry.try_get<Objects::Dirty<Spatials::Components::Spatial>>(engineState.CameraEntity) != nullptr) {
			auto camera = registry.get<Cameras::Components::Camera>(engineState.CameraEntity);
			auto view = registry.view<Components::MeshDrawCollection>();
			view.each([&](entt::entity meshDrawCollectionEntity, const Components::MeshDrawCollection& meshDrawCollection) {
				const auto& spatial = registry.get<Spatials::Components::Spatial>(meshDrawCollectionEntity);
				for (entt::entity meshDrawEntity : meshDrawCollection.meshDrawEntities) {
					auto& meshDraw = registry.get<Scenes::Draws::SceneDraw>(meshDrawEntity);
					meshDraw.depth = glm::distance(camera.absolutePosition, spatial.absolutePosition);
					registry.emplace_or_replace<Graphics::SynchronizationState<Scenes::Draws::SceneDraw>>(meshDrawEntity, engineState.getFrameCount());
				}
					  });
		}
	}
	void MeshSystem::ProcessShadowDirtyDraws() {
		auto dirtyPointLightView = registry.view<
			Lights::Components::PointLight,
			Objects::Dirty<Spatials::Components::Spatial>,
			Spatials::Components::Spatial,
			Components::ShadowMeshDrawCollection
		>();
		/*dirtyPointLightView.each([&](
			entt::entity pointlightEntity,
			Lights::Components::PointLight& pointLight,
			Objects::Dirty<Spatials::Components::Spatial>& dirtySpatial,
			Spatials::Components::Spatial& lightSpatial,
			const Components::ShadowMeshDrawCollection& shadowMeshDrawCollection
			) {
				for (const auto& shadowMeshDrawEntity : shadowMeshDrawCollection.meshDrawEntities) {
					auto& shadowMeshDraw = registry.get<Components::ShadowMeshDraw>(shadowMeshDrawEntity);
					registry.emplace_or_replace<Graphics::SynchronizationState<Scenes::Draws::ShadowSceneDraw>>(shadowMeshDrawEntity, engineState.getFrameCount());
				}
			});*/
	}
	Draws::DrawVertexBufferInfo MeshSystem::GetVertexBufferInfo(entt::entity drawEntity) {
		auto meshDraw = registry.get<Components::MeshDraw>(drawEntity);
		Draws::DrawVertexBufferInfo bufferInfo{
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
			.materialEntity = Materials::MaterialSystem::copyMaterialEntity(source, destination, mesh.materialEntity)
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
	MeshGroup
		MeshSystem::copyMeshGroup(
			const entt::registry& source,
			entt::registry& destination,
			const MeshGroup& sourceMeshGroup
		) {
		MeshGroup destinationMeshGroup{
			.meshEntities = std::vector<entt::entity>(sourceMeshGroup.meshEntities.size())
		};
		std::transform(
			sourceMeshGroup.meshEntities.begin(),
			sourceMeshGroup.meshEntities.end(),
			destinationMeshGroup.meshEntities.data(),
			[&source, &destination](entt::entity sourceMeshEntity) {
				return copyMeshEntity(source, destination, sourceMeshEntity);
			}
		);

		return destinationMeshGroup;
	}

	void MeshSystem::ProcessObjectEntity(
		entt::entity objectEntity,
		Stores::StoreItem<Objects::Models::Object>& objectStoreItem,
		const MeshGroup& meshGroup,
		const Spatials::Components::Spatial& spatial,
		const Cameras::Components::Camera& camera,
		const Stores::StoreItemLocation<Cameras::Models::Camera>& cameraStoreItemLocation,
		entt::entity lightEntity,
		entt::entity lightPerspectiveEntity,
		const Lights::Components::LightPerspective& perspective,
		const Stores::StoreItemLocation<Lights::Models::LightPerspective>& perspectiveStoreItemLocation,
		const Stores::StoreItemLocation<Spatials::Models::Spatial>& perspectiveSpatialStoreItemLocation,
		Components::ShadowMeshDrawCollection& shadowMeshDrawCollection
	) {
		const auto& objectStoreItemLocation = objectStoreItem.frameStoreItems[engineState.getFrameIndex()];
		for (const auto& meshEntity : meshGroup.meshEntities) {
			auto& mesh = registry.get<Components::Mesh>(meshEntity);
			auto meshResource = registry.get<std::shared_ptr<Meshes::Components::MeshResource>>(meshEntity);
			auto& material = registry.get<std::shared_ptr<Materials::Components::Material>>(mesh.materialEntity);
			const Meshes::Components::MeshBufferView& meshBufferView = registry.get<Meshes::Components::MeshBufferView>(
				meshEntity
			);
			const Stores::StoreItem<Meshes::Models::Mesh> meshStoreItem = registry.get<Stores::StoreItem<Meshes::Models::Mesh>>(
				meshEntity
			);
			const auto& meshStoreItemLocation = meshStoreItem.frameStoreItems[engineState.getFrameIndex()];

			Scenes::Draws::ShadowSceneDraw draw = {
				.drawSystem = this,
				.pipelineTypeIndex = std::type_index(typeid(Pipelines::ShadowMeshPipeline)),
				.indexBufferView = meshBufferView.IndexBufferView,
				.vertexBufferView = meshBufferView.VertexBufferView,
				.lightEntity = lightEntity,
				.scissor = perspective.shadowMapRect,
				.lightPerspectiveEntity = lightPerspectiveEntity,
				.hasTransparency = material->hasTransparency,
				.depth = glm::distance(camera.absolutePosition, spatial.absolutePosition),
			};
			Components::ShadowMeshDraw meshDraw = {
				.meshResource = meshResource,
				.meshBufferView = meshBufferView,
				.meshItemLocation = meshStoreItemLocation,
				.objectItemLocation = objectStoreItemLocation,
				.cameraItemLocation = cameraStoreItemLocation,
				.lightPerspectiveItemLocation = perspectiveStoreItemLocation,
				.lightPerspectiveSpatialItemLocation = perspectiveSpatialStoreItemLocation
			};
			auto entity = registry.create();
			shadowMeshDrawCollection.meshDrawEntities.push_back(entity);
			registry.emplace_or_replace<Scenes::Draws::ShadowSceneDraw>(entity, std::move(draw));
			registry.emplace_or_replace<Components::ShadowMeshDraw>(entity, std::move(meshDraw));
			registry.emplace_or_replace<Graphics::SynchronizationState<Scenes::Draws::ShadowSceneDraw>>(entity, engineState.getFrameCount());
		}
	}
}
