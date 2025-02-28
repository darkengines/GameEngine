#include "SceneSystem.hpp"

#include <map>

#include "../Draws/SceneDraw.hpp"
#include "../Draws/ShadowSceneDraw.hpp"

namespace drk::Scenes::Systems
{
  SceneSystem::SceneSystem(entt::registry& registry, Engine::EngineState& engineState)
      : registry(registry),
        engineState(engineState)
  {
    auto synchronizables = registry.view<Graphics::SynchronizationState<Draws::SceneDraw>>();
  }
  void SceneSystem::updateDraws()
  {
    auto synchronizables = registry.view<Graphics::SynchronizationState<Draws::SceneDraw>>();
    if (synchronizables.begin() == synchronizables.end())
      return;
    registry.sort<Scenes::Draws::SceneDraw>(
        [](const Draws::SceneDraw& leftDraw, const Draws::SceneDraw& rightDraw)
        {
          if (leftDraw.hasTransparency < rightDraw.hasTransparency)
            return true;
          if (leftDraw.hasTransparency > rightDraw.hasTransparency)
            return false;

          if (leftDraw.hasTransparency && rightDraw.hasTransparency)
          {
            if (leftDraw.depth < rightDraw.depth)
              return false;
            if (leftDraw.depth > rightDraw.depth)
              return true;
          }
          else
          {
            if (leftDraw.depth > rightDraw.depth)
              return false;
            if (leftDraw.depth < rightDraw.depth)
              return true;
          }

          if (leftDraw.drawSystem < rightDraw.drawSystem)
            return true;
          if (leftDraw.drawSystem > rightDraw.drawSystem)
            return false;

          if (leftDraw.pipelineTypeIndex < rightDraw.pipelineTypeIndex)
            return true;
          if (leftDraw.pipelineTypeIndex > rightDraw.pipelineTypeIndex)
            return false;

          if (leftDraw.indexBufferView.byteOffset < rightDraw.indexBufferView.byteOffset)
            return true;

          return false;
        });

    auto sceneDrawEntities = registry.view<Scenes::Draws::SceneDraw>();

    // std::cout << "----------------------" << std::endl;
    std::map<std::type_index, int> pipelineDrawIndices;
    sceneDrawEntities.each(
        [&pipelineDrawIndices, this](entt::entity sceneDrawEntity, Draws::SceneDraw& draw)
        {
          if (!pipelineDrawIndices.contains(draw.pipelineTypeIndex))
          {
            pipelineDrawIndices[draw.pipelineTypeIndex] = 0;
          }
          draw.drawSystem->updateDraw(sceneDrawEntity, pipelineDrawIndices[draw.pipelineTypeIndex]);
          if (registry.any_of<Graphics::SynchronizationState<Draws::SceneDraw>>(sceneDrawEntity))
          {
            auto& synchronizationState = registry.get<Graphics::SynchronizationState<Draws::SceneDraw>>(sceneDrawEntity);
            if (!synchronizationState.Update(engineState.getFrameIndex()))
            {
              registry.remove<Graphics::SynchronizationState<Draws::SceneDraw>>(sceneDrawEntity);
            }
          }
          // std::cout << draw.hasTransparency << " " << draw.depth << std::endl;
          pipelineDrawIndices[draw.pipelineTypeIndex]++;
        });
    // std::cout << "----------------------" << std::endl;
  }
  void SceneSystem::updateShadowDraws()
  {
    auto synchronizables = registry.view<Graphics::SynchronizationState<Draws::ShadowSceneDraw>>();
    if (synchronizables.begin() == synchronizables.end())
      return;
    registry.sort<Scenes::Draws::ShadowSceneDraw>(
        [](const Draws::ShadowSceneDraw& leftDraw, const Draws::ShadowSceneDraw& rightDraw)
        {
          if (leftDraw.hasTransparency < rightDraw.hasTransparency)
            return true;
          if (leftDraw.hasTransparency > rightDraw.hasTransparency)
            return false;

          if (leftDraw.hasTransparency && rightDraw.hasTransparency)
          {
            if (leftDraw.depth < rightDraw.depth)
              return false;
            if (leftDraw.depth > rightDraw.depth)
              return true;
          }
          if (leftDraw.drawSystem < rightDraw.drawSystem)
            return true;
          if (leftDraw.drawSystem > rightDraw.drawSystem)
            return false;

          if (leftDraw.lightPerspectiveEntity < rightDraw.lightPerspectiveEntity)
            return true;
          if (leftDraw.lightPerspectiveEntity > rightDraw.lightPerspectiveEntity)
            return false;

          if (leftDraw.pipelineTypeIndex < rightDraw.pipelineTypeIndex)
            return true;
          if (leftDraw.pipelineTypeIndex > rightDraw.pipelineTypeIndex)
            return false;

          if (leftDraw.indexBufferView.byteOffset < rightDraw.indexBufferView.byteOffset)
            return true;

          return false;
        });

    auto sceneDrawEntities = registry.view<Scenes::Draws::ShadowSceneDraw>();

    // std::cout << "----------------------" << std::endl;
    std::map<std::type_index, int> pipelineDrawIndices;
    sceneDrawEntities.each(
        [&pipelineDrawIndices, this](entt::entity shadowSceneDrawEntity, Draws::ShadowSceneDraw& shadowSceneDraw)
        {
          if (!pipelineDrawIndices.contains(shadowSceneDraw.pipelineTypeIndex))
          {
            pipelineDrawIndices[shadowSceneDraw.pipelineTypeIndex] = 0;
          }
          shadowSceneDraw.drawSystem->updateDraw(
              shadowSceneDrawEntity, pipelineDrawIndices[shadowSceneDraw.pipelineTypeIndex]);
          if (registry.any_of<Graphics::SynchronizationState<Draws::ShadowSceneDraw>>(shadowSceneDrawEntity))
          {
            auto& synchronizationState =
                registry.get<Graphics::SynchronizationState<Draws::ShadowSceneDraw>>(shadowSceneDrawEntity);
            if (!synchronizationState.Update(engineState.getFrameIndex()))
            {
              registry.remove<Graphics::SynchronizationState<Draws::ShadowSceneDraw>>(shadowSceneDrawEntity);
            }
          }
          // std::cout << draw.hasTransparency << " " << draw.depth << std::endl;
          pipelineDrawIndices[shadowSceneDraw.pipelineTypeIndex]++;
        });
    // std::cout << "----------------------" << std::endl;
  }
}  // namespace drk::Scenes::Systems
