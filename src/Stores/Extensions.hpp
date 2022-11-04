#pragma once
#include <boost/di.hpp>

namespace drk::Stores {

	auto AddStores() {
		return boost::di::make_injector();
	}
}