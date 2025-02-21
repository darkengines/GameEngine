#pragma once

#include <boost/di.hpp>

#include "AssimpLoader.hpp"

namespace drk::Loaders {
fruit::Component<AssimpLoader> addLoaders() {
	return fruit::createComponent()
		.registerConstructor<AssimpLoader()>();
}
auto AddLoaders() {
	return boost::di::make_injector(
		boost::di::bind<AssimpLoader>.to<AssimpLoader>()
	);
}
}  // namespace drk::Loaders