#pragma once

#include "../../Pipelines/Pipeline.hpp"
#include "../../Devices/BufferView.hpp"
#include "../../Draws/DrawSystem.hpp"
#include <typeindex>

namespace drk::Scenes::Draws {
	struct ShadowSceneDraw {
		drk::Draws::DrawSystem* drawSystem;
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