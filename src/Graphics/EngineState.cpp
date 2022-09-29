#include "EngineState.hpp"
#include <utility>

namespace drk::Graphics {
	EngineState::EngineState(const Devices::DeviceContext *deviceContext) :
		DeviceContext(deviceContext) {
		FrameStates.push_back(std::move<Graphics::FrameState>({DeviceContext}));
		FrameStates.push_back(std::move<Graphics::FrameState>({DeviceContext}));
	}
}
