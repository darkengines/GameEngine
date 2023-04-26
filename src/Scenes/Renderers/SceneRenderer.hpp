
#pragma once

#include <entt/entt.hpp>
#include "../../Devices/DeviceContext.hpp"
#include "../../Renderers/Renderer.hpp"
#include "../../Engine/EngineState.hpp"
#include "../../Meshes/Pipelines/MeshPipeline.hpp"


namespace drk::Scenes::Renderers {
	class SceneRenderer : public drk::Renderers::Renderer {
	protected:
		entt::registry& registry;
		const Devices::DeviceContext& deviceContext;
		Engine::EngineState& engineState;
		Devices::Texture colorTexture;
		Devices::Texture depthTexture;
		vk::Framebuffer framebuffer;
		std::optional<Devices::Texture> targetTexture;
		std::unique_ptr<Meshes::Pipelines::MeshPipeline> meshPipeline;
		vk::RenderPass renderPass;
	public:
		SceneRenderer(
			const Devices::DeviceContext& deviceContext,
			drk::Engine::EngineState& engineState,
			entt::registry& registry,
			std::unique_ptr<Meshes::Pipelines::MeshPipeline> meshPipeline
		);
		SceneRenderer::~SceneRenderer();
		void Render();
	protected:
		void createFramebufferResources();
		void destroyFramebufferResources();
		void createFramebuffer();
		void destroyFramebuffer();
		void createRenderPass();
		void recreatePipeLine();
		void render(const vk::CommandBuffer& commandBuffer);
	};
}