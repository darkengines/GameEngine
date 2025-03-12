#pragma once
#include <boost/di.hpp>

#include "AssetExplorer.hpp"
#include "Renderers/UserInterfaceRenderer.hpp"
#include "UserInterface.hpp"
#include <entt/entt.hpp>

namespace drk::UserInterfaces
{
  fruit::Component<UserInterface, AssetExplorer, Renderers::UserInterfaceRenderer> addUserInterfaces()
  {
    return fruit::createComponent()
		  .registerConstructor<UserInterface(Windows::Window & window, Controllers::FlyCamController & flyCamController, entt::registry & registry)>()
        .registerConstructor<AssetExplorer(drk::Loaders::AssimpLoader & assimpLoader)>()
        .registerConstructor<drk::UserInterfaces::Renderers::UserInterfaceRenderer(
            Devices::DeviceContext & deviceContext, Engine::EngineState & engineState, const Windows::Window& window)>()
        .install(Controllers::addControllers)
        .install(Windows::addWindows)
        .install(Devices::addDevices)
        .install(Loaders::addLoaders)
        .install(Engine::addEngine)
		.install(drk::addRegistry);
  }
  auto AddUserInterfaces()
  {
    return boost::di::make_injector(boost::di::bind<UserInterface>.to<UserInterface>(),
        boost::di::bind<AssetExplorer>.to<AssetExplorer>(),
        boost::di::bind<drk::UserInterfaces::Renderers::UserInterfaceRenderer>.to<drk::UserInterfaces::Renderers::UserInterfaceRenderer>());
  }
}  // namespace drk::UserInterfaces
