#include <vector>
#include "./NodeAnimation.hpp"
#include "./MeshAnimation.hpp"
#include "./MeshMorphAnimation.hpp"

namespace drk::Animations::Components {
	struct Animation {
		double duration;
		double ticksPerSecond;
		std::vector<NodeAnimation> nodeAnimations;
		std::vector<MeshAnimation> meshAnimations;
		std::vector<MeshMorphAnimation> meshMorphAnimations;
	};
}