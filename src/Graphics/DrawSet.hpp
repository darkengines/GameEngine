#pragma once

#include <cstdint>
#include <vector>
#include "Draw.hpp"
#include "../Draws/DrawCommand.hpp"

namespace drk::Graphics {
	struct DrawSet {
		Devices::Buffer indexBuffer;
		Devices::Buffer vertexBuffer;
		std::vector< Draws::DrawCommand> drawCommands;
		std::vector<Draw> draws;
	};
}
