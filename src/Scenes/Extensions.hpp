#pragma once
#include <boost/di.hpp>
#include "SceneSystem.hpp"
#include "Renderers/SceneRenderer.hpp"

namespace drk::Scenes {
	auto AddScenes() {
		return boost::di::make_injector(
			boost::di::bind<SceneSystem>.to<SceneSystem>(),
			boost::di::bind<Renderers::SceneRenderer>.to<Renderers::SceneRenderer>()
		);
	}
}