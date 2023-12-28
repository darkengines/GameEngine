#pragma once

#include <boost/di.hpp>
#include "Systems/CameraSystem.hpp"

namespace drk::Cameras {
	auto AddCameras() {
		return boost::di::make_injector(
			boost::di::bind<Systems::CameraSystem>.to<Systems::CameraSystem>()
		);
	}
}