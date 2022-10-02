#pragma once

#include "../Devices/DeviceContext.hpp"
#include "../Graphics/EngineState.hpp"

namespace drk::Meshes {
	class MeshSystem {
	protected:
		Devices::DeviceContext *DeviceContext;
		Graphics::EngineState *EngineState;

	public:
		MeshSystem(Devices::DeviceContext *pContext, Graphics::EngineState *pState);
		void UploadMeshes();
	};
}