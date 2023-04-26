#pragma once

#include "../../Pipelines/Pipeline.hpp"
#include "../../Devices/BufferView.hpp"

namespace drk::Scenes::Models {
	struct SceneDraw {
		Devices::BufferView const * indexBufferView;
		Devices::BufferView const * vertexBufferView;
		bool hasTransparency;
		float depth;
	};
}