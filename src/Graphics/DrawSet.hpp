#pragma once

#include <cstdint>
#include <vector>
#include "Draw.hpp"
#include "DrawCommand.hpp"

namespace drk::Graphics {
	struct DrawSet {
		Devices::Buffer indexBuffer;
		Devices::Buffer vertexBuffer;
		std::vector<DrawCommand> drawCommands;
		std::vector<Draw> draws;
	};
}
