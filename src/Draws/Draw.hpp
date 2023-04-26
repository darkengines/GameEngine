#pragma once
#include <vulkan/vulkan.hpp>
#include "../Devices/Buffer.hpp"
#include "../Devices/BufferView.hpp"
#include "../Pipelines/Pipeline.hpp"
#include "../Renderers/Renderer.hpp"

namespace drk::Draws {
	struct Draw {
		const Renderers::Renderer const * renderer;
		const Pipelines::Pipeline const * pipeline;
		const Devices::BufferView const * indexBufferView;
		const Devices::BufferView const * vertexBufferView;
		bool hasTransparency;
		float depth;
	};
}