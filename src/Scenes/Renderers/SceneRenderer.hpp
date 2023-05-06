
#pragma once

#include <entt/entt.hpp>
#include "../../Devices/DeviceContext.hpp"
#include "../../Renderers/Renderer.hpp"
#include "../../Engine/EngineState.hpp"
#include "../../Meshes/Pipelines/MeshPipeline.hpp"
#include "../../Devices/ImageInfo.hpp"
#include "../../Draws/DrawSystem.hpp"
#include "../Draws/SceneDraw.hpp"
#include "SceneRenderOperation.hpp"


namespace drk::Scenes::Renderers {
	class SceneRenderer : public drk::Renderers::Renderer {
	protected:
		entt::registry& registry;
		const Devices::DeviceContext& deviceContext;
		std::optional<Devices::Texture> colorTexture;
		std::optional<Devices::Texture> depthTexture;
		std::vector<vk::Framebuffer> framebuffers;
		std::optional<Devices::ImageInfo> targetImageInfo;
		std::vector<vk::ImageView> targetImageViews;
		std::unique_ptr<Meshes::Pipelines::MeshPipeline> meshPipeline;
		vk::RenderPass renderPass;
	public:
		SceneRenderer(
			const Devices::DeviceContext& deviceContext,
			entt::registry& registry,
			std::unique_ptr<Meshes::Pipelines::MeshPipeline> meshPipeline
		);
		~SceneRenderer();
		void render(uint32_t targetImageIndex, const vk::CommandBuffer& sceneDraw);
		void setTargetImageViews(Devices::ImageInfo targetImageInfo, std::vector<vk::ImageView> targetImageViews);
		static Devices::Texture BuildSceneRenderTargetTexture(
			const Devices::DeviceContext& deviceContext,
			vk::Extent3D extent
		);
		void setTargetExtent(vk::Extent3D extent2D);
	protected:
		void createFramebufferResources();
		void destroyFramebufferResources();
		void createFramebuffers();
		void destroyFramebuffers();
		void createRenderPass();
		void destroyRenderPass();
		void draw(
			entt::entity previousDrawEntity,
			Draws::SceneDraw previousSceneDraw,
			const vk::CommandBuffer& commandBuffer,
			int instanceCount,
			int firstInstance
		);
		void doOperations(
			const vk::CommandBuffer& commandBuffer,
			SceneRenderOperation sceneRenderOperation,
			Draws::SceneDraw sceneDraw
		);
	};
}