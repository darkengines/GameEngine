#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>

namespace drk::Physics
{
  namespace Layers
  {
    static constexpr JPH::ObjectLayer MOVING = 0;
    static constexpr JPH::ObjectLayer NON_MOVING = 1;
    static constexpr JPH::ObjectLayer LAYER_COUNT = 2;
  }  // namespace Layers

  class ObjectLayerPairFilter : public JPH::ObjectLayerPairFilter
  {
   public:
    virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
    {
      switch (inObject1)
      {
        case drk::Physics::Layers::NON_MOVING:
          return inObject2 == drk::Physics::Layers::MOVING;  // Non moving only collides with moving
        case drk::Physics::Layers::MOVING: return true;      // Moving collides with everything
        default: JPH_ASSERT(false); return false;
      }
    }
  };
}  // namespace drk::Physics
