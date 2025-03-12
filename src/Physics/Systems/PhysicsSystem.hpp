#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <entt/entt.hpp>
#include <Jolt/Core/JobSystemThreadPool.h>

namespace drk::Physics::Systems
{
  class PhysicsSystem
  {
   protected:
    JPH::PhysicsSystem& joltPhysicsSystem;
    JPH::TempAllocatorImpl& tempAllocator;
    JPH::JobSystemThreadPool& jobSystemThreadPool;
   public:
    PhysicsSystem(JPH::PhysicsSystem& joltPhysicsSystem, JPH::TempAllocatorImpl& tempAllocator, JPH::JobSystemThreadPool& jobSystemThreadPool);
    void addBodies(entt::registry& registry);
    void syncNodeToPhysics(entt::registry& registry);
    void stepPhysics(float deltaTime);
    void syncPhysicsToNode(entt::registry& registry);
  };
}  // namespace drk::Physics::Systems
