#pragma once

#include "../../Pipelines/Pipeline.hpp"
#include "../../Devices/BufferView.hpp"
#include "../../Draws/DrawSystem.hpp"

namespace drk::Scenes::Draws {
	struct SceneDraw {
		drk::Draws::DrawSystem* drawSystem;
		Devices::BufferView const * indexBufferView;
		Devices::BufferView const * vertexBufferView;
		bool hasTransparency;
		float depth;
	};
}