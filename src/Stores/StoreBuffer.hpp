#pragma once

#include <cstdint>
#include <vulkan/vulkan.hpp>
#include "../Devices/BufferView.hpp"
#include "../Common/IndexGenerator.hpp"
#include <fmt/format.h>

namespace drk::Stores {
	class GenericStoreBuffer {
	protected:
		void *const pMappedMemory;
	public:
		uint32_t nextIndex;
		uint32_t maxIndexCount;
		std::queue<uint32_t> availableIndices;
		uint32_t descriptorArrayElement;

		GenericStoreBuffer(uint32_t maxIndexCount, uint32_t descriptorArrayElement, void *const pMappedMemory)
			: maxIndexCount(maxIndexCount), descriptorArrayElement(descriptorArrayElement), pMappedMemory(pMappedMemory) {
			nextIndex = 0;
		}
		~GenericStoreBuffer() = default;

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
					fmt::format(
						"Failed to yield new index: maximum index count reached ({0}).",
						maxIndexCount
					));
			return nextIndex++;
		}

		uint32_t remove(uint32_t index) {
			if (index >= nextIndex) {
				if (nextIndex > maxIndexCount)
					throw std::runtime_error(
						fmt::format(
							"Failed to release index {0}: must be lower than the next available index ({1}).",
							index,
							maxIndexCount
						));
				availableIndices.push(index);
			}
			return 0;
		}
	};

	template<class T>
	class StoreBuffer : public GenericStoreBuffer {
	public:
		StoreBuffer(uint32_t maxIndexCount, uint32_t descriptorArrayElement, T *const mappedMemory) : GenericStoreBuffer(
			maxIndexCount,
			descriptorArrayElement,
			reinterpret_cast<void *>(mappedMemory)) {
		}

		T *const mappedMemory() {
			return reinterpret_cast<T *const>(pMappedMemory);
		};
	};
}