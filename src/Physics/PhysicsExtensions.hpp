#pragma once
#include <fruit/fruit.h>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include "../Configuration/Extensions.hpp"
#include "Systems/PhysicsSystem.hpp"

namespace drk::Physics
{
  void TraceImpl(const char *inFMT, ...);
  bool AssertFailedImpl(const char *inExpression, const char *inMessage, const char *inFile, uint32_t inLine);
  int test();
  fruit::Component<Systems::PhysicsSystem> addPhysics();
}  // namespace drk::Physics
