#pragma once
#include <boost/di.hpp>
#include "../Engine/EngineState.hpp"
#include "DescriptorSetLayouts.hpp"

namespace drk::Engine {

	auto AddEngine() {
		return boost::di::make_injector(
			boost::di::bind<EngineState>.to<EngineState>(),
			boost::di::bind<DescriptorSetLayoutCache>.to<DescriptorSetLayoutCache>(),
			boost::di::bind<DescriptorSetAllocator>.to<DescriptorSetAllocator>(),
			boost::di::bind<DescriptorSetLayouts>.to<DescriptorSetLayouts>()
		);
	}
}