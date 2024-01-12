#pragma once
#include "BufferView.hpp"

namespace drk::Devices {
	template<typename TState>
	struct BufferViewState {
		BufferView bufferView;
		TState state;
	};
}
