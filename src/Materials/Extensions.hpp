#pragma once

#include <boost/di.hpp>
#include "MaterialSystem.hpp"
namespace drk::Materials {
	auto AddMaterials() {
		return boost::di::make_injector(
			boost::di::bind<MaterialSystem>.to<MaterialSystem>()
		);
	}
}