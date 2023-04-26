#pragma once
#include "../../Devices/Device.hpp"
#include "../../Devices/DeviceContext.hpp"
#include "../../Engine/EngineState.hpp"
#include "../Models/MeshDraw.hpp"
#include "../../Draws/DrawSet.hpp"
#include "../../Draws/Draw.hpp"
#include "../../Pipelines/Pipeline.hpp"

namespace drk::Meshes::Pipelines {
	class MeshPipeline : public drk::Pipelines::Pipeline {
	public:
		MeshPipeline(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			const Engine::DescriptorSetLayouts& descriptorSetLayouts
		);
		~MeshPipeline();

		void render(const vk::CommandBuffer& commandBuffer) const;
		void bind(const vk::CommandBuffer& commandBuffer) const;
		std::vector<Draws::DrawSet> PrepareDraws();

	protected:
		const Devices::DeviceContext& deviceContext;
		const Engine::EngineState& engineState;
		vk::ShaderModule mainVertexShaderModule;
		vk::ShaderModule mainFragmentShaderModule;
		vk::Pipeline pipeline;
		std::array<vk::DescriptorSetLayout, 4> descriptorSetLayouts;
		vk::PipelineLayout pipelineLayout;

		void createShaderModules();
		void destroyShaderModules();

		void createPipeline();
		void recreatePipeLine();

		static vk::PipelineLayout createPipelineLayout(
			const Devices::DeviceContext& deviceContext,
			const std::array<vk::DescriptorSetLayout, 4>& descriptorSetLayouts
		);
	};
}