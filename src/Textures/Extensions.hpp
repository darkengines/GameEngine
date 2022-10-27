#pragma once

#include <boost/di.hpp>
#include "TextureSystem.hpp"
namespace drk::Textures {
	auto AddTextures() {
		return boost::di::make_injector(
			boost::di::bind<TextureSystem>.to<TextureSystem>()
		);
	}
}