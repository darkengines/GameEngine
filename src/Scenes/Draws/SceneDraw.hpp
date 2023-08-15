#pragma once

#include "../../Pipelines/Pipeline.hpp"
#include "../../Devices/BufferView.hpp"
#include "../../Draws/DrawSystem.hpp"
#include <typeindex>

namespace drk::Scenes::Draws {
	struct SceneDraw {
		drk::Draws::DrawSystem* drawSystem;
		std::type_index pipelineTypeIndex;
		Devices::BufferView indexBufferView;
		Devices::BufferView vertexBufferView;
		bool hasTransparency;
		float depth;
	};
}