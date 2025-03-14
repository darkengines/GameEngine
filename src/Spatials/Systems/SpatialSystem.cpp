#include "SpatialSystem.hpp"

#include "../../Common/Components/Dirty.hpp"
#include "../../Common/Components/Name.hpp"
#include "../../Nodes/Components/NodeMeshCollection.hpp"
#include "../Models/RelativeSpatial.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <fmt/format.h>

#include <entt/entt.hpp>
#include <string>

#include "glm/gtx/quaternion.hpp"

namespace drk::Spatials::Systems
{

  SpatialSystem::SpatialSystem(const Devices::DeviceContext& deviceContext, Engine::EngineState& engineState, entt::registry& registry)
      : StorageSystem(engineState, registry),
        deviceContext(deviceContext)
  {
    const auto& nodeMeshView = registry.view<Nodes::Components::NodeMeshCollection>();
    addSpatialSystem(registry);
  }
  void SpatialSystem::addSpatialSystem(entt::registry& registry)
  {
    registry.on_construct<Components::Spatial<Components::Relative>>().connect<SpatialSystem::OnSpatialConstruct>();
  }
  void SpatialSystem::RemoveSpatialSystem(entt::registry& registry)
  {
    registry.on_construct<Components::Spatial<Components::Relative>>().disconnect<SpatialSystem::OnSpatialConstruct>();
  }
  void SpatialSystem::OnSpatialConstruct(entt::registry& registry, entt::entity spatialEntity)
  {
    registry.emplace<Common::Components::Dirty<Components::Spatial<Components::Relative>>>(spatialEntity);
  }
  void SpatialSystem::update(Models::Spatial& spatialModel, const Components::Spatial<Components::Absolute>& spatial)
  {
    spatialModel.position = spatial.position;
    spatialModel.rotation = spatial.rotation;
    spatialModel.scale = spatial.scale;
    spatialModel.model = spatial.model;
  }

  void SpatialSystem::makeDirty(entt::registry& registry, const entt::entity spatialEntity)
  {
    auto& relationship = registry.get<Nodes::Components::Node>(spatialEntity);
    registry.emplace_or_replace<Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Relative>>>(spatialEntity);
    for (const auto& child : relationship.children)
    {
      makeDirty(registry, child);
    }
  }

  bool SpatialSystem::IsParent(entt::entity left, entt::entity right)
  {
    if (right == entt::null)
      return false;
    const auto& relationship = registry.get<Nodes::Components::Node>(right);
    return relationship.parent == left || IsParent(left, relationship.parent);
  }

  uint32_t SpatialSystem::GetDepth(const entt::registry& registry, entt::entity entity)
  {
    const auto& relationship = registry.get<Nodes::Components::Node>(entity);
    if (relationship.parent == entt::null)
      return 0;
    return GetDepth(registry, relationship.parent) + 1;
  }

  std::string SpatialSystem::GetPath(entt::entity entity)
  {
    const auto& relationship = registry.get<Nodes::Components::Node>(entity);
    const auto& nameComponent = registry.get<Common::Components::Name>(entity);
    if (relationship.parent == entt::null)
      return nameComponent.name;
    return fmt::format("{0}-->{1}", GetPath(relationship.parent), nameComponent.name);
  }

  void SpatialSystem::PropagateChanges()
  {
    registry.sort<Common::Components::Dirty<Components::Spatial<Components::Relative>>>(
        [&](entt::entity left, entt::entity& right) { return GetDepth(registry, left) < GetDepth(registry, right); });
    registry.view<Common::Components::Dirty<Components::Spatial<Components::Relative>>, Nodes::Components::Node>().each(
        [&](entt::entity entity, const Nodes::Components::Node& relationship)
        {
          const Nodes::Components::NodeMeshCollection* objectMeshCollection = registry.try_get<Nodes::Components::NodeMeshCollection>(entity);
          if (objectMeshCollection != nullptr)
          {
            for (entt::entity objectMeshEntity : objectMeshCollection->nodeMeshes)
            {
              registry.emplace_or_replace<Common::Components::Dirty<Components::Spatial<Components::Relative>>>(objectMeshEntity);
            }
          }
          // std::cout << fmt::format("{0} {1}", GetDepth(entity), object.Name) << std::endl;

          auto& spatial = registry.get<Components::Spatial<Components::Relative>>(entity);
          auto& absoluteSpatial = registry.get_or_emplace<Components::Spatial<Components::Absolute>>(entity);

          auto translationMatrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3(spatial.position));
          auto rotationMatrix = glm::toMat4(spatial.rotation);
          auto scalingMatrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3(spatial.scale));
          spatial.model = translationMatrix * rotationMatrix * scalingMatrix;

          if (relationship.parent != entt::null)
          {
            auto& parentSpatial = registry.get<Components::Spatial<Components::Absolute>>(relationship.parent);
            absoluteSpatial.scale = parentSpatial.scale * spatial.scale;
            absoluteSpatial.rotation = parentSpatial.rotation * spatial.rotation;
            absoluteSpatial.position = parentSpatial.position + parentSpatial.rotation * (parentSpatial.scale * spatial.position);
            absoluteSpatial.model = parentSpatial.model * spatial.model;
          }
          else
          {
            absoluteSpatial.scale = spatial.scale;
            absoluteSpatial.rotation = spatial.rotation;
            absoluteSpatial.position = spatial.position;
            absoluteSpatial.model = spatial.model;
          }
          registry.emplace_or_replace<Common::Components::Dirty<Components::Spatial<Components::Absolute>>>(entity);
          registry.emplace_or_replace<Graphics::SynchronizationState<Models::Spatial>>(entity, (uint32_t)engineState.getFrameCount());
          registry.emplace_or_replace<Graphics::SynchronizationState<Models::RelativeSpatial>>(entity, (uint32_t)engineState.getFrameCount());
        });
  }
}  // namespace drk::Spatials::Systems
