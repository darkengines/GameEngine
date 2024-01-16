#pragma once

#include <vulkan/vulkan.hpp>
#include "../../Devices/Buffer.hpp"
#include "../../Devices/DeviceContext.hpp"
#include "../../Devices/BufferUploadResult.hpp"
#include <vector>

namespace drk::Buffers::Resources {
	class BufferResourceManager {
	protected:
		Devices::DeviceContext& deviceContext;
		std::vector<Devices::Buffer> buffers;
	public:
		BufferResourceManager(Devices::DeviceContext& deviceContext): deviceContext(deviceContext) {}
		~BufferResourceManager() {
			for (const auto buffer : buffers) {
				deviceContext.DestroyBuffer(buffer);
			}
		}
		template<typename TElement>
		Devices::BufferUploadResult upload(std::vector<std::span<TElement>> sourceBuffer, vk::BufferUsageFlags memoryUsage) {
			const auto& uploadResult = Devices::Device::uploadBuffers(
				deviceContext.PhysicalDevice,
				deviceContext.device,
				deviceContext.GraphicQueue,
				deviceContext.CommandPool,
				deviceContext.Allocator,
				sourceBuffer,
				memoryUsage
			);

			buffers.push_back(uploadResult.buffer);

			return uploadResult;
		}
	};
}