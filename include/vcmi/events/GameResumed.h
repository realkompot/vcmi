/*
 * GameResumed.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#pragma once

#include "Event.h"
#include "SubscriptionRegistry.h"

namespace events
{

class DLL_LINKAGE GameResumed : public Event
{
public:
	using PreHandler = SubscriptionRegistry<GameResumed>::PreHandler;
	using PostHandler = SubscriptionRegistry<GameResumed>::PostHandler;
	using ExecHandler = SubscriptionRegistry<GameResumed>::ExecHandler;
	using BusTag = SubscriptionRegistry<GameResumed>::BusTag;

	static SubscriptionRegistry<GameResumed> * getRegistry();

	static void defaultExecute(const EventBus * bus);

	friend class SubscriptionRegistry<GameResumed>;
};

}
