/*
 * api/Spell.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "Spell.h"

#include "Registry.h"

#include "../LuaStack.h"
#include "../LuaCallWrapper.h"

namespace scripting
{
namespace api
{
using ::spells::Spell;

VCMI_REGISTER_CORE_SCRIPT_API(SpellProxy, "Spell");

//TODO:calculateDamage,forEachSchool

const std::vector<SpellProxy::RegType> SpellProxy::REGISTER =
{
	{"getIconIndex", LuaCallWrapper<const Entity>::createFunctor(&Entity::getIconIndex)},
	{"getIndex", LuaCallWrapper<const Entity>::createFunctor(&Entity::getIndex)},
	{"getJsonKey", LuaCallWrapper<const Entity>::createFunctor(&Entity::getJsonKey)},
	{"getName", LuaCallWrapper<const Entity>::createFunctor(&Entity::getName)},
	{"isAdventure", LuaCallWrapper<const Spell>::createFunctor(&Spell::isAdventure)},
	{"isCombat", LuaCallWrapper<const Spell>::createFunctor(&Spell::isCombat)},
	{"isCreatureAbility", LuaCallWrapper<const Spell>::createFunctor(&Spell::isCreatureAbility)},
	{"isPositive", LuaCallWrapper<const Spell>::createFunctor(&Spell::isPositive)},
	{"isNegative", LuaCallWrapper<const Spell>::createFunctor(&Spell::isNegative)},
	{"isNeutral", LuaCallWrapper<const Spell>::createFunctor(&Spell::isNeutral)},
	{"isDamage", LuaCallWrapper<const Spell>::createFunctor(&Spell::isDamage)},
	{"isOffensive", LuaCallWrapper<const Spell>::createFunctor(&Spell::isOffensive)},
	{"isSpecial", LuaCallWrapper<const Spell>::createFunctor(&Spell::isSpecial)},

	{"getCost", LuaCallWrapper<const Spell>::createFunctor(&Spell::getCost)},
	{"getBasePower", LuaCallWrapper<const Spell>::createFunctor(&Spell::getBasePower)},
	{"getLevelPower", LuaCallWrapper<const Spell>::createFunctor(&Spell::getLevelPower)},
	{"getLevelDescription", LuaCallWrapper<const Spell>::createFunctor(&Spell::getLevelDescription)},
};

const std::vector<SpellProxy::CustomRegType> SpellProxy::REGISTER_CUSTOM =
{

};

}
}
