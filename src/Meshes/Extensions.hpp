#pragma once

#include <boost/di.hpp>
#include "MeshSystem.hpp"
#include "Pipelines/MeshPipeline.hpp"
#include "Pipelines/ShadowMeshPipeline.hpp"
namespace drk::Meshes {
	auto AddMeshes() {
		return boost::di::make_injector(
			boost::di::bind<MeshSystem>.to<MeshSystem>(),
			boost::di::bind<Pipelines::MeshPipeline>.to<Pipelines::MeshPipeline>().in(boost::di::unique),
			boost::di::bind<Pipelines::ShadowMeshPipeline>.to<Pipelines::ShadowMeshPipeline>().in(boost::di::unique)
		);
	}
}