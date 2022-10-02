#pragma once

#include "../Devices/BufferView.hpp"

namespace drk::Meshes {
	struct Mesh {
		Devices::BufferView IndexBufferView;
		Devices::BufferView VertexBufferView;
	};
}
