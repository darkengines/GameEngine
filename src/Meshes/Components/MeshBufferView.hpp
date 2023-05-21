#pragma once

#include "../../Devices/BufferView.hpp"

namespace drk::Meshes::Components {
	struct MeshBufferView {
		Devices::BufferView IndexBufferView;
		Devices::BufferView VertexBufferView;
	};
}
