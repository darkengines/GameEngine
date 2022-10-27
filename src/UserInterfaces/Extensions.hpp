#pragma once
#include <boost/di.hpp>
#include "UserInterface.hpp"

namespace drk::UserInterfaces {
	auto AddUserInterfaces() {
		return boost::di::make_injector(
			boost::di::bind<UserInterface>.to<UserInterface>()
		);
	}
}