local ReceiverBase = require("core:erm.ReceiverBase")

local HE = ReceiverBase:new()

function HE:new(p1, p2, p3)
	assert(p1 ~= nil, "!!HE requires hero identifier")

	if p2 and p3 then
		--by coordinates
		error("!!HEx/y/l: form is not implemented")
	else
		-- assume p1 is identifier
		return ReceiverBase.new(self, {id=p1})
	end
end

function HE:A(x, ...)
	logError("!!HE:A is not implemented")
end
function HE:B(x, ...)
	logError("!!HE:B is not implemented")
end
function HE:C(x, ...)
	logError("!!HE:A is not implemented")
end
function HE:D(x, ...)
	logError("!!HE:D not implemented")
end
function HE:E(x, ...)
	logError("!!HE:E is not implemented")
end
function HE:F(x, ...)
	logError("!!HE:F is not implemented")
end
function HE:G(x, ...)
	logError("!!HE:G is not implemented")
end
function HE:H(x, ...)
	logError("!!HE:H is not implemented")
end
function HE:I(x, ...)
	logError("!!HE:I is not implemented")
end
function HE:K(x, ...)
	logError("!!HE:K is not implemented")
end
function HE:L(x, ...)
	logError("!!HE:L is not implemented")
end
function HE:M(x, ...)
	logError("!!HE:M is not implemented")
end
function HE:N(x, ...)
	logError("!!HE:N is not implemented")
end
function HE:O(x, ...)
	logError("!!HE:O is not implemented")
end
function HE:P(x, ...)
	logError("!!HE:P is not implemented")
end
function HE:R(x, ...)
	logError("!!HE:R is not implemented")
end
function HE:S(x, ...)
	logError("!!HE:S is not implemented")
end
function HE:T(x, ...)
	logError("!!HE:T is not implemented")
end
function HE:U(x, ...)
	logError("!!HE:U is not implemented")
end
function HE:V(x, ...)
	logError("!!HE:V is not implemented")
end
function HE:W(x, ...)
	logError("!!HE:A is not implemented")
end
function HE:X(x, ...)
	logError("!!HE:X is not implemented")
end
function HE:Y(x, ...)
	logError("!!HE:Y is ot implemented")
end


return HE
