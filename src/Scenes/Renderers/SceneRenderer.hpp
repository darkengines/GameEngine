
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
#include "../../Renderers/RenderOperation.hpp"
#include "../../Lines/Pipelines/LinePipeline.hpp"
#include "./ShadowSceneRenderer.hpp"
#include "../../Lights/Systems/ShadowMappingSystem.hpp"
#include "../../BoundingVolumes/Pipelines/BoundingVolumePipeline.hpp"
#include "../../Frustums/Pipelines/FrustumPipeline.hpp"
#include <imgui.h>


namespace drk::Scenes::Renderers {
	class SceneRenderer : public drk::Renderers::Renderer {
	protected:
		Engine::EngineState& engineState;
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
		std::unique_ptr<BoundingVolumes::Pipelines::BoundingVolumePipeline> boundingVolumePipeline;
		std::unique_ptr<Frustums::Pipelines::FrustumPipeline> frustumPipeline;
		std::unique_ptr<ShadowSceneRenderer> shadowSceneRenderer;
		Lights::Systems::ShadowMappingSystem& shadowMappingSystem;
		vk::RenderPass renderPass;
		std::unordered_map<std::type_index, Pipelines::GraphicsPipeline*> pipelines;
		ImGuiContext* imGuiContextPtr;
	public:
		SceneRenderer(
			Engine::EngineState& engineState,
			const Devices::DeviceContext& deviceContext,
			entt::registry& registry,
			std::unique_ptr<Meshes::Pipelines::MeshPipeline> meshPipeline,
			std::unique_ptr<Points::Pipelines::PointPrimitivePipeline> pointPrimitivePipeline,
			std::unique_ptr<Lines::Pipelines::LinePipeline> linePipeline,
			std::unique_ptr<BoundingVolumes::Pipelines::BoundingVolumePipeline> boundingVolumePipeline,
			std::unique_ptr<Frustums::Pipelines::FrustumPipeline> frustumPipeline,
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
		Pipelines::GraphicsPipeline* getPipeline(std::type_index pipelineTypeIndex);
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
			Pipelines::GraphicsPipeline const* pPipeline
		);
		void doOperations(
			const vk::CommandBuffer& commandBuffer,
			drk::Renderers::RenderOperation sceneRenderOperation,
			const Draws::SceneDraw& sceneDraw,
			Pipelines::GraphicsPipeline const** ppPipeline
		);
	};
}