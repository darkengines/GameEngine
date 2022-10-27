#pragma once

#include <boost/di.hpp>
#include "MeshSystem.hpp"
namespace drk::Meshes {
	auto AddMeshes() {
		return boost::di::make_injector(
			boost::di::bind<MeshSystem>.to<MeshSystem>()
		);
	}
}