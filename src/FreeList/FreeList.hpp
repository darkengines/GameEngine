#include <vector>
#include <list>
#include "FreeBlock.hpp"
#include <algorithm>

namespace drk {
	struct FreeList {
		const uint32_t length;
		const short levelCount;
		std::list<FreeBlock> freeList;
		std::vector<std::list<std::list<FreeBlock>::iterator>> skipLists;
	public:
		FreeBlock allocate(uint32_t length) {
			auto skipListIndex = level(length);
			auto& skipList = skipLists[skipListIndex];
			if (skipList.empty()) return {0, 0};
			auto freeListIterator = skipList.front();
			auto candidateLevel = level(freeListIterator->length);
			auto allocationOffset = freeListIterator->offset;
			freeListIterator->offset += length;
			freeListIterator->length -= length;
			auto newSkipListIndex = level(freeListIterator->length);
			for (auto index = 0; index <= newSkipListIndex; index++) {
				auto& skipList = skipLists[index];
				skipList.remove(freeListIterator);
				auto upperBound = std::upper_bound(
					skipList.begin(),
					skipList.end(),
					freeListIterator,
					freeListIteratorComparer
				);
				skipList.insert(upperBound, freeListIterator);
			}
			for (auto index = newSkipListIndex + 1; index <= candidateLevel; index++)
				skipLists[index].remove(
					freeListIterator
				);
			if (!freeListIterator->length) freeList.erase(freeListIterator);
			FreeBlock result{
				.offset = allocationOffset,
				.length = length
			};
			return result;
		}
		void free(const FreeBlock& block) {
			auto next = std::upper_bound(freeList.begin(), freeList.end(), block, blockOffsetComparer);
			std::list<FreeBlock>::iterator previous;
			auto hasNext = next != freeList.end();
			auto hasPrevious = next != freeList.begin();
			if (hasPrevious) previous = std::prev(next);

			uint16_t clipping = NoClip;
			if (hasPrevious && previous->offset + previous->length == block.offset) clipping |= ClipPrevious;
			if (hasNext && next->offset == block.offset + block.length) clipping |= ClipNext;

			if (clipping == NoClip) {
				FreeBlock freeBlock{
					.offset = block.offset,
					.length = block.length
				};
				const auto& freeListUpperBound = std::upper_bound(
					freeList.begin(),
					freeList.end(),
					block,
					blockOffsetComparer
				);
				const auto& freeListIterator = freeList.insert(freeListUpperBound, freeBlock);
				auto blockLevel = level(freeBlock.length);
				for (auto index = 0; index <= blockLevel; index++) {
					auto& skipList = skipLists[index];
					const auto& skipListUpperBound = std::upper_bound(
						skipList.begin(),
						skipList.end(),
						freeListIterator,
						freeListIteratorComparer
					);
					const auto& skipListIterator = skipList.insert(skipListUpperBound, freeListIterator);
				}
			} else {
				if (clipping == (ClipNext | ClipPrevious)) {
					previous->length += block.length + next->length;
					auto nextLevel = level(next->length);
					for (auto index = 0; index <= nextLevel; index++) {
						auto& skipList = skipLists[index];
						skipList.remove(next);
					}
					freeList.erase(next);
					auto previousLevel = level(previous->length);
					for (auto index = 0; index <= previousLevel; index++) {
						auto& skipList = skipLists[index];
						skipList.remove(previous);
						auto skipListUpperBound = std::upper_bound(
							skipList.begin(),
							skipList.end(),
							previous,
							freeListIteratorComparer
						);
						skipList.insert(skipListUpperBound, previous);
					}
				} else {
					if (clipping == ClipPrevious) {
						previous->length += block.length;
						auto previousLevel = level(previous->length);
						for (auto index = 0; index <= previousLevel; index++) {
							auto& skipList = skipLists[index];
							skipList.remove(previous);
							auto skipListUpperBound = std::upper_bound(
								skipList.begin(),
								skipList.end(),
								previous,
								freeListIteratorComparer
							);
							skipList.insert(skipListUpperBound, previous);
						}
					} else if (clipping == ClipNext) {
						next->offset = block.offset;
						next->length += block.length;
						auto nextLevel = level(next->length);
						for (auto index = 0; index <= nextLevel; index++) {
							auto& skipList = skipLists[index];
							skipList.remove(next);
							auto skipListUpperBound = std::upper_bound(
								skipList.begin(),
								skipList.end(),
								next,
								freeListIteratorComparer
							);
							skipList.insert(skipListUpperBound, next);
						}
					}
				}
			}
		}
		static FreeList create(uint32_t length) {
			const short levelCount = level(length) + 1;
			FreeList freeList{
				.length = length,
				.levelCount = levelCount,
				.freeList = {{.offset = 0, .length = length}},
				.skipLists = {}
			};
			freeList.skipLists.resize(freeList.levelCount);
			auto skipListIndex = 0;
			while (skipListIndex < freeList.levelCount) {
				freeList.skipLists[skipListIndex].emplace_back(freeList.freeList.begin());
				skipListIndex++;
			}
			return freeList;
		}
	protected:
		static short level(uint32_t n) {
			short result = -1;
			while ((n >>= 1) > 0) result++;
			return result;
		}
		static bool blockOffsetComparer(const FreeBlock& left, const FreeBlock& right) {
			return left.offset < right.offset;
		}
		static bool freeListIteratorComparer(
			const std::list<FreeBlock>::iterator& left,
			const std::list<FreeBlock>::iterator& right
		) {
			return left->length < right->length;
		}
		static const uint16_t NoClip = 0u;
		static const uint16_t ClipPrevious = 1u;
		static const uint16_t ClipNext = 2u;
	};
}