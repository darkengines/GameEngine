#pragma once

#include "../../Pipelines/GraphicsPipeline.hpp"
#include "../../Devices/BufferView.hpp"
#include "../../Draws/Systems/DrawSystem.hpp"
#include <typeindex>

namespace drk::Scenes::Draws {
	struct SceneDraw {
		entt::entity nodeEntity;
		drk::Draws::Systems::DrawSystem* drawSystem;
		std::type_index pipelineTypeIndex;
		Devices::BufferView indexBufferView;
		Devices::BufferView vertexBufferView;
		bool hasTransparency;
		float depth;
	};
}