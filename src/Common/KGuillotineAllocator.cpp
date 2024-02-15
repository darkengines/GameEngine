/*
	SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
	SPDX-License-Identifier: LGPL-3.0-or-later
*/

#include "KGuillotineAllocator.hpp"
#include <limits>

namespace drk {

	namespace guillotine {

		static bool isEmpty(const vk::Extent2D& extent) { return extent.height <= 0 || extent.width <= 0; };
		static vk::Extent2D transpose(const vk::Extent2D& extent) { return {extent.height, extent.width}; };
		static vk::Rect2D rectUnion(const vk::Rect2D& left, const vk::Rect2D& right) {
			auto x1 = std::min(left.offset.x, right.offset.x);
			auto y1 = std::min(left.offset.y, right.offset.y);
			auto x2 = std::max(left.offset.x + left.extent.width, right.offset.x + right.extent.width);
			auto y2 = std::max(left.offset.y + left.extent.height, right.offset.y + right.extent.height);
			return vk::Rect2D{{x1,      y1},
							  {x2 - x1, y2 - y1}};
		};

		struct AllocationNode {
			enum class Kind {
				Fork,
				Leaf,
			};

			enum class Status {
				Free,
				Occupied,
				Deleted,
			};

			AllocationId prevSibling = AllocationId::null();
			AllocationId nextSibling = AllocationId::null();
			AllocationId parent = AllocationId::null();

			Orientation orientation;
			vk::Rect2D scissor;
			Kind kind;
			Status status;
		};

		class AllocatorPrivate {
		public:
			std::tuple<AllocationId, bool> selectFreeNode(const vk::Extent2D& size) const;

			AllocationId allocateNode();
			void releaseNode(AllocationId nodeId);

			int computeScore(int xDelta, int yDelta) const;

			std::vector<AllocationNode> nodes;
			vk::Extent2D size;
			AllocatorOptions options;
		};

		AllocationId AllocatorPrivate::allocateNode() {
			for (int i = 0; i < nodes.size(); ++i) {
				if (nodes[i].status == AllocationNode::Status::Deleted) {
					return AllocationId(i);
				}
			}

			nodes.push_back(AllocationNode{});
			return nodes.size() - 1;
		}

		void AllocatorPrivate::releaseNode(AllocationId nodeId) {
			nodes[nodeId].status = AllocationNode::Status::Deleted;
		}

		Allocator::Allocator(const vk::Extent2D& size, const AllocatorOptions& options)
			: d(new AllocatorPrivate) {
			d->size = size;
			d->options = options;

			d->nodes.push_back(
				AllocationNode{
					.prevSibling = AllocationId::null(),
					.nextSibling = AllocationId::null(),
					.parent = AllocationId::null(),
					.orientation = Orientation::Horizontal,
					.scissor = vk::Rect2D({0, 0}, size),
					.kind = AllocationNode::Kind::Leaf,
					.status = AllocationNode::Status::Free,
				}
			);
		}

		Allocator::~Allocator() {
		}

		vk::Extent2D Allocator::size() const {
			return d->size;
		}

		int AllocatorPrivate::computeScore(int xDelta, int yDelta) const {
			switch (options.method) {
				case AllocationMethod::PreferLessHorizontalSpace:
					return xDelta;
				case AllocationMethod::PreferLessVerticalSpace:
					return yDelta;
				default:
					return -1;
			}
		}

		std::tuple<AllocationId, bool> AllocatorPrivate::selectFreeNode(const vk::Extent2D& size) const {
			AllocationId bestCandidate = AllocationId::null();
			int bestScore = std::numeric_limits<int>::max();
			bool bestTransposed = false;

			for (int nodeId = 0; nodeId < nodes.size(); ++nodeId) {
				if (nodes[nodeId].status != AllocationNode::Status::Free ||
					nodes[nodeId].kind != AllocationNode::Kind::Leaf) {
					continue;
				}
				const vk::Extent2D availableSize = nodes[nodeId].scissor.extent;

				const int xDelta = availableSize.width - size.width;
				const int yDelta = availableSize.height - size.height;
				if (xDelta >= 0 && yDelta >= 0) {
					if (xDelta == 0 && yDelta == 0) {
						return {nodeId, false};
					}
					const int score = computeScore(xDelta, yDelta);
					if (score < bestScore) {
						bestCandidate = nodeId;
						bestScore = score;
						bestTransposed = false;
					}
				}

				if (options.allowTranspose) {
					const int xDelta = availableSize.width - size.height;
					const int yDelta = availableSize.height - size.width;
					if (xDelta >= 0 && yDelta >= 0) {
						if (xDelta == 0 && yDelta == 0) {
							return {nodeId, true};
						}
						const int score = computeScore(xDelta, yDelta);
						if (score < bestScore) {
							bestCandidate = nodeId;
							bestScore = score;
							bestTransposed = true;
						}
					}
				}
			}

			return {bestCandidate, bestTransposed};
		}

		static Orientation flipOrientation(Orientation orientation) {
			return orientation == Orientation::Horizontal ? Orientation::Vertical : Orientation::Horizontal;
		}

		static std::tuple<vk::Rect2D, vk::Rect2D, vk::Rect2D>
		guillotine(const vk::Rect2D& bounds, const vk::Extent2D& size, Orientation axis) {
			const vk::Rect2D allocatedRect(bounds.offset, size);

			vk::Rect2D leftoverRect, splitRect;
			if (axis == Orientation::Vertical) {
				leftoverRect = vk::Rect2D{{bounds.offset.x, (int32_t) (bounds.offset.y + size.height)},
										  {size.width,      bounds.extent.height - size.height}};
				splitRect = vk::Rect2D{{(int32_t) (bounds.offset.x + size.width), bounds.offset.y},
									   {bounds.extent.width - size.width,         bounds.extent.height}};
			} else {
				leftoverRect = vk::Rect2D{{(int32_t) (bounds.offset.x + size.width), bounds.offset.y},
										  {bounds.extent.width - size.width,         size.height}};

				splitRect = vk::Rect2D{{bounds.offset.x,     (int32_t) (bounds.offset.y + size.height)},
									   {bounds.extent.width, bounds.extent.height - size.height}};
			}

			return {allocatedRect, leftoverRect, splitRect};
		}

		Allocation Allocator::allocate(const vk::Extent2D& requestedSize) {
			if (isEmpty(requestedSize)) {
				return Allocation{};
			}

			const auto [selectedId, transposed] = d->selectFreeNode(requestedSize);
			if (selectedId == AllocationId::null()) {
				return Allocation{};
			}

			const vk::Extent2D adjustedSize = transposed ? transpose(requestedSize) : requestedSize;
			if (d->nodes[selectedId].scissor.extent.width == adjustedSize.width &&
				d->nodes[selectedId].scissor.extent.height == adjustedSize.height) {
				d->nodes[selectedId].status = AllocationNode::Status::Occupied;
				return Allocation{
					.scissor = d->nodes[selectedId].scissor,
					.id = selectedId,
					.transposed = transposed,
				};
			}

			const auto [allocatedRect, leftoverRect, splitRect] =
				guillotine(d->nodes[selectedId].scissor, adjustedSize, d->nodes[selectedId].orientation);

			// Note that some rectangles can be empty, avoid creating nodes for these rects.
			const AllocationId allocatedId = d->allocateNode();
			const AllocationId leftoverId =
				!isEmpty(leftoverRect.extent) ? d->allocateNode() : AllocationId::null();
			const AllocationId splitId =
				!isEmpty(splitRect.extent) ? d->allocateNode() : AllocationId::null();

			d->nodes[selectedId].kind = AllocationNode::Kind::Fork;

			const Orientation childOrientation = flipOrientation(d->nodes[selectedId].orientation);
			d->nodes[allocatedId] = AllocationNode{
				.prevSibling = AllocationId::null(),
				.nextSibling = leftoverId,
				.parent = selectedId,

				.orientation = childOrientation,
				.scissor = allocatedRect,
				.kind = AllocationNode::Kind::Leaf,
				.status = AllocationNode::Status::Occupied,
			};

			// If the requested rectangle perfectly fits the bin, i.e. there is no leftover,
			// avoid creating the leftover node.
			if (leftoverId != AllocationId::null()) {
				d->nodes[leftoverId] = AllocationNode{
					.prevSibling = allocatedId,
					.nextSibling = AllocationId::null(),
					.parent = selectedId,

					.orientation = childOrientation,
					.scissor = leftoverRect,
					.kind = AllocationNode::Kind::Leaf,
					.status = AllocationNode::Status::Free,
				};
			}

			// Avoid creating the split node if its area is empty, the leftover rect can still
			// be valid though. Note that the split node is a sibling of the parent node.
			if (splitId != AllocationId::null()) {
				d->nodes[splitId] = AllocationNode{
					.prevSibling = selectedId,
					.nextSibling = d->nodes[selectedId].nextSibling,
					.parent = d->nodes[selectedId].parent,

					.orientation = d->nodes[selectedId].orientation,
					.scissor = splitRect,
					.kind = AllocationNode::Kind::Leaf,
					.status = AllocationNode::Status::Free,
				};
				d->nodes[selectedId].nextSibling = splitId;
			}

			return Allocation{
				.scissor = allocatedRect,
				.id = allocatedId,
				.transposed = transposed,
			};
		}

		void Allocator::deallocate(AllocationId nodeId) {
			d->nodes[nodeId].status = AllocationNode::Status::Free;

			while (true) {
				// Merge the node with the next (free) sibling nodes. Note that the sibling nodes are
				// sorted along the axis where they had been split.
				while (d->nodes[nodeId].nextSibling != AllocationId::null()) {
					const AllocationId nextSibling = d->nodes[nodeId].nextSibling;
					if (d->nodes[nextSibling].kind != AllocationNode::Kind::Leaf ||
						d->nodes[nextSibling].status != AllocationNode::Status::Free) {
						break;
					}

					d->nodes[nodeId].scissor = rectUnion(d->nodes[nodeId].scissor, d->nodes[nextSibling].scissor);

					const AllocationId grandNextSibling = d->nodes[nextSibling].nextSibling;
					if (grandNextSibling != AllocationId::null()) {
						d->nodes[grandNextSibling].prevSibling = nodeId;
						d->nodes[nodeId].nextSibling = grandNextSibling;
					}

					d->releaseNode(nextSibling);
				}

				// Merge the node with the previous (free) sibling nodes. Note that sibling nodes are
				// sorted along the axis where they had been split.
				while (d->nodes[nodeId].prevSibling != AllocationId::null()) {
					const AllocationId prevSibling = d->nodes[nodeId].prevSibling;
					if (d->nodes[prevSibling].kind != AllocationNode::Kind::Leaf ||
						d->nodes[prevSibling].status != AllocationNode::Status::Free) {
						break;
					}

					d->nodes[nodeId].scissor = rectUnion(d->nodes[nodeId].scissor, d->nodes[prevSibling].scissor);

					const AllocationId grandPrevSibling = d->nodes[prevSibling].prevSibling;
					if (grandPrevSibling != AllocationId::null()) {
						d->nodes[grandPrevSibling].nextSibling = nodeId;
						d->nodes[nodeId].prevSibling = grandPrevSibling;
					}

					d->releaseNode(prevSibling);
				}

				// Stop if it's either a root node or one of the siblings is still occcupied.
				if (d->nodes[nodeId].parent == AllocationId::null() ||
					d->nodes[nodeId].prevSibling != AllocationId::null() ||
					d->nodes[nodeId].nextSibling != AllocationId::null()) {
					break;
				}

				// If the parent node has only one child, merge those two and check whether the parent
				// node can be merged with one of its siblings.
				const AllocationId parentId = d->nodes[nodeId].parent;
				d->nodes[parentId].scissor = rectUnion(d->nodes[parentId].scissor, d->nodes[nodeId].scissor);
				d->nodes[parentId].kind = AllocationNode::Kind::Leaf;
				d->nodes[parentId].status = AllocationNode::Status::Free;

				d->releaseNode(nodeId);
				nodeId = parentId;
			}
		}
	}
} // namespace KGuillotineAllocator