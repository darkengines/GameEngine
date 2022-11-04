#pragma once

#include "../Devices/Buffer.hpp"
#include "../Devices/DeviceContext.hpp"
#include "Models/PointVertex.hpp"
#include <span>
#include "../Devices/Device.hpp"

namespace drk::Geometries {
	class Primitives {
		Devices::Buffer vertexBuffer;
		Devices::Buffer indexBuffer;
		const Devices::DeviceContext& deviceContext;
		Devices::BufferView pointVertexBufferView;
		Devices::BufferView pointIndexBufferView;

		Primitives(const Devices::DeviceContext& deviceContext) : deviceContext(deviceContext) {
			vk::DeviceSize primitiveOffset = 0;
			vk::DeviceSize indexOffset = 0;
			auto pointOffset = primitiveOffset += sizeof(Points::Models::PointVertex);
			auto pointIndexOffset = indexOffset += sizeof(uint32_t);
			std::vector<unsigned char> primitiveBytes(primitiveOffset);
			std::vector<uint32_t> indices(indexOffset);
			initializePoint(
				*reinterpret_cast<Points::Models::PointVertex*>(primitiveBytes.data() + pointOffset),
				*(indices.data() + indexOffset));

			std::span primitiveByteSpan{primitiveBytes};
			std::span indexByteSpan{indices};

			auto vertexResult = Devices::Device::uploadBuffers<unsigned char>(
				deviceContext.PhysicalDevice,
				deviceContext.device,
				deviceContext.GraphicQueue,
				deviceContext.CommandPool,
				deviceContext.Allocator,
				{primitiveByteSpan},
				vk::BufferUsageFlagBits::eVertexBuffer
			);
			vertexBuffer = vertexResult.buffer;
			pointVertexBufferView = {
				.buffer = vertexBuffer,
				.byteOffset = pointOffset,
				.byteLength = sizeof(Points::Models::PointVertex)
			};

			auto indexResult = Devices::Device::uploadBuffers<uint32_t>(
				deviceContext.PhysicalDevice,
				deviceContext.device,
				deviceContext.GraphicQueue,
				deviceContext.CommandPool,
				deviceContext.Allocator,
				{indexByteSpan},
				vk::BufferUsageFlagBits::eIndexBuffer
			);
			indexBuffer = indexResult.buffer;
			pointIndexBufferView = {
				.buffer = indexBuffer,
				.byteOffset = indexOffset,
				.byteLength = sizeof(uint32_t)
			};
		}

		void initializePoint(Points::Models::PointVertex& point, uint32_t& index) {
			index = 0;
			point = {
				.position = {0.0f, 0.0f, 0.0f, 1.0f},
				.diffuseColor = {1.0f, 1.0f, 1.0f, 1.0f},
				.textureCoordinates = {0.0f, 0.0f}
			};
		}
	};
}
