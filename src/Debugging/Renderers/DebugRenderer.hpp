#include "../../BoundingVolumes/Pipelines/BoundingVolumePipeline.hpp"
#include "../../Devices/DeviceContext.hpp"
#include "../../Devices/ImageInfo.hpp"
#include "../../Renderers/Renderer.hpp"
#include "../../Renderers/RenderOperation.hpp"
#include "../Components/DebugDraw.hpp"
#include <entt/entity/fwd.hpp>
#include <optional>

namespace drk::Debugging::Renderers {
	class DebugRenderer : public drk::Renderers::Renderer {
	protected:
		entt::registry& registry;
		const Devices::DeviceContext& deviceContext;
		std::optional<Devices::Texture> colorTexture;
		std::optional<Devices::Texture> depthTexture;
		std::vector<vk::Framebuffer> framebuffers;
		std::optional<Devices::ImageInfo> targetImageInfo;
		std::vector<vk::ImageView> targetImageViews;
		std::unique_ptr<BoundingVolumes::Pipelines::BoundingVolumePipeline> boundingVolumePipeline;
		vk::RenderPass renderPass;
		std::unordered_map<std::type_index, Pipelines::GraphicsPipeline*> pipelines;
	public:
		DebugRenderer(
			const Devices::DeviceContext& deviceContext,
			entt::registry& registry,
			std::unique_ptr<BoundingVolumes::Pipelines::BoundingVolumePipeline> boundingVolumePipeline
		);
		~DebugRenderer();
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
			const Components::DebugDraw& previousSceneDraw,
			const vk::CommandBuffer& commandBuffer,
			int instanceCount,
			int firstInstance,
			Pipelines::GraphicsPipeline const* pPipeline
		);
		void doOperations(
			const vk::CommandBuffer& commandBuffer,
			drk::Renderers::RenderOperation sceneRenderOperation,
			const Components::DebugDraw& sceneDraw,
			Pipelines::GraphicsPipeline const** ppPipeline
		);
	};
}