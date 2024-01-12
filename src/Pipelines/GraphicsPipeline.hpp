#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vector>
#include <memory>
#include <functional>
#include "../Draws/Components/DrawVertexBufferInfo.hpp"
#include <entt/entity/fwd.hpp>

namespace drk::Pipelines {
	class GraphicsPipeline {
	public:
		virtual void bind(const vk::CommandBuffer& commandBuffer) = 0;
		virtual void destroyPipeline() = 0;
		virtual void configure(std::function<void(vk::GraphicsPipelineCreateInfo&)> configure) = 0;
		virtual Draws::Components::DrawVertexBufferInfo getBufferInfo(const entt::registry& registry, entt::entity drawEntity) const = 0;
	};
}


