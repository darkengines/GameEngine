#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include "ObjectLayerPairFilter.hpp"

namespace drk::Physics
{
  namespace BroadPhaseLayers
  {
    static constexpr JPH::BroadPhaseLayer MOVING(Layers::MOVING);
    static constexpr JPH::BroadPhaseLayer NON_MOVING(Layers::NON_MOVING);
    static constexpr uint32_t LAYER_COUNT(Layers::LAYER_COUNT);
  }  // namespace BroadPhaseLayers

  class BroadPhaseLayer : public JPH::BroadPhaseLayerInterface
  {
   public:
    BroadPhaseLayer()
    {
      broadPhaseLayers[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
      broadPhaseLayers[Layers::MOVING] = BroadPhaseLayers::MOVING;
    }
    /// Return the number of broadphase layers there are
    virtual JPH::uint GetNumBroadPhaseLayers() const override
    {
      return Layers::LAYER_COUNT;
    }

    /// Convert an object layer to the corresponding broadphase layer
    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
    {
      return broadPhaseLayers[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
    {
      switch ((JPH::BroadPhaseLayer::Type)inLayer)
      {
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING: return "NON_MOVING";
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING: return "MOVING";
        default: JPH_ASSERT(false); return "INVALID";
      }
    }
#endif

   protected:
    JPH::BroadPhaseLayer broadPhaseLayers[Layers::LAYER_COUNT];
  };
}  // namespace drk::Physics
