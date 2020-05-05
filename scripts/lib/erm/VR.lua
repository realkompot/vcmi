local ReceiverBase = require("core:erm.ReceiverBase")

local VR = ReceiverBase:new()

function VR:new(v)
	assert(v ~= nil, "!!VR requires variable identifier")
	return ReceiverBase.new(self, {v=v})
end

local match = string.match

local function trim(s)
   return match(s,'^()%s*$') and '' or match(s,'^%s*(.*%S)')
end

function VR:H(flagIndex)
	local v = trim(self.v)
	self.ERM.F[flagIndex] = v ~= ''
end


return VR
