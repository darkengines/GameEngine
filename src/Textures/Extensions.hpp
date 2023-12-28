#pragma once

#include <boost/di.hpp>
#include "Systems/TextureSystem.hpp"

namespace drk::Textures {
	auto AddTextures() {
		return boost::di::make_injector(
			boost::di::bind<Systems::TextureSystem>.to<Systems::TextureSystem>()
		);
	}
}