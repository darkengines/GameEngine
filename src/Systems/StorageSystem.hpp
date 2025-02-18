#pragma once

namespace drk::Systems {

class StorageSystem {
public:
	virtual void store() = 0;
	virtual void updateStore() = 0;
};
}  // namespace drk::Systems