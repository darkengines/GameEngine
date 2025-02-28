#pragma once

#include <entt/entt.hpp>

#include "../../Cameras/Components/Camera.hpp"
#include "../../Draws/Systems/IDrawSystem.hpp"
#include "../../Engine/EngineState.hpp"
#include "../../Lights/Components/LightPerspective.hpp"
#include "../../Meshes/Components/MeshBufferView.hpp"
#include "../../Spatials/Components/Spatial.hpp"

namespace drk::Meshes::Systems
{
  class MeshShadowSystem : public Draws::Systems::IDrawSystem
  {
   protected:
    entt::registry& registry;
    Engine::EngineState& engineState;

   public:
    MeshShadowSystem(entt::registry& registry, Engine::EngineState& engineState);
    void updateDraw(entt::entity drawEntity, int drawIndex) override;
    void emitDraws() override;

   protected:
    entt::entity ProcessObjectEntity(entt::entity objectMeshEntity,
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
        entt::entity lightPerspectiveSpatialEntity);
  };
}  // namespace drk::Meshes::Systems
