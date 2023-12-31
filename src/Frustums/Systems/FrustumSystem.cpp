#include "FrustumSystem.hpp"
namespace drk::Frustums::Systems {
	void FrustumSystem::update(Models::Frustum& frustumModel, const Components::Frustum& frustumComponent) {
		frustumModel.leftPlane = frustumComponent.leftPlane;
		frustumModel.rightPlane = frustumComponent.rightPlane;
		frustumModel.farPlane = frustumComponent.farPlane;
		frustumModel.nearPlane = frustumComponent.nearPlane;
		frustumModel.bottomPlane = frustumComponent.bottomPlane;
		frustumModel.topPlane = frustumComponent.topPlane;
	}
}