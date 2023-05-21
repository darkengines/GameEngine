#pragma once
#include <boost/di.hpp>
#include "UserInterface.hpp"
#include "Renderers/UserInterfaceRenderer.hpp"
#include "AssetExplorer.hpp"

namespace drk::UserInterfaces {
	auto AddUserInterfaces() {
		return boost::di::make_injector(
			boost::di::bind<UserInterface>.to<UserInterface>(),
			boost::di::bind<AssetExplorer>.to<AssetExplorer>(),
			boost::di::bind<drk::UserInterfaces::Renderers::UserInterfaceRenderer>.to<drk::UserInterfaces::Renderers::UserInterfaceRenderer>()
		);
	}
}