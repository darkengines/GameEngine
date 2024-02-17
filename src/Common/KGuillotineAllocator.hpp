/*
	SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
	SPDX-License-Identifier: LGPL-3.0-or-later
*/

#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

namespace drk::guillotine {
	typedef enum Orientation {
		Horizontal = 1 << 0,
		Vertical = 1 << 1
	} Orientation;

	class AllocatorPrivate;

	/**
	 * \internal
	 */
	struct AllocationId {
		static constexpr AllocationId null() noexcept { return -1; }

		constexpr AllocationId(size_t value) noexcept
			: data(value) {
		}
		constexpr operator size_t() const noexcept {
			return data;
		}
		constexpr bool operator==(const AllocationId& other) const noexcept {
			return data == other.data;
		}

		size_t data;
	};

	/**
	 * The Allocation type represents a single 2D allocation. The isNull() function
	 * returns \c true if the allocation is invalid.
	 */
	struct Allocation {
		bool isNull() const { return id == AllocationId::null(); }

		vk::Rect2D scissor;
		AllocationId id = AllocationId::null();
		bool transposed;
	};

	/**
	 * This enum type is used to specify how the allocator should place items.
	 */
	enum class AllocationMethod {
		/**
		 * Allocate items in a way that minimizes the remaning width of item bins. The allocator
		 * will tend to stack items horizontally.
		 */
		PreferLessHorizontalSpace,
		/**
		 * Allocate items in a way that minimizes the remaining height of item bins. The allocator
		 * will tend to stack items vertically.
		 */
		PreferLessVerticalSpace,
	};

	/**
	 * The AllocatorOptions provide a way to fine tune the behavior of the allocator.
	 */
	struct AllocatorOptions {
		AllocationMethod method = AllocationMethod::PreferLessHorizontalSpace;
		bool allowTranspose = true;
	};

	/**
	 * The Allocator class represens a dynamic texture atlas allocator.
	 *
	 * If the allocator  has failed to find a suitable area for the specified texture size, a null
	 * Allocation object will be returned.
	 */
	class Allocator {
	public:
		explicit Allocator(const vk::Extent2D& size, const AllocatorOptions& options = {});
		~Allocator();

		vk::Extent2D size() const;

		/**
		 * Allocates the space for a texture with the specified size \a requestedSize. If
		 * the allocation has failed, a null Allocation object will be returned.
		 */
		Allocation allocate(const vk::Extent2D& requestedSize);

		/**
		 * Release a rectangular area previously allocated by the allocate() function. Passing
		 * an allocation id that was not returned by allocate() will lead to undefined behavior.
		 */
		void deallocate(AllocationId allocationId);

	private:
		AllocatorPrivate* d;
	};
} // namespace KGuillotineAllocator