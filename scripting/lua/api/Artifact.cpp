/*
 * api/Artifact.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "Artifact.h"

#include "Registry.h"

#include "../LuaStack.h"
#include "../LuaCallWrapper.h"

namespace scripting
{
namespace api
{

VCMI_REGISTER_CORE_SCRIPT_API(ArtifactProxy, "Artifact");

const std::vector<ArtifactProxy::RegType> ArtifactProxy::REGISTER =
{
	{"getIconIndex", LuaCallWrapper<const Entity>::createFunctor(&Entity::getIconIndex)},
	{"getIndex", LuaCallWrapper<const Entity>::createFunctor(&Entity::getIndex)},
	{"getJsonKey", LuaCallWrapper<const Entity>::createFunctor(&Entity::getJsonKey)},
	{"getName", LuaCallWrapper<const Entity>::createFunctor(&Entity::getName)},
	{"accessBonuses", LuaCallWrapper<const EntityWithBonuses<ArtifactID>>::createFunctor(&EntityWithBonuses<ArtifactID>::accessBonuses)},
	{"getDescription", LuaCallWrapper<const Artifact>::createFunctor(&Artifact::getDescription)},
	{"getEventText", LuaCallWrapper<const Artifact>::createFunctor(&Artifact::getEventText)},
	{"isBig", LuaCallWrapper<const Artifact>::createFunctor(&Artifact::isBig)},
	{"isTradable", LuaCallWrapper<const Artifact>::createFunctor(&Artifact::isTradable)},
	{"getPrice", LuaCallWrapper<const Artifact>::createFunctor(&Artifact::getPrice)},
};

const std::vector<ArtifactProxy::CustomRegType> ArtifactProxy::REGISTER_CUSTOM =
{

};


}
}
