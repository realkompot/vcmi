local ReceiverBase = {}

function ReceiverBase:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

return ReceiverBase

