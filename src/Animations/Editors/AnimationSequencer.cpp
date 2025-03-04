#include "AnimationSequencer.hpp"

#include <fmt/format.h>

#include "../../Common/Components/Name.hpp"
namespace drk::Animations::Editors
{
  AnimationSequencer::AnimationSequencer(entt::registry& registry, Animations::Systems::AnimationSystem& animationSystem)
      : registry(registry),
        animationSystem(animationSystem)
  {
  }
  int AnimationSequencer::GetFrameMin() const
  {
    return 0;
  }

  int AnimationSequencer::GetFrameMax() const
  {
    return 0;
  }

  int AnimationSequencer::GetItemCount() const
  {
    const auto animationCount = registry.view<Components::Animation>().size();
    return animationCount;
  }
  const char* AnimationSequencer::GetItemTypeName(int typeIndex) const
  {
    return "Animation";
  }
  const char* AnimationSequencer::GetItemLabel(int index) const
  {
    Components::Animation* animation;
    auto animationEntity = GetAnimation(index, &animation);
    const auto& animationName = registry.try_get<Common::Components::Name>(animationEntity);
    if (!animationName)
      return fmt::format("{:d}", index).c_str();
    return animationName->name.c_str();
  }
  entt::entity AnimationSequencer::GetAnimation(int index, Components::Animation** animationPtr) const
  {
    const auto& animations = registry.group<Components::Animation>();
    int animationIndex = 0;
    auto entity = animations[index];
    Components::Animation& animation = animations.get<Components::Animation>(entity);
    *animationPtr = &animation;
    return entity;
  }
  void AnimationSequencer::Get(int index, int** start, int** end, int* type, unsigned int* color)
  {
    Components::Animation* animation;
    auto animationEntity = GetAnimation(index, &animation);
    if (start != nullptr)
      *start = (int*)&animation->start;
    if (end != nullptr)
      *end = (int*)&animation->duration;
    //if (type != nullptr)
    //  *type = 0;
    //if (color != nullptr)
    //  *color = 0xff0000ff;
  }


}  // namespace drk::Animations::Editors
