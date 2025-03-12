#include "PhysicsConfiguration.hpp"

#include <nameof.hpp>

namespace drk::Physics
{
  void to_json(nlohmann::json& j, const PhysicsConfiguration& p)
  {
    j = nlohmann::json{ { NAMEOF(p.TempAllocatorSize), p.TempAllocatorSize } };
    j = nlohmann::json{ { NAMEOF(p.MaxBodyCount), p.MaxBodyCount } };
    j = nlohmann::json{ { NAMEOF(p.BodyMutexCount), p.BodyMutexCount } };
    j = nlohmann::json{ { NAMEOF(p.MaxBodyPairCount), p.MaxBodyPairCount } };
    j = nlohmann::json{ { NAMEOF(p.MaxContactConstraintCount), p.MaxContactConstraintCount } };
  }
  void from_json(const nlohmann::json& j, PhysicsConfiguration& p)
  {
    j.at(NAMEOF(p.TempAllocatorSize).c_str()).get_to(p.TempAllocatorSize);
    j.at(NAMEOF(p.MaxBodyCount).c_str()).get_to(p.MaxBodyCount);
    j.at(NAMEOF(p.BodyMutexCount).c_str()).get_to(p.BodyMutexCount);
    j.at(NAMEOF(p.MaxBodyPairCount).c_str()).get_to(p.MaxBodyPairCount);
    j.at(NAMEOF(p.MaxContactConstraintCount).c_str()).get_to(p.MaxContactConstraintCount);
  }
}  // namespace drk::Physics
