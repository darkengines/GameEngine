#include "./ShadowMappingSystem.hpp"
#include "../../Scenes/Renderers/ShadowSceneRenderer.hpp"

namespace drk::Lights::Systems {
	ShadowMappingSystem::ShadowMappingSystem(
		Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState
	) :
		deviceContext(deviceContext),
		engineState(engineState),
		shadowMappingTexture(BuildShadowMappingTexture(deviceContext, engineState)),
		shadowMapAllocator(
			guillotine::Allocator(
				{
					ShadowMappingSystem::shadowMapWidth,
					ShadowMappingSystem::shadowMapHeight
				}, {}
			)) {

	}
	Devices::Texture ShadowMappingSystem::BuildShadowMappingTexture(
		Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState
	) {
		auto texture = Scenes::Renderers::ShadowSceneRenderer::BuildSceneRenderTargetTexture(
			deviceContext, {
				ShadowMappingSystem::shadowMapWidth,
				ShadowMappingSystem::shadowMapHeight,
				1
			}
		);
		engineState.textureStore->registerTexture(texture);
		return texture;
	}
}