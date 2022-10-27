#pragma once

#include <boost/di.hpp>
#include "AssimpLoader.hpp"
namespace drk::Loaders {
	auto AddLoaders() {
		return boost::di::make_injector(
			boost::di::bind<AssimpLoader>.to<AssimpLoader>()
		);
	}
}