#pragma once

#include <boost/di.hpp>

#include "../Buffers/Extensions.hpp"
#include "../Draws/Systems/IDrawSystem.hpp"
#include "../Graphics/Extentions.hpp"
#include "Pipelines/MeshPipeline.hpp"
#include "Pipelines/ShadowMeshPipeline.hpp"
#include "Resources/MeshResourceManager.hpp"
#include "Systems/MeshShadowSystem.hpp"
#include "Systems/MeshSystem.hpp"

namespace drk::Meshes
{
  fruit::Component<Systems::MeshSystem,
      Systems::MeshShadowSystem,
      std::function<std::unique_ptr<Meshes::Pipelines::MeshPipeline>()>,
      std::function<std::unique_ptr<Meshes::Pipelines::ShadowMeshPipeline>()>>
  addMeshes()
  {
    return fruit::createComponent()
        .registerConstructor<Resources::MeshResourceManager(
            Buffers::Resources::BufferResourceManager & bufferResourceManager)>()
        .registerConstructor<Systems::MeshSystem(const Devices::DeviceContext& deviceContext,
            Engine::EngineState& engineState,
            entt::registry& registry,
            Graphics::GlobalSystem& globalSystem,
            Resources::MeshResourceManager& meshResourceManager)>()
        .registerConstructor<Systems::MeshShadowSystem(entt::registry & registry, Engine::EngineState & engineState)>()
        .registerFactory<std::unique_ptr<Meshes::Pipelines::MeshPipeline>(const Devices::DeviceContext& deviceContext,
            Engine::EngineState& engineState,
            const Engine::DescriptorSetLayouts& descriptorSetLayouts)>(
            [](const Devices::DeviceContext& deviceContext,
                Engine::EngineState& engineState,
                const Engine::DescriptorSetLayouts& descriptorSetLayouts)
            { return std::make_unique<Meshes::Pipelines::MeshPipeline>(deviceContext, engineState, descriptorSetLayouts); })
        .registerFactory<std::unique_ptr<Pipelines::ShadowMeshPipeline>(const Devices::DeviceContext& deviceContext,
            Engine::EngineState& engineState,
            const Engine::DescriptorSetLayouts& descriptorSetLayouts)>(
            [](const Devices::DeviceContext& deviceContext,
                Engine::EngineState& engineState,
                const Engine::DescriptorSetLayouts& descriptorSetLayouts)
            { return std::make_unique<Pipelines::ShadowMeshPipeline>(deviceContext, engineState, descriptorSetLayouts); })
        .install(Devices::addDevices)
        .install(Engine::addEngine)
        .install(drk::addRegistry)
        .install(Buffers::addBuffers)
        .install(Graphics::addGraphics)
        .addMultibinding<drk::Systems::IStorageSystem, Systems::MeshSystem>()
        .addMultibinding<drk::Draws::Systems::IDrawSystem, Systems::MeshSystem>()
        .addMultibinding<drk::Draws::Systems::IDrawSystem, Systems::MeshShadowSystem>();
  }
  auto AddMeshes()
  {
    return boost::di::make_injector(boost::di::bind<Resources::MeshResourceManager>.to<Resources::MeshResourceManager>(),
        boost::di::bind<Systems::MeshSystem>.to<Systems::MeshSystem>(),
        boost::di::bind<Systems::MeshShadowSystem>.to<Systems::MeshShadowSystem>(),
        boost::di::bind<Pipelines::MeshPipeline>.to<Pipelines::MeshPipeline>().in(boost::di::unique),
        boost::di::bind<Pipelines::ShadowMeshPipeline>.to<Pipelines::ShadowMeshPipeline>().in(boost::di::unique));
  }
}  // namespace drk::Meshes
