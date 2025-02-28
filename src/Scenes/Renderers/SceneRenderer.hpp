
#pragma once

#include <imgui.h>

#include <entt/entt.hpp>

#include "../../BoundingVolumes/Pipelines/BoundingVolumePipeline.hpp"
#include "../../Devices/DeviceContext.hpp"
#include "../../Devices/ImageInfo.hpp"
#include "../../Draws/Systems/IDrawSystem.hpp"
#include "../../Engine/EngineState.hpp"
#include "../../Frustums/Pipelines/FrustumPipeline.hpp"
#include "../../Lights/Systems/ShadowMappingSystem.hpp"
#include "../../Lines/Pipelines/LinePipeline.hpp"
#include "../../Meshes/Pipelines/MeshPipeline.hpp"
#include "../../Points/Pipelines/PointPrimitivePipeline.hpp"
#include "../../Renderers/RenderOperation.hpp"
#include "../../Renderers/Renderer.hpp"
#include "../Draws/SceneDraw.hpp"
#include "./ShadowSceneRenderer.hpp"

namespace drk::Scenes::Renderers
{
  class SceneRenderer : public drk::Renderers::Renderer
  {
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
    vk::Extent3D userExtent;

   public:
    void SetupImgui();
    SceneRenderer(Engine::EngineState& engineState,
        const Devices::DeviceContext& deviceContext,
        entt::registry& registry,
        std::function<std::unique_ptr<Meshes::Pipelines::MeshPipeline>()> meshPipelineFactory,
        std::function<std::unique_ptr<Points::Pipelines::PointPrimitivePipeline>()> pointPrimitivePipelineFactory,
        std::function<std::unique_ptr<Lines::Pipelines::LinePipeline>()> linePipelineFactory,
        std::function<std::unique_ptr<BoundingVolumes::Pipelines::BoundingVolumePipeline>()> boundingVolumePipelineFactory,
        std::function<std::unique_ptr<Frustums::Pipelines::FrustumPipeline>()> frustumPipelineFactory,
        std::function<std::unique_ptr<Renderers::ShadowSceneRenderer>()> shadowSceneRendererFactory,
        Lights::Systems::ShadowMappingSystem& shadowMappingSystem);
    ~SceneRenderer();
    vk::Extent3D getUserExtent();
    void render(uint32_t targetImageIndex, const vk::CommandBuffer& sceneDraw);
    void setTargetImageViews(Devices::ImageInfo targetImageInfo, std::vector<vk::ImageView> targetImageViews);
    static Devices::Texture BuildSceneRenderTargetTexture(const Devices::DeviceContext& deviceContext, vk::Extent3D extent);
    void setTargetExtent(vk::Extent3D extent2D);

   protected:
    Pipelines::GraphicsPipeline* getPipeline(std::type_index pipelineTypeIndex);
    void createFramebufferResources();
    void destroyFramebufferResources();
    void createFramebuffers();
    void destroyFramebuffers();
    void createRenderPass();
    void destroyRenderPass();
    void draw(entt::entity previousDrawEntity,
        const Draws::SceneDraw& previousSceneDraw,
        const vk::CommandBuffer& commandBuffer,
        int instanceCount,
        int firstInstance,
        Pipelines::GraphicsPipeline const* pPipeline);
    void doOperations(const vk::CommandBuffer& commandBuffer,
        drk::Renderers::RenderOperation sceneRenderOperation,
        const Draws::SceneDraw& sceneDraw,
        Pipelines::GraphicsPipeline const** ppPipeline);
  };
}  // namespace drk::Scenes::Renderers
