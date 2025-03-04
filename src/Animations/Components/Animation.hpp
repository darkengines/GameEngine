#pragma once
#include <vector>

#include "./MeshAnimation.hpp"
#include "./MeshMorphAnimation.hpp"
#include "./NodeAnimation.hpp"

namespace drk::Animations::Components
{
  struct Animation
  {
    double start = .0f;
    double duration;
    double ticksPerSecond;
    std::vector<NodeAnimation> nodeAnimations;
    std::vector<MeshAnimation> meshAnimations;
    std::vector<MeshMorphAnimation> meshMorphAnimations;
  };
}  // namespace drk::Animations::Components
