#pragma once

#include "../../Pipelines/GraphicsPipeline.hpp"
#include "../../Devices/BufferView.hpp"
#include "../../Draws/Systems/IDrawSystem.hpp"
#include <typeindex>

namespace drk::Scenes::Draws {
	struct ShadowSceneDraw {
		entt::entity nodeEntity;
		drk::Draws::Systems::IDrawSystem* drawSystem;
		std::type_index pipelineTypeIndex;
		Devices::BufferView indexBufferView;
		Devices::BufferView vertexBufferView;
		entt::entity lightEntity;
		vk::Rect2D scissor;
		entt::entity lightPerspectiveEntity;
		bool hasTransparency;
		float depth;
	};
}