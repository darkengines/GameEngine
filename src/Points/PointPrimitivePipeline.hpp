#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include "../Devices/DeviceContext.hpp"
#include "../Graphics/Graphics.hpp"
#include "Models/PointVertex.hpp"
#include "../Objects/Models/Object.hpp"
#include "Components/PointDraw.hpp"
#include "../Cameras/Camera.hpp"
#include "../Geometries/Primitives.hpp"
#include <algorithm>

namespace drk::Points {
	class PointPrimitivePipeline {
	protected:
		const Devices::DeviceContext& deviceContext;
		const vk::PipelineLayout& mainPipelineLayout;
		const Engine::EngineState& engineState;
		const entt::registry& registry;
		const Geometries::Primitives& primitives;
		const vk::ShaderModule vertexShaderModule;
		const vk::ShaderModule fragmentShaderModule;
	public:
		const vk::ShaderModule& getVertexShaderModule() const;
		const vk::ShaderModule& getFragmentShaderModule() const;

	public:
		PointPrimitivePipeline(
			const Devices::DeviceContext& deviceContext,
			const vk::PipelineLayout& mainPipelineLayout,
			const entt::registry& registry,
			const Geometries::Primitives& primitives,
			const Engine::EngineState& engineState
		);
		~PointPrimitivePipeline() {
			deviceContext.device.destroyShaderModule(vertexShaderModule);
			deviceContext.device.destroyShaderModule(fragmentShaderModule);
		}


		vk::Pipeline BuildGraphicsPipeline(const vk::RenderPass& renderPass, const vk::Extent2D& extent);

//		std::vector<Draws::DrawCommand> BuildMainRenderPass() const {
//			auto objectEntities = registry.view<Stores::StoreItem<Objects::Models::Object>, Models::PointVertex, Spatials::Spatial>();
//			std::vector<PointDraw> draws;
//			std::vector<PointDraw> transparencyDraws;
//			objectEntities.each(
//				[&](
//					entt::entity pointEntity,
//					auto& objectStoreItem,
//					auto& point,
//					auto& spatial
//				) {
//					const auto& objectStoreItemLocation = objectStoreItem.frameStoreItems[engineState.FrameIndex];
//					const auto pointStoreItem = engineState.Registry.get<Stores::StoreItem<Models::PointVertex>>(pointEntity);
//					const auto& pointStoreItemLocation = pointStoreItem.frameStoreItems[engineState.FrameIndex];
//					PointDraw draw = {
//						.point = point,
//						.pointStoreItem = {
//							.storeIndex = pointStoreItemLocation.pStore->descriptorArrayElement,
//							.itemIndex = pointStoreItemLocation.index
//						},
//						.objectLocation = {
//							.storeIndex = objectStoreItemLocation.pStore->descriptorArrayElement,
//							.itemIndex = objectStoreItemLocation.index
//						},
//						.spatial = spatial,
//						.hasTransparency = false
//					};
//					if (point.materialEntity != entt::null) {
//						const auto& material = registry.get<Materials::Material>(point.materialEntity);
//						draw.hasTransparency = material.hasTransparency;
//					}
//					if (draw.hasTransparency) {
//						transparencyDraws.push_back(draw);
//					} else {
//						draws.push_back(draw);
//					}
//				}
//			);
//
//			auto cameraEntity = engineState.CameraEntity;
//			auto camera = registry.get<Cameras::Camera>(cameraEntity);
//
//			std::stable_sort(
//				transparencyDraws.begin(),
//				transparencyDraws.end(),
//				[&camera](const PointDraw& leftDraw, const PointDraw& rightDraw) {
//					auto leftDistance = glm::distance(camera.absolutePosition, leftDraw.spatial.absolutePosition);
//					auto rightDistance = glm::distance(camera.absolutePosition, rightDraw.spatial.absolutePosition);
//					return leftDistance > rightDistance;
//				}
//			);
//
//
//
//			return drawContext;
//		}
//
//		void PopulateDrawContext(
//			DrawContext& drawContext,
//			const std::vector<PointDraw>& draws,
//			uint32_t drawOffset
//		) const {
//			auto drawStore = engineState.FrameStates[engineState.FrameIndex].DrawStore.get();
//			const PointDraw* previousDraw = nullptr;
//
//			if (!drawContext.drawSets.empty() && !drawContext.drawSets.back().draws.empty())
//				previousDraw = &drawContext.drawSets.back().draws.back();
//
//			for (auto drawIndex = 0u; drawIndex < draws.size(); drawIndex++) {
//				auto& draw = draws[drawIndex];
//
//				if (previousDraw != nullptr && previousDraw->meshInfo == draw.meshInfo) {
//					drawContext.drawSets.back().drawCommands.back().instanceCount++;
//				} else {
//					if (previousDraw == nullptr ||
//						draw.mesh.IndexBufferView.buffer.buffer != previousDraw->mesh.IndexBufferView.buffer.buffer) {
//						drawContext.drawSets.push_back(
//							{
//								.indexBuffer = draw.mesh.IndexBufferView.buffer,
//								.vertexBuffer = draw.mesh.VertexBufferView.buffer,
//							}
//						);
//					}
//					drawContext.drawSets.back().drawCommands.push_back(
//						{
//							.indexCount = (uint32_t) draw.meshInfo->indices.size(),
//							.instanceCount = 1,
//							.firstIndex = static_cast<uint32_t>(draw.mesh.IndexBufferView.byteOffset /
//																sizeof(Meshes::VertexIndex)),
//							.vertexOffset = static_cast<uint32_t>(draw.mesh.VertexBufferView.byteOffset /
//																  sizeof(Meshes::Vertex)),
//							.firstInstance = drawIndex + drawOffset
//						}
//					);
//				}
//
//				drawContext.drawSets.back().draws.push_back(draw);
//
//				const auto drawItemLocation = drawStore->Get(drawIndex + drawOffset);
//
//				const Models::Draw drawModel = {
//					.meshItemLocation = draw.meshStoreItem,
//					.objectItemLocation = draw.objectLocation,
//				};
//
//				drawItemLocation.pItem->meshItemLocation.storeIndex = drawModel.meshItemLocation.storeIndex;
//				drawItemLocation.pItem->meshItemLocation.itemIndex = drawModel.meshItemLocation.itemIndex;
//				drawItemLocation.pItem->objectItemLocation.storeIndex = drawModel.objectItemLocation.storeIndex;
//				drawItemLocation.pItem->objectItemLocation.itemIndex = drawModel.objectItemLocation.itemIndex;
//
//				previousDraw = &draw;
//			}
//		}

	};
}