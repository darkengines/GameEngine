#include <cstdint>

namespace drk {
	struct FreeBlock {
		uint32_t offset;
		uint32_t length;
	};
}