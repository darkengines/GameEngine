#pragma once
#include <boost/di.hpp>
#include "Application.hpp"

namespace drk::Applications {
	auto AddApplications() {
		return boost::di::make_injector(
			boost::di::bind<Application>.to<Application>()
		);
	}
}