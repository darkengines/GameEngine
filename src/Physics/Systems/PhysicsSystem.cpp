#include "PhysicsSystem.hpp"
#include "../../Common/Components/Dirty.hpp"
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include "../../Spatials/Components/Spatial.hpp"
#include "../Components/Body.hpp"

namespace drk::Physics::Systems
{
  PhysicsSystem::PhysicsSystem(JPH::PhysicsSystem& joltPhysicsSystem,
      JPH::TempAllocatorImpl& tempAllocator,
      JPH::JobSystemThreadPool& jobSystemThreadPool)
      : joltPhysicsSystem(joltPhysicsSystem),
        tempAllocator(tempAllocator),
        jobSystemThreadPool(jobSystemThreadPool)
  {
  }
  void PhysicsSystem::addBodies(entt::registry& registry)
  {
    auto view = registry.view<Components::Body>(entt::exclude<JPH::BodyID>);

    for (auto entity : view)
    {
      auto& bodyComp = registry.get<Components::Body>(entity);

      // Get spatial data if exists
      JPH::Vec3 position = JPH::Vec3(0, 0, 0);
      JPH::Quat rotation = JPH::Quat::sIdentity();

      if (registry.all_of<Spatials::Components::Spatial<Spatials::Components::Absolute>>(entity))
      {
        auto& spatial = registry.get<Spatials::Components::Spatial<Spatials::Components::Absolute>>(entity);
        position = JPH::Vec3(spatial.position.x, spatial.position.y, spatial.position.z);
        //rotation = JPH::Quat(spatial.rotation.w, spatial.rotation.x, spatial.rotation.y, spatial.rotation.z);
      }

      // Create body settings
      JPH::BodyCreationSettings settings(bodyComp.shape, position, rotation, bodyComp.motionType, bodyComp.collisionLayer);

      if (bodyComp.motionType == JPH::EMotionType::Dynamic)
      {
        settings.mMassPropertiesOverride.mMass = bodyComp.mass;
      }
	  auto &bodyInterface = joltPhysicsSystem.GetBodyInterface();
      JPH::Body* body = bodyInterface.CreateBody(settings);
      if (body)
      {
        JPH::BodyID bodyID = body->GetID();
        bodyInterface.AddBody(bodyID, JPH::EActivation::Activate);

        // Store BodyID in ECS
        registry.emplace<JPH::BodyID>(entity, bodyID);
      }
    }
  }

  void PhysicsSystem::syncNodeToPhysics(entt::registry& registry)
  {
    auto view = registry.view<Spatials::Components::Spatial<Spatials::Components::Relative>, JPH::BodyID>();

    for (auto entity : view)
    {
	  auto &spatial = view.get<Spatials::Components::Spatial<Spatials::Components::Relative>>(entity);
      auto& bodyID = view.get<JPH::BodyID>(entity);

      JPH::Vec3 joltPos(spatial.position.x, spatial.position.y, spatial.position.z);
	  JPH::Quat joltRot(spatial.rotation.x, spatial.rotation.y, spatial.rotation.z, spatial.rotation.w);
	  auto &bodyInterface = joltPhysicsSystem.GetBodyInterface();
      bodyInterface.SetPositionAndRotation(bodyID, joltPos, joltRot, JPH::EActivation::Activate);
    }
  }

  void PhysicsSystem::stepPhysics(float deltaTime)
  {
    joltPhysicsSystem.Update(deltaTime, 1, &tempAllocator, &jobSystemThreadPool);
  }

  void PhysicsSystem::syncPhysicsToNode(entt::registry& registry)
  {
    auto view = registry.view<Spatials::Components::Spatial<Spatials::Components::Relative>, JPH::BodyID>();

    for (auto entity : view)
    {
      auto& spatial = registry.get<Spatials::Components::Spatial<Spatials::Components::Relative>>(entity);
      auto& bodyID = registry.get<JPH::BodyID>(entity);

      JPH::Vec3 joltPos;
      JPH::Quat joltRot;
	  auto& bodyInterface = joltPhysicsSystem.GetBodyInterface();
      bodyInterface.GetPositionAndRotation(bodyID, joltPos, joltRot);

      spatial.position = glm::vec4(joltPos.GetX(), joltPos.GetY(), joltPos.GetZ(), 1.0f);
      spatial.rotation = glm::quat(joltRot.GetW(), joltRot.GetX(), joltRot.GetY(), joltRot.GetZ());

      registry.emplace_or_replace<Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Relative>>>(entity);

    }
  }
}  // namespace drk::Physics::Systems
