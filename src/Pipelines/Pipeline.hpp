#pragma once
#include <vector>
#include <memory>
#include "../Draws/Draw.hpp"

namespace drk::Pipelines {
	class Pipeline {
		virtual std::vector<Draws::DrawSet> PrepareDraws() = 0;
	};
}


