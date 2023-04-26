#pragma once
#include <vector>
#include "DrawCommand.hpp"
#include "../Devices/BufferView.hpp"
#include "Draw.hpp"

namespace drk::Draws {
	struct DrawSet {
		const Devices::BufferView& indexBufferView;
		const Devices::BufferView& vertexBufferView;
		std::vector<std::unique_ptr<GenericDraw>> draws;
		std::vector<Draws::DrawCommand> drawCommands;
	};
}