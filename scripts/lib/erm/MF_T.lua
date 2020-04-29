local TriggerBase = require("core:erm.TriggerBase")
local ApplyDamage = require("events.ApplyDamage")

local trigger = TriggerBase:new()

local eventBus = EVENT_BUS;

local beforeApplyDamage = function(event)
	trigger:call(event)
end

local sub = ApplyDamage.subscribeBefore(eventBus, beforeApplyDamage)

if type(sub) == "string" then
	error("ApplyDamage subscription failed: "..sub)
elseif type(sub) ~= "userdata" then
	error("ApplyDamage subscription failed")
end

return trigger
