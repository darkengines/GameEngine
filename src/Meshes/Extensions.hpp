#pragma once

#include <boost/di.hpp>
#include "Systems/MeshSystem.hpp"
#include "Systems/MeshShadowSystem.hpp"
#include "Pipelines/MeshPipeline.hpp"
#include "Pipelines/ShadowMeshPipeline.hpp"
#include "Resources/MeshResourceManager.hpp"

namespace drk::Meshes {
	auto AddMeshes() {
		return boost::di::make_injector(
			boost::di::bind<Resources::MeshResourceManager>.to<Resources::MeshResourceManager>(),
			boost::di::bind<Systems::MeshSystem>.to<Systems::MeshSystem>(),
			boost::di::bind<Systems::MeshShadowSystem>.to<Systems::MeshShadowSystem>(),
			boost::di::bind<Pipelines::MeshPipeline>.to<Pipelines::MeshPipeline>().in(boost::di::unique),
			boost::di::bind<Pipelines::ShadowMeshPipeline>.to<Pipelines::ShadowMeshPipeline>().in(boost::di::unique)
		);
	}
}