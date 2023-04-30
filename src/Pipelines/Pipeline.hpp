#pragma once
#include <vector>
#include <memory>

namespace drk::Pipelines {
	class Pipeline {
		virtual void render(const vk::CommandBuffer& commandBuffer) const = 0;
		virtual void bind(const vk::CommandBuffer& commandBuffer) const = 0;
		virtual void setExtent(const vk::Extent2D& extent) = 0;
		virtual void setRenderPass(const vk::RenderPass& renderPass) = 0;
		virtual void destroyPipeline() = 0;
		virtual void createPipeline() = 0;
	};
}


