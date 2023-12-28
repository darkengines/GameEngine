#pragma once

#include <boost/di.hpp>
#include "Systems/RelationshipSystem.hpp"
namespace drk::Relationships {
	auto AddRelationships() {
		return boost::di::make_injector(
			boost::di::bind<Systems::RelationshipSystem>.to<Systems::RelationshipSystem>()
		);
	}
}