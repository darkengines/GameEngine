#pragma once

#include <vector>
#include "Buffer.hpp"
#include "BufferViewState.hpp"

namespace drk::Devices {
	template<typename TState>
	struct BufferStateUploadResult {
		const Buffer buffer;
		const std::vector<BufferViewState<TState>> bufferViewStates;
	};
}