/*
 * Registry.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "Registry.h"

#ifdef VCMI_EMSCRIPTEN
	#include "Obstacle.h"
	#include "Catapult.h"
	#include "Dispel.h"
	#include "RemoveObstacle.h"
	#include "Heal.h"
	#include "Sacrifice.h"
	#include "Teleport.h"
	#include "Clone.h"
	#include "Summon.h"
#endif

namespace spells
{
namespace effects
{

// These functions are not called, they are here to workaround emscripten's dead code elimination.
#ifdef VCMI_EMSCRIPTEN
	Obstacle * unusedFuncObstacle() { return new Obstacle(); }
	Catapult * unusedFuncCatapult() { return new Catapult(); }
	Dispel * unusedFuncDispel() { return new Dispel(); }
	RemoveObstacle * unusedFuncRemoveObstacle() { return new RemoveObstacle(); }
	Heal * unusedFuncHeal() { return new Heal(); }
	Sacrifice * unusedFuncSacrifice() { return new Sacrifice(); }
	Teleport * unusedFuncTeleport() { return new Teleport(); }
	Clone * unusedFuncClone() { return new Clone(); }
	Summon * unusedFuncSummon() { return new Summon(); }
#endif

namespace detail
{
class RegistryImpl : public Registry
{
	using DataPtr = std::shared_ptr<IEffectFactory>;
public:
	RegistryImpl() = default;
	~RegistryImpl() = default;

	const IEffectFactory * find(const std::string & name) const override
	{
		auto iter = data.find(name);
		if(iter == data.end())
			return nullptr;
		else
			return iter->second.get();
	}

	void add(const std::string & name, IEffectFactory * item) override
	{
		data[name].reset(item);
	}

private:
	std::map<std::string, DataPtr> data;
};

}

Registry::Registry() = default;

Registry::~Registry() = default;

Registry * Registry::get()
{
	static std::unique_ptr<Registry> Instance = make_unique<detail::RegistryImpl>();
	return Instance.get();
}

}
}
