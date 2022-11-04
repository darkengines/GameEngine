#pragma once
#include <boost/di.hpp>
#include "../Engine/DescriptorSetLayoutCache.hpp"

namespace drk::Draws {
	auto AddDraws() {
		return boost::di::make_injector();
	}
}