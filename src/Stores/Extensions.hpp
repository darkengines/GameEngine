#pragma once
#include <boost/di.hpp>

namespace drk::Stores {
auto addStores() {
	return fruit::createComponent();
}
auto AddStores() {
	return boost::di::make_injector();
}
}  // namespace drk::Stores