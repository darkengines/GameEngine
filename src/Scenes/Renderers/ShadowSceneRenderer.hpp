
#pragma once

#include <entt/entt.hpp>
#include "../../Devices/DeviceContext.hpp"
#include "../../Renderers/Renderer.hpp"
#include "../../Engine/EngineState.hpp"
#include "../../Meshes/Pipelines/ShadowMeshPipeline.hpp"
#include "../../Points/Pipelines/PointPrimitivePipeline.hpp"
#include "../../Devices/ImageInfo.hpp"
#include "../../Draws/Systems/DrawSystem.hpp"
#include "../Draws/ShadowSceneDraw.hpp"
#include "SceneRenderOperation.hpp"
#include "../../Lines/Pipelines/LinePipeline.hpp"


namespace drk::Scenes::Renderers {
	class ShadowSceneRenderer : public drk::Renderers::Renderer {
	protected:
		entt::registry& registry;
		const Devices::DeviceContext& deviceContext;
		std::vector<vk::Framebuffer> framebuffers;
		std::optional<Devices::ImageInfo> targetImageInfo;
		std::vector<vk::ImageView> targetImageViews;
		std::unique_ptr<Meshes::Pipelines::ShadowMeshPipeline> meshShadowPipeline;
		vk::RenderPass renderPass;
	public:
		ShadowSceneRenderer(
			const Devices::DeviceContext& deviceContext,
			entt::registry& registry,
			std::unique_ptr<Meshes::Pipelines::ShadowMeshPipeline> meshShadowPipeline
		);
		~ShadowSceneRenderer();
		void render(uint32_t targetImageIndex, const vk::CommandBuffer& sceneDraw);
		void setTargetImageViews(Devices::ImageInfo targetImageInfo, std::vector<vk::ImageView> targetImageViews);
		static Devices::Texture BuildSceneRenderTargetTexture(
			const Devices::DeviceContext& deviceContext,
			vk::Extent3D extent
		);
		void setTargetExtent(vk::Extent3D extent2D);
	protected:
		Pipelines::Pipeline* getPipeline(std::type_index pipelineTypeIndex);
		void createFramebuffers();
		void destroyFramebuffers();
		void createRenderPass();
		void destroyRenderPass();
		void draw(
			entt::entity previousDrawEntity,
			const Draws::ShadowSceneDraw& previousSceneDraw,
			const vk::CommandBuffer& commandBuffer,
			int instanceCount,
			int firstInstance,
			Pipelines::Pipeline const* pPipeline
		);
		void doOperations(
			const vk::CommandBuffer& commandBuffer,
			SceneRenderOperation sceneRenderOperation,
			const Draws::ShadowSceneDraw& sceneDraw,
			Pipelines::Pipeline const** ppPipeline
		);
	};
}