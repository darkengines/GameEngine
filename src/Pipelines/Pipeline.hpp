#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vector>
#include <memory>
#include <functional>
#include "../Draws/DrawVertexBufferInfo.hpp"
#include <entt/entity/fwd.hpp>

namespace drk::Pipelines {
	class Pipeline {
	public:
		virtual void bind(const vk::CommandBuffer& commandBuffer) = 0;
		virtual void destroyPipeline() = 0;
		virtual void configure(std::function<void(vk::GraphicsPipelineCreateInfo&)> configure) = 0;
		virtual Draws::DrawVertexBufferInfo getBufferInfo(const entt::registry& registry, entt::entity drawEntity) const = 0;
	};
}


