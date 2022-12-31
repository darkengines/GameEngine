#pragma once
#include <vector>
#include "DrawCommand.hpp"
#include "../Devices/BufferView.hpp"

namespace drk::Draws {
	template<typename TDraw>
	struct DrawSet {
		const Devices::BufferView& indexBufferView;
		const Devices::BufferView& vertexBufferView;
		std::vector<TDraw> draws;
		std::vector<Draws::DrawCommand> drawCommands;
	};
}