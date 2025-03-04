#pragma once

#include <imgui.h>
#include <ImGuizmo.h>
#include <ImSequencer.h>

#include <entt/entt.hpp>

#include "../Systems/AnimationSystem.hpp"

namespace drk::Animations::Editors
{
  class AnimationSequencer : public ImSequencer::SequenceInterface
  {
   protected:
    entt::registry& registry;
    Animations::Systems::AnimationSystem& animationSystem;

   public:
    AnimationSequencer(entt::registry& registry, Animations::Systems::AnimationSystem& animationSystem);
    // Inherited via SequenceInterface
    int GetFrameMin() const override;
    int GetFrameMax() const override;
    int GetItemCount() const override;
    const char* GetItemTypeName(int typeIndex) const;
    const char* GetItemLabel(int index) const;
    entt::entity GetAnimation(int index, Components::Animation** animation) const;
    void Get(int index, int** start, int** end, int* type, unsigned int* color) override;
  };
}
