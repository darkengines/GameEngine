#pragma once

#include <cstdint>
#include <vector>

namespace drk::Graphics {
	template<typename TComponent>
	class SynchronizationState {
	public:
		SynchronizationState(uint32_t storeCount) : RemainingCount(storeCount), Stores(storeCount) {};
		bool Update(uint32_t index){
			if (!Stores[index]) {
				Stores[index] = true;
				RemainingCount--;
			}
			return RemainingCount;
		};

	protected:
		uint32_t RemainingCount;
		std::vector<bool> Stores;
	};
}