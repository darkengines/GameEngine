#pragma once

#include "StoreBufferAllocator.hpp"

namespace drk::Graphics {
	StoreBufferAllocator::~StoreBufferAllocator() {
		for (const auto buffer: Buffers) {
			DeviceContext->DestroyBuffer(buffer);
		}
	}
}