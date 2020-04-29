/*
 * api/Creature.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "Creature.h"

#include "Registry.h"

#include "../LuaStack.h"
#include "../LuaCallWrapper.h"

namespace scripting
{
namespace api
{

VCMI_REGISTER_CORE_SCRIPT_API(CreatureProxy, "Creature");

const std::vector<CreatureProxy::RegType> CreatureProxy::REGISTER =
{
	{"getIconIndex", LuaCallWrapper<const Entity>::createFunctor(&Entity::getIconIndex)},
	{"getIndex", LuaCallWrapper<const Entity>::createFunctor(&Entity::getIndex)},
	{"getJsonKey", LuaCallWrapper<const Entity>::createFunctor(&Entity::getJsonKey)},
	{"getName", LuaCallWrapper<const Entity>::createFunctor(&Entity::getName)},
	{"accessBonuses", LuaCallWrapper<const EntityWithBonuses<CreatureID>>::createFunctor(&EntityWithBonuses<CreatureID>::accessBonuses)},

	{"getMaxHealth", LuaCallWrapper<const Creature>::createFunctor(&Creature::getMaxHealth)},
	{"getPluralName", LuaCallWrapper<const Creature>::createFunctor(&Creature::getPluralName)},
	{"getSingularName", LuaCallWrapper<const Creature>::createFunctor(&Creature::getSingularName)},

	{"getAdvMapAmountMin", LuaCallWrapper<const Creature>::createFunctor(&Creature::getAdvMapAmountMin)},
	{"getAdvMapAmountMax", LuaCallWrapper<const Creature>::createFunctor(&Creature::getAdvMapAmountMax)},
	{"getAIValue", LuaCallWrapper<const Creature>::createFunctor(&Creature::getAIValue)},
	{"getFightValue", LuaCallWrapper<const Creature>::createFunctor(&Creature::getFightValue)},
	{"getLevel", LuaCallWrapper<const Creature>::createFunctor(&Creature::getLevel)},
	{"getGrowth", LuaCallWrapper<const Creature>::createFunctor(&Creature::getGrowth)},
	{"getHorde", LuaCallWrapper<const Creature>::createFunctor(&Creature::getHorde)},
	{"getFactionIndex", LuaCallWrapper<const Creature>::createFunctor(&Creature::getFactionIndex)},

	{"getBaseAttack", LuaCallWrapper<const Creature>::createFunctor(&Creature::getBaseAttack)},
	{"getBaseDefense", LuaCallWrapper<const Creature>::createFunctor(&Creature::getBaseDefense)},
	{"getBaseDamageMin", LuaCallWrapper<const Creature>::createFunctor(&Creature::getBaseDamageMin)},
	{"getBaseDamageMax", LuaCallWrapper<const Creature>::createFunctor(&Creature::getBaseDamageMax)},
	{"getBaseHitPoints", LuaCallWrapper<const Creature>::createFunctor(&Creature::getBaseHitPoints)},
	{"getBaseSpellPoints", LuaCallWrapper<const Creature>::createFunctor(&Creature::getBaseSpellPoints)},
	{"getBaseSpeed", LuaCallWrapper<const Creature>::createFunctor(&Creature::getBaseSpeed)},
	{"getBaseShots", LuaCallWrapper<const Creature>::createFunctor(&Creature::getBaseShots)},

	{"getCost", LuaCallWrapper<const Creature>::createFunctor(&Creature::getCost)},
	{"isDoubleWide", LuaCallWrapper<const Creature>::createFunctor(&Creature::isDoubleWide)},
};

const std::vector<CreatureProxy::CustomRegType> CreatureProxy::REGISTER_CUSTOM =
{

};

}
}
