#pragma once

#include <cstdint>
#include <format>
#include <vulkan/vulkan.hpp>
#include "../Devices/BufferView.hpp"
#include "../Common/IndexGenerator.hpp"

namespace drk::Graphics {
	class GenericStoreBuffer {
	protected:
		void *const pMappedMemory;
	public:
		uint32_t nextIndex;
		uint32_t maxIndexCount;
		std::queue<uint32_t> availableIndices;

		GenericStoreBuffer(uint32_t maxIndexCount, void *const pMappedMemory)
			: maxIndexCount(maxIndexCount), pMappedMemory(pMappedMemory) {
			nextIndex = 0;
		}
		~GenericStoreBuffer() = default;
		uint32_t descriptorArrayElement;

		bool hasAvailableIndex() {
			return nextIndex < maxIndexCount || !availableIndices.empty();
		}

		uint32_t add() {
			if (!availableIndices.empty()) {
				auto index = availableIndices.front();
				availableIndices.pop();
				return index;
			}
			if (nextIndex > maxIndexCount)
				throw std::runtime_error(
					std::format(
						"Failed to yield new index: maximum index count reached ({0}).",
						maxIndexCount
					));
			return nextIndex++;
		}

		uint32_t remove(uint32_t index) {
			if (index >= nextIndex) {
				if (nextIndex > maxIndexCount)
					throw std::runtime_error(
						std::format(
							"Failed to release index {0}: must be lower than the next available index ({1}).",
							index,
							maxIndexCount
						));
				availableIndices.push(index);
			}
		}
	};

	template<class T>
	class StoreBuffer : public GenericStoreBuffer {
	public:
		StoreBuffer(uint32_t maxIndexCount, T *const mappedMemory) : GenericStoreBuffer(
			maxIndexCount,
			reinterpret_cast<void *>(mappedMemory)) {
		}

		T *const mappedMemory() {
			return reinterpret_cast<T *const>(pMappedMemory);
		};
	};
}