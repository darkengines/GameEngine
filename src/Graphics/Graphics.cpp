#include "Graphics.hpp"
#include "../Windows/Window.hpp"
#include "../Common/Common.hpp"
#include "../Devices/Device.hpp"
#include "../Meshes/Vertex.hpp"
#include "../Objects/Object.hpp"
#include "../Meshes/MeshGroup.hpp"
#include "Draw.hpp"
#include "../Spatials/Spatial.hpp"
#include "../Draws/DrawCommand.hpp"
#include "Models/Draw.hpp"
#include <imgui_impl_vulkan.h>
#include "../Objects/Models/Object.hpp"
#include "../Meshes/Models/Mesh.hpp"
#include "../Spatials/Models/Spatial.hpp"
#include <algorithm>
#include <vector>
#include "../Meshes/Vertex.hpp"
#include "../Materials/Material.hpp"
#include "../Cameras/Camera.hpp"
#include <stdexcept>

namespace drk::Graphics {
	std::vector<const char*> Graphics::RequiredInstanceExtensions = Windows::Window::getVulkanInstanceExtension();
	std::vector<const char*> Graphics::RequiredDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
	};

	Graphics::Graphics(
		Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		Windows::Window& window
	) : DeviceContext(deviceContext), EngineState(engineState) {

		CreateSwapchain(window.GetExtent());
	}

	Graphics::~Graphics() {
		DeviceContext.device.destroyDescriptorPool(ImGuiDescriptorPool);
		DestroySwapchain();
	}

	void Graphics::CreateSwapchain(const vk::Extent2D& extent) {
		Swapchain = Devices::Device::createSwapchain(
			DeviceContext.device,
			DeviceContext.PhysicalDevice,
			DeviceContext.Surface,
			extent
		);
	}

	void Graphics::DestroySwapchain() {
		Devices::Device::destroySwapchain(DeviceContext.device, Swapchain);
	}

	void Graphics::RecreateSwapchain(vk::Extent2D extent) {
		DeviceContext.device.waitIdle();
		DestroySwapchain();
		CreateSwapchain(extent);
		DeviceContext.device.waitIdle();
	}

	vk::PipelineDepthStencilStateCreateInfo Graphics::DefaultPipelineDepthStencilStateCreateInfo() {
		vk::PipelineDepthStencilStateCreateInfo depthStencil = {
			.depthTestEnable = true,
			.depthWriteEnable = true,
			.depthCompareOp = vk::CompareOp::eLess,
			.depthBoundsTestEnable = false,
			.stencilTestEnable = true,
			.minDepthBounds = 0.0f,
			.maxDepthBounds = 1.0f,
		};
		return depthStencil;
	}

	vk::PipelineColorBlendAttachmentState Graphics::DefaultPipelineColorBlendAttachmentState() {
		vk::PipelineColorBlendAttachmentState colorBlendAttachment = {
			.blendEnable = true,
			.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
			.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
			.colorBlendOp = vk::BlendOp::eAdd,
			.srcAlphaBlendFactor = vk::BlendFactor::eOne,
			.dstAlphaBlendFactor = vk::BlendFactor::eZero,
			.alphaBlendOp = vk::BlendOp::eAdd,
			.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
							  vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
		};
		return colorBlendAttachment;
	}

	vk::PipelineColorBlendStateCreateInfo
	Graphics::DefaultPipelineColorBlendStateCreateInfo(vk::PipelineColorBlendAttachmentState& pipelineColorBlendAttachmentState) {
		pipelineColorBlendAttachmentState = DefaultPipelineColorBlendAttachmentState();
		vk::PipelineColorBlendStateCreateInfo colorBlending = {
			.logicOpEnable = false,
			.logicOp = vk::LogicOp::eCopy,
			.attachmentCount = 1,
			.pAttachments = &pipelineColorBlendAttachmentState,
			.blendConstants = std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}
		};
		return colorBlending;
	}

	vk::PipelineMultisampleStateCreateInfo Graphics::DefaultPipelineMultisampleStateCreateInfo() {
		vk::PipelineMultisampleStateCreateInfo multisampling = {
			.rasterizationSamples = vk::SampleCountFlagBits::e8,
			.sampleShadingEnable = false,
			.minSampleShading = 1.0f,
			.alphaToCoverageEnable = false,
			.alphaToOneEnable = false,
		};
		return multisampling;
	}

	vk::PipelineRasterizationStateCreateInfo Graphics::DefaultPipelineRasterizationStateCreateInfo() {
		vk::PipelineRasterizationStateCreateInfo rasterizer = {
			.depthClampEnable = false,
			.rasterizerDiscardEnable = false,
			.polygonMode = vk::PolygonMode::eFill,
			.cullMode = vk::CullModeFlagBits::eBack,
			.frontFace = vk::FrontFace::eCounterClockwise,
			.depthBiasEnable = false,
			.depthBiasConstantFactor = 0.0f,
			.depthBiasClamp = 0.0f,
			.depthBiasSlopeFactor = 0.0f,
			.lineWidth = 1.0f,
		};

		return rasterizer;
	}

	vk::PipelineViewportStateCreateInfo Graphics::DefaultPipelineViewportStateCreateInfo(
		const vk::Extent2D& extent,
		vk::Viewport& viewport,
		vk::Rect2D& scissor
	) {
		viewport = vk::Viewport{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(extent.width),
			.height = static_cast<float>(extent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};

		scissor = {
			.offset = {0, 0},
			.extent = extent,
		};

		vk::PipelineViewportStateCreateInfo viewportState = {
			.viewportCount = 1,
			.pViewports = &viewport,
			.scissorCount = 1,
			.pScissors = &scissor
		};

		return viewportState;
	}

	vk::PipelineInputAssemblyStateCreateInfo Graphics::DefaultPipelineInputAssemblyStateCreateInfo() {
		vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {
			.topology = vk::PrimitiveTopology::eTriangleList,
			.primitiveRestartEnable = false,
		};
		return inputAssembly;
	}

	vk::PipelineVertexInputStateCreateInfo Graphics::DefaultPipelineVertexInputStateCreateInfo(
		const std::vector<vk::VertexInputBindingDescription>& vertexInputBindingDescriptions,
		const std::vector<vk::VertexInputAttributeDescription>& vertexInputAttributeDescriptions
	) {
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {
			.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindingDescriptions.size()),
			.pVertexBindingDescriptions = vertexInputBindingDescriptions.data(),
			.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributeDescriptions.size()),
			.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data(),
		};

		return vertexInputInfo;
	}

	vk::Result Graphics::Present(uint32_t swapchainImageIndex) {
		const auto& frameState = EngineState.getCurrentFrameState();
		const auto& fence = frameState.fence;
		const auto& imageReadySemaphore = frameState.imageReadySemaphore;
		const auto& imageRenderedSemaphore = frameState.imageRenderedSemaphore;

		vk::Result presentResults;
		vk::PresentInfoKHR presentInfoKHR = {
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &imageRenderedSemaphore,
			.swapchainCount = 1,
			.pSwapchains = &Swapchain.swapchain,
			.pImageIndices = &swapchainImageIndex,
			.pResults = &presentResults,
		};

		return DeviceContext.PresentQueue.presentKHR(presentInfoKHR);
	}
	vk::ResultValue<uint32_t> Graphics::AcuireSwapchainImageIndex() {
		const auto& frameState = EngineState.getCurrentFrameState();
		auto result = DeviceContext.device.acquireNextImageKHR(
			Swapchain.swapchain,
			UINT64_MAX,
			frameState.imageReadySemaphore,
			VK_NULL_HANDLE
		);
		return result;
	}
	const Devices::Swapchain& Graphics::GetSwapchain() const {
		return Swapchain;
	}
}