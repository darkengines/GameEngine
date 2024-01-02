#include "../../Stores/StoreItemLocation.hpp"
namespace drk::Draws::Models {
	struct Draw {
		alignas(8)  Stores::Models::StoreItemLocation geometryItemLocation;
		alignas(8) Stores::Models::StoreItemLocation objectItemLocation;
	};
}