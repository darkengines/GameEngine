
#pragma once

#include <entt/entt.hpp>
#include "../../Devices/DeviceContext.hpp"
#include "../../Renderers/Renderer.hpp"
#include "../../Engine/EngineState.hpp"
#include "../../Meshes/Pipelines/MeshPipeline.hpp"
#include "../../Points/Pipelines/PointPrimitivePipeline.hpp"
#include "../../Devices/ImageInfo.hpp"
#include "../../Draws/Systems/DrawSystem.hpp"
#include "../Draws/SceneDraw.hpp"
#include "SceneRenderOperation.hpp"
#include "../../Lines/Pipelines/LinePipeline.hpp"
#include "./ShadowSceneRenderer.hpp"
#include "../../Lights/Systems/ShadowMappingSystem.hpp"


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
		std::unique_ptr<Points::Pipelines::PointPrimitivePipeline> pointPrimitivePipeline;
		std::unique_ptr<Lines::Pipelines::LinePipeline> linePipeline;
		std::unique_ptr<ShadowSceneRenderer> shadowSceneRenderer;
		Lights::Systems::ShadowMappingSystem& shadowMappingSystem;
		vk::RenderPass renderPass;
	public:
		SceneRenderer(
			const Devices::DeviceContext& deviceContext,
			entt::registry& registry,
			std::unique_ptr<Meshes::Pipelines::MeshPipeline> meshPipeline,
			std::unique_ptr<Points::Pipelines::PointPrimitivePipeline> pointPrimitivePipeline,
			std::unique_ptr<Lines::Pipelines::LinePipeline> linePipeline,
			std::unique_ptr<ShadowSceneRenderer> shadowSceneRenderer,
			Lights::Systems::ShadowMappingSystem& shadowMappingSystem
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
		Pipelines::Pipeline* getPipeline(std::type_index pipelineTypeIndex);
		void createFramebufferResources();
		void destroyFramebufferResources();
		void createFramebuffers();
		void destroyFramebuffers();
		void createRenderPass();
		void destroyRenderPass();
		void draw(
			entt::entity previousDrawEntity,
			const Draws::SceneDraw& previousSceneDraw,
			const vk::CommandBuffer& commandBuffer,
			int instanceCount,
			int firstInstance,
			Pipelines::Pipeline const* pPipeline
		);
		void doOperations(
			const vk::CommandBuffer& commandBuffer,
			SceneRenderOperation sceneRenderOperation,
			const Draws::SceneDraw& sceneDraw,
			Pipelines::Pipeline const** ppPipeline
		);
	};
}