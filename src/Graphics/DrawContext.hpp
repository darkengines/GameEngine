#pragma once

#include <vector>
#include "../Devices/Buffer.hpp"
#include "DrawSet.hpp"

namespace drk::Graphics {
	struct DrawContext {
		std::vector<DrawSet> drawSets;
	};
}
