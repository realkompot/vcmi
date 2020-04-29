/*
 * api/HeroType.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "HeroType.h"

#include "Registry.h"

#include "../LuaStack.h"
#include "../LuaCallWrapper.h"

namespace scripting
{
namespace api
{

VCMI_REGISTER_CORE_SCRIPT_API(HeroTypeProxy, "HeroType");

const std::vector<HeroTypeProxy::RegType> HeroTypeProxy::REGISTER =
{
	{"getIconIndex", LuaCallWrapper<const Entity>::createFunctor(&Entity::getIconIndex)},
	{"getIndex", LuaCallWrapper<const Entity>::createFunctor(&Entity::getIndex)},
	{"getJsonKey", LuaCallWrapper<const Entity>::createFunctor(&Entity::getJsonKey)},
	{"getName", LuaCallWrapper<const Entity>::createFunctor(&Entity::getName)},
};

const std::vector<HeroTypeProxy::CustomRegType> HeroTypeProxy::REGISTER_CUSTOM =
{

};


}
}
