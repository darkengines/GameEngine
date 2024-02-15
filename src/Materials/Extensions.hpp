#pragma once

#include <boost/di.hpp>
#include "Systems/MaterialSystem.hpp"

namespace drk::Materials {
	auto AddMaterials() {
		return boost::di::make_injector(
			boost::di::bind<Systems::MaterialSystem>.to<Systems::MaterialSystem>()
		);
	}
}