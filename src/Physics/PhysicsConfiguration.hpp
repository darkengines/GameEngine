#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace drk::Physics
{
  struct PhysicsConfiguration
  {
    uint32_t TempAllocatorSize = 10485760;
    uint32_t MaxBodyCount = 1024;
    uint32_t BodyMutexCount = 0;
    uint32_t MaxBodyPairCount = 1024;
    uint32_t MaxContactConstraintCount = 1024;
  };
  void to_json(nlohmann::json& j, const PhysicsConfiguration& p);
  void from_json(const nlohmann::json& j, PhysicsConfiguration& p);
}  // namespace drk::Physics
