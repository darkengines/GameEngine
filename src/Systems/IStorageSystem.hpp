#pragma once

#include <vector>

namespace drk::Systems
{
  class IStorageSystem
  {
   public:
    virtual void store() = 0;
    virtual void updateStore() = 0;
    inline static std::vector<IStorageSystem*> storageSystems;
  };
}  // namespace drk::Systems
