local TriggerBase = require("core:erm.TriggerBase")
local GameResumed = require("events.GameResumed")

local trigger = TriggerBase:new()

local sub

local afterGameResumed = function(event)
	trigger:call(event)
end

function trigger:setId(id)
	TriggerBase.setId(self, id)

	local id1 = self.id[1]

	if id1 == 0 or id1 == "0" then
		sub = GameResumed.subscribeAfter(EVENT_BUS, afterGameResumed)
		self:checkSub(sub, "GameResumed")
	else
		error ("Identifier "..id1 .. " not supported by !?GM")
	end
end

return trigger
