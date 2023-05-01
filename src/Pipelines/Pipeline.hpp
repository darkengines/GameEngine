#pragma once
#include <vector>
#include <memory>

namespace drk::Pipelines {
	class Pipeline {
		virtual void bind(const vk::CommandBuffer& commandBuffer) = 0;
		virtual void destroyPipeline() = 0;
		virtual void configure(std::function<void(vk::GraphicsPipelineCreateInfo&)> configure) = 0;
	};
}


