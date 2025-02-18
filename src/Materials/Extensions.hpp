#pragma once

#include <fruit/fruit.h>
#include <boost/di.hpp>
#include "Systems/MaterialSystem.hpp"

namespace drk::Materials {
auto AddMaterials() {
	fruit::createComponent()
		.registerConstructor<Systems::MaterialSystem(const drk::Devices::DeviceContext&, entt::registry&, Engine::EngineState&)>()
		.addMultibinding<drk::Systems::StorageSystem, Systems::MaterialSystem>();
	return boost::di::make_injector(boost::di::bind<Systems::MaterialSystem>.to<Systems::MaterialSystem>());
}
}  // namespace drk::Materials