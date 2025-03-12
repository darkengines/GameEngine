#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

namespace drk::Physics::Components
{
  struct Body
  {
    JPH::EMotionType motionType = JPH::EMotionType::Dynamic;
    JPH::ShapeRefC shape;
    float mass = 1.0f;
    uint16_t collisionLayer = 0;
  };
}  // namespace drk::Physics::Components
