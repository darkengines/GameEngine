#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include <iostream>

namespace drk::Physics
{
  class BodyActivationListener final : public JPH::BodyActivationListener
  {
   public:
    BodyActivationListener() : JPH::BodyActivationListener()
    {
    }
    void OnBodyActivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override
    {
      std::cout << "A body got activated" << std::endl;
    }

    void OnBodyDeactivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override
    {
      std::cout << "A body went to sleep" << std::endl;
    }
  };
}  // namespace drk::Physics
