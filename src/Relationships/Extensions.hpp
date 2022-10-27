#pragma once

#include <boost/di.hpp>
#include "RelationshipSystem.hpp"
namespace drk::Relationships {
	auto AddRelationships() {
		return boost::di::make_injector(
			boost::di::bind<RelationshipSystem>.to<RelationshipSystem>()
		);
	}
}