#include "MeshShadowSystem.hpp"

#include "../../Animations/Components/SkinnedBufferView.hpp"
#include "../../Cameras/Models/Camera.hpp"
#include "../../Lights/Components/DirectionalLight.hpp"
#include "../../Lights/Components/LightPerspectiveCollection.hpp"
#include "../../Lights/Components/PointLight.hpp"
#include "../../Lights/Components/Spotlight.hpp"
#include "../../Lights/Models/DirectionalLight.hpp"
#include "../../Lights/Models/LightPerspective.hpp"
#include "../../Materials/Components/MaterialReference.hpp"
#include "../../Nodes/Components/NodeMesh.hpp"
#include "../../Nodes/Models/Node.hpp"
#include "../../Scenes/Draws/ShadowSceneDraw.hpp"
#include "../../Spatials/Models/Spatial.hpp"
#include "../../Stores/StoreItemLocation.hpp"
#include "../Components/HasShadowDraw.hpp"
#include "../Components/Mesh.hpp"
#include "../Components/ShadowMeshDraw.hpp"
#include "../Models/Mesh.hpp"
#include "../Models/ShadowMeshDraw.hpp"
#include "../Pipelines/ShadowMeshPipeline.hpp"

namespace drk::Meshes::Systems
{
  MeshShadowSystem::MeshShadowSystem(entt::registry& registry, Engine::EngineState& engineState)
      : registry(registry),
        engineState(engineState)
  {
    auto objectMeshEntities =
        registry.view<Nodes::Components::NodeMesh, Meshes::Components::Mesh>(entt::exclude<Components::HasShadowDraw>);
    auto pointLightView = registry.view<Lights::Components::PointLight, Lights::Components::LightPerspectiveCollection>();
    auto directionalLightView = registry.view<Lights::Components::DirectionalLight, Lights::Components::LightPerspective>();
    auto spotlightView = registry.view<Lights::Components::Spotlight, Lights::Components::LightPerspective>();
  }
  void MeshShadowSystem::updateDraw(entt::entity shadowDrawEntity, int drawIndex)
  {
    const auto& meshDraw = registry.get<Components::ShadowMeshDraw>(shadowDrawEntity);
    auto& frameState = engineState.getCurrentFrameState();
    // todo: optimization - fetch uniform store in parent scope and give as argument
    auto& drawStore = frameState.getUniformStore<Models::ShadowMeshDraw>();
    const auto& meshItemLocation = drawStore.get(drawIndex);

    auto& meshStoreItem = registry.get<Stores::StoreItem<Models::Mesh>>(meshDraw.meshEntity);
    auto& objectStoreItem = registry.get<Stores::StoreItem<Nodes::Models::Node>>(meshDraw.nodeEntity);
    auto& cameraStoreItem = registry.get<Stores::StoreItem<Cameras::Models::Camera>>(engineState.cameraEntity);
    auto& lightPerspectiveStoreItem =
        registry.get<Stores::StoreItem<Lights::Models::LightPerspective>>(meshDraw.lightPerspectiveEntity);
    auto& spatialLightPerspectiveStoreItem =
        registry.get<Stores::StoreItem<Spatials::Models::Spatial>>(meshDraw.lightPerspectiveSpatialEntity);

    auto frameIndex = engineState.getFrameIndex();
    meshItemLocation.pItem->meshItemLocation = meshStoreItem.frameStoreItems[frameIndex];
    meshItemLocation.pItem->objectItemLocation = objectStoreItem.frameStoreItems[frameIndex];
    meshItemLocation.pItem->cameraItemLocation = cameraStoreItem.frameStoreItems[frameIndex];
    meshItemLocation.pItem->LightPerspectiveItemLocation = lightPerspectiveStoreItem.frameStoreItems[frameIndex];
    meshItemLocation.pItem->LightPerspectiveSpatialItemLocation =
        spatialLightPerspectiveStoreItem.frameStoreItems[frameIndex];
  }

  void MeshShadowSystem::emitDraws()
  {
    const auto& [camera, cameraStoreItem, cameraSpatial] = registry.get<Cameras::Components::Camera,
        Stores::StoreItem<Cameras::Models::Camera>,
        Spatials::Components::Spatial<Spatials::Components::Absolute>>(engineState.cameraEntity);
    const auto& cameraStoreItemLocation = cameraStoreItem.frameStoreItems[engineState.getFrameIndex()];
    auto objectMeshEntities = registry.view<Nodes::Components::NodeMesh, Meshes::Components::Mesh>();
    auto freshObjectMeshEntities =
        registry.view<Nodes::Components::NodeMesh, Meshes::Components::Mesh>(entt::exclude<Components::HasShadowDraw>);
    auto freshPointLightView = registry.view<Lights::Components::PointLight, Lights::Components::LightPerspectiveCollection>(
        entt::exclude<Components::HasShadowDraw>);
    auto freshDirectionalLightView =
        registry.view<Lights::Components::DirectionalLight, Lights::Components::LightPerspective>(
            entt::exclude<Components::HasShadowDraw>);
    auto freshSpotlightView = registry.view<Lights::Components::Spotlight, Lights::Components::LightPerspective>(
        entt::exclude<Components::HasShadowDraw>);

    auto pointLightView = registry.view<Lights::Components::PointLight, Lights::Components::LightPerspectiveCollection>();
    auto directionalLightView = registry.view<Lights::Components::DirectionalLight, Lights::Components::LightPerspective>();
    auto spotlightView = registry.view<Lights::Components::Spotlight, Lights::Components::LightPerspective>();

    freshObjectMeshEntities.each(
        [&](entt::entity objectMeshEntity, const Nodes::Components::NodeMesh& nodeMesh, const Meshes::Components::Mesh& mesh)
        {
          const auto& [materialReference, meshResource, meshBufferView] =
              registry.get<Materials::Components::MaterialReference,
                  Meshes::Components::MeshResource,
                  Meshes::Components::MeshBufferView>(nodeMesh.meshEntity);
          const auto& spatial =
              registry.get<Spatials::Components::Spatial<Spatials::Components::Absolute>>(nodeMesh.nodeEntity);
          const auto& material = registry.get<Materials::Components::Material>(materialReference.materialEntity);
          auto currentFrameIndex = engineState.getFrameIndex();
          pointLightView.each(
              [&](entt::entity pointLightEntity,
                  const Lights::Components::PointLight& pointLight,
                  const Lights::Components::LightPerspectiveCollection& lightPerspectiveCollection)
              {
                for (const auto& lightPerspectiveEntity : lightPerspectiveCollection.lightPerspectives)
                {
                  const auto& lightPerspective = registry.get<Lights::Components::LightPerspective>(lightPerspectiveEntity);
                  ProcessObjectEntity(objectMeshEntity,
                      pointLightEntity,
                      lightPerspectiveEntity,
                      lightPerspective,
                      camera,
                      cameraSpatial,
                      spatial,
                      material,
                      meshBufferView,
                      meshResource,
                      nodeMesh.nodeEntity,
                      nodeMesh.meshEntity,
                      engineState.cameraEntity,
                      pointLightEntity);
                }
                registry.emplace_or_replace<Components::HasShadowDraw>(pointLightEntity);
              });

          directionalLightView.each(
              [&](entt::entity directionalLightEntity,
                  const Lights::Components::DirectionalLight& directionalLight,
                  const Lights::Components::LightPerspective& lightPerspective)
              {
                ProcessObjectEntity(objectMeshEntity,
                    directionalLightEntity,
                    directionalLightEntity,
                    lightPerspective,
                    camera,
                    cameraSpatial,
                    spatial,
                    material,
                    meshBufferView,
                    meshResource,
                    nodeMesh.nodeEntity,
                    nodeMesh.meshEntity,
                    engineState.cameraEntity,
                    directionalLightEntity);
                registry.emplace_or_replace<Components::HasShadowDraw>(directionalLightEntity);
              });

          spotlightView.each(
              [&](entt::entity spotlightEntity,
                  const Lights::Components::Spotlight& spotlight,
                  const Lights::Components::LightPerspective& lightPerspective)
              {
                ProcessObjectEntity(objectMeshEntity,
                    spotlightEntity,
                    spotlightEntity,
                    lightPerspective,
                    camera,
                    cameraSpatial,
                    spatial,
                    material,
                    meshBufferView,
                    meshResource,
                    nodeMesh.nodeEntity,
                    nodeMesh.meshEntity,
                    engineState.cameraEntity,
                    spotlightEntity);
                registry.emplace_or_replace<Components::HasShadowDraw>(spotlightEntity);
              });
          registry.emplace<Components::HasShadowDraw>(objectMeshEntity);
        });

    freshPointLightView.each(
        [&](entt::entity pointLightEntity,
            const Lights::Components::PointLight& pointLight,
            const Lights::Components::LightPerspectiveCollection& lightPerspectiveCollection)
        {
          for (const auto& lightPerspectiveEntity : lightPerspectiveCollection.lightPerspectives)
          {
            objectMeshEntities.each(
                [&](entt::entity objectMeshEntity,
                    const Nodes::Components::NodeMesh& nodeMesh,
                    const Meshes::Components::Mesh& mesh)
                {
                  const auto& [materialReference, meshResource, meshBufferView] =
                      registry.get<Materials::Components::MaterialReference,
                          Meshes::Components::MeshResource,
                          Meshes::Components::MeshBufferView>(nodeMesh.meshEntity);
                  const auto& spatial =
                      registry.get<Spatials::Components::Spatial<Spatials::Components::Absolute>>(nodeMesh.nodeEntity);
                  const auto& material = registry.get<Materials::Components::Material>(materialReference.materialEntity);
                  auto currentFrameIndex = engineState.getFrameIndex();
                  const auto& lightPerspective = registry.get<Lights::Components::LightPerspective>(lightPerspectiveEntity);
                  ProcessObjectEntity(objectMeshEntity,
                      pointLightEntity,
                      lightPerspectiveEntity,
                      lightPerspective,
                      camera,
                      cameraSpatial,
                      spatial,
                      material,
                      meshBufferView,
                      meshResource,
                      nodeMesh.nodeEntity,
                      nodeMesh.meshEntity,
                      engineState.cameraEntity,
                      pointLightEntity);
                  registry.emplace_or_replace<Components::HasShadowDraw>(objectMeshEntity);
                });
          }
          registry.emplace<Components::HasShadowDraw>(pointLightEntity);
        });

    freshDirectionalLightView.each(
        [&](entt::entity directionalLightEntity,
            const Lights::Components::DirectionalLight& directionalLight,
            const Lights::Components::LightPerspective& lightPerspective)
        {
          objectMeshEntities.each(
              [&](entt::entity objectMeshEntity,
                  const Nodes::Components::NodeMesh& nodeMesh,
                  const Meshes::Components::Mesh& mesh)
              {
                const auto& [materialReference, meshResource, meshBufferView] =
                    registry.get<Materials::Components::MaterialReference,
                        Meshes::Components::MeshResource,
                        Meshes::Components::MeshBufferView>(nodeMesh.meshEntity);
                const auto& spatial =
                    registry.get<Spatials::Components::Spatial<Spatials::Components::Absolute>>(nodeMesh.nodeEntity);
                const auto& material = registry.get<Materials::Components::Material>(materialReference.materialEntity);
                auto currentFrameIndex = engineState.getFrameIndex();
                ProcessObjectEntity(objectMeshEntity,
                    directionalLightEntity,
                    directionalLightEntity,
                    lightPerspective,
                    camera,
                    cameraSpatial,
                    spatial,
                    material,
                    meshBufferView,
                    meshResource,
                    nodeMesh.nodeEntity,
                    nodeMesh.meshEntity,
                    engineState.cameraEntity,
                    directionalLightEntity);
                registry.emplace<Components::HasShadowDraw>(objectMeshEntity);
              });
          registry.emplace<Components::HasShadowDraw>(directionalLightEntity);
        });

    freshSpotlightView.each(
        [&](entt::entity spotlightEntity,
            const Lights::Components::Spotlight& spotlight,
            const Lights::Components::LightPerspective& lightPerspective)
        {
          objectMeshEntities.each(
              [&](entt::entity objectMeshEntity,
                  const Nodes::Components::NodeMesh& nodeMesh,
                  const Meshes::Components::Mesh& mesh)
              {
                const auto& [materialReference, meshResource, meshBufferView] =
                    registry.get<Materials::Components::MaterialReference,
                        Meshes::Components::MeshResource,
                        Meshes::Components::MeshBufferView>(nodeMesh.meshEntity);
                const auto& spatial =
                    registry.get<Spatials::Components::Spatial<Spatials::Components::Absolute>>(nodeMesh.nodeEntity);
                const auto& material = registry.get<Materials::Components::Material>(materialReference.materialEntity);
                auto currentFrameIndex = engineState.getFrameIndex();
                ProcessObjectEntity(objectMeshEntity,
                    spotlightEntity,
                    spotlightEntity,
                    lightPerspective,
                    camera,
                    cameraSpatial,
                    spatial,
                    material,
                    meshBufferView,
                    meshResource,
                    nodeMesh.nodeEntity,
                    nodeMesh.meshEntity,
                    engineState.cameraEntity,
                    spotlightEntity);
                registry.emplace<Components::HasShadowDraw>(objectMeshEntity);
              });
          registry.emplace<Components::HasShadowDraw>(spotlightEntity);
        });
  }

  entt::entity MeshShadowSystem::ProcessObjectEntity(entt::entity objectMeshEntity,
      entt::entity lightEntity,
      entt::entity lightPerspectiveEntity,
      const Lights::Components::LightPerspective& perspective,
      const Cameras::Components::Camera& camera,
      const Spatials::Components::Spatial<Spatials::Components::Absolute>& cameraSpatial,
      const Spatials::Components::Spatial<Spatials::Components::Absolute>& spatial,
      const Materials::Components::Material& material,
      const Meshes::Components::MeshBufferView& meshBufferView,
      const Meshes::Components::MeshResource& meshResource,
      entt::entity nodeEntity,
      entt::entity meshEntity,
      entt::entity cameraEntity,
      entt::entity lightPerspectiveSpatialEntity)
  {
    auto animationVertexBufferViewPtr = registry.try_get<Animations::Components::SkinnedBufferView>(objectMeshEntity);
    Devices::BufferView const* bufferView = &meshBufferView.VertexBufferView;
    if (animationVertexBufferViewPtr != nullptr)
    {
      bufferView = &animationVertexBufferViewPtr->frameSkinnedBufferViews[engineState.getFrameIndex()];
    }
    Scenes::Draws::ShadowSceneDraw draw = {
      .nodeEntity = nodeEntity,
      .drawSystem = this,
      .pipelineTypeIndex = std::type_index(typeid(Pipelines::ShadowMeshPipeline)),
      .indexBufferView = meshBufferView.IndexBufferView,
      .vertexBufferView = *bufferView,
      .lightEntity = lightEntity,
      .scissor = perspective.shadowMapRect,
      .lightPerspectiveEntity = lightPerspectiveEntity,
      .hasTransparency = material.hasTransparency,
      .depth = glm::distance(cameraSpatial.position, spatial.position),
    };
    Components::ShadowMeshDraw meshDraw = { .indexCount = static_cast<uint32_t>(meshResource.indices.size()),
      .meshBufferView = { meshBufferView.IndexBufferView, *bufferView },
      .meshEntity = meshEntity,
      .nodeEntity = nodeEntity,
      .cameraEntity = cameraEntity,
      .lightPerspectiveEntity = lightPerspectiveEntity,
      .lightPerspectiveSpatialEntity = lightPerspectiveSpatialEntity };

    auto objectMeshPerspectiveEntity = registry.create();
    registry.emplace_or_replace<Scenes::Draws::ShadowSceneDraw>(objectMeshPerspectiveEntity, std::move(draw));
    registry.emplace_or_replace<Components::ShadowMeshDraw>(objectMeshPerspectiveEntity, std::move(meshDraw));
    registry.emplace_or_replace<Graphics::SynchronizationState<Scenes::Draws::ShadowSceneDraw>>(
        objectMeshPerspectiveEntity, engineState.getFrameCount());

    return objectMeshPerspectiveEntity;
  }
}  // namespace drk::Meshes::Systems
