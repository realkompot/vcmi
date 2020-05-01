
local _G=_G
local ipairs=ipairs
local select=select
local pairs=pairs
local type = type
local unpack=unpack
local logError = logError

local DATA = DATA

--/////////////////////////

local function table_print (tt, done)
  done = done or {}
  if type(tt) == "table" then
    local sb = {}
    table.insert(sb, "{");
    for key, value in pairs (tt) do
      if type (value) == "table" and not done [value] then
        done [value] = true
        table.insert(sb, key .. ":{");
        table.insert(sb, table_print (value, done))
        table.insert(sb, "}");
      else
        table.insert(sb, string.format(
            "%s:\"%s\"", tostring (key), tostring(value)))
       end
       table.insert(sb, ",");
    end
    table.insert(sb, "}");

    m = getmetatable(tt);
    if m and m.__index then
		table.insert(sb, table_print (m.__index, done));
    end

    return table.concat(sb)
  else
    return tt .. ""
  end
end

local function to_string( tbl )
    if  "nil"       == type( tbl ) then
        return tostring(nil)
    elseif  "table" == type( tbl ) then
        return table_print(tbl)
    elseif  "string" == type( tbl ) then
        return tbl
    else
        return tostring(tbl)
    end
end

--/////////////////////////

local VERM = {}

local function createEnv(parent, current)
	return setmetatable(
		current or {},
		{
			__parent = parent,
			__index = parent,
			__newindex = function(t, k ,v)
				if type(k) ~= "string" then
					error("String key for env. table required, but got:"..to_string(k))
				end

				local function setOnFirstHit(t, k, v)
					local vv = rawget(t, k)
					if vv~= nil then rawset(t, k, v); return true end

					local m = getmetatable(t)

					if not m then return false end --assume top

					local p = m.__parent

					if not p then
						return false
					else
						return setOnFirstHit(p, k, v)
					end
				end

				if not setOnFirstHit(t, k, v) then
					rawset(t, k, v)
				end
			end
		}
	)
end

local function isNIL(v)
	return (type(v) == "table") and (next(v) == nil)
end

local function prognForm(e, ...)
	--eval each argument, return last result

	local argc = select('#',...)

	if argc == 0 then return {}	end

	for n = 1, argc - 1 do
		VERM:eval(select(n,...), e)
	end

	return VERM:eval(select(argc,...), e)
end

local function lambdaOrMacro(e, isMacro, args, ...)

	--TODO: get rid of pack-unpack
	local body = {...}
	local oldEnv = e

	local ret = function(e, ...)

	-- we need a function that have parameters with names from `args` table
	-- pack parameters from '...' and bind to new environment

		local newEnv = createEnv(oldEnv, {})

		for i, v in ipairs(args) do
			local p = select(i,...)
			if isMacro then
				newEnv[v] = p
			else
				newEnv[v] = VERM:evalValue(p, e)
			end
		end
		if isMacro then
			local buffer = {}
			for _, v in ipairs(body) do
				table.insert(buffer, (VERM:eval(v, newEnv)))
			end
			return prognForm(newEnv, unpack(buffer))
		else
			return prognForm(newEnv, unpack(body))
		end

	end

	return ret
end

local function lambdaForm(e, args, ...)
	return lambdaOrMacro(e, false, args,  ...)
end

local function defunForm(e, name, args, ...)
	local ret = lambdaOrMacro(e, false, args, ...)
	e[name] = ret
	return ret
end

local function defmacroForm(e, name, args, ...)
	local ret = lambdaOrMacro(e, true, args, ...)
	e[name] = ret
	return ret
end

local function backquoteEval(e, v)
	if isNIL(v) then
		return v
	elseif type(v) == "table" then
		local car = v[1]

		if car == "comma" then
			return VERM:evalValue(v[2], e)
		else
			local ret = {}

			for _, v in ipairs(v) do
				table.insert(ret, (backquoteEval(e, v)))
			end
			return ret
		end
	else
		return v
	end
end

local specialForms =
{
	["<"] = function(e, lhs, rhs)
		lhs = VERM:evalValue(lhs, e)
		rhs = VERM:evalValue(rhs, e)

		if lhs < rhs then
			return lhs
		else
			return {}
		end
	end,
	["<="] = function(e, lhs, rhs)
		lhs = VERM:evalValue(lhs, e)
		rhs = VERM:evalValue(rhs, e)
		if lhs <= rhs then
			return lhs
		else
			return {}
		end
	end,
	[">"] = function(e, lhs, rhs)
		lhs = VERM:evalValue(lhs, e)
		rhs = VERM:evalValue(rhs, e)
		if lhs > rhs then
			return lhs
		else
			return {}
		end
	end,
	[">="] = function(e, lhs, rhs)
		lhs = VERM:evalValue(lhs, e)
		rhs = VERM:evalValue(rhs, e)
		if lhs >= rhs then
			return lhs
		else
			return {}
		end
	end,
	["="] = function(e, lhs, rhs)
		lhs = VERM:evalValue(lhs, e)
		rhs = VERM:evalValue(rhs, e)
		if lhs == rhs then
			return lhs
		else
			return {}
		end
	end,
	["+"] = function(e, ...)
		local ret = 0
		for n=1,select('#',...) do
			local v = VERM:evalValue((select(n,...)), e)
			ret = ret + v
		end
		return ret
	end,
	["*"] = function(e, ...)
		local ret = 1
		for n=1,select('#',...) do
			local v = VERM:evalValue((select(n,...)), e)
			ret = ret * v
		end
		return ret
	end,
	["-"] = function(e, lhs, rhs)
		lhs = VERM:evalValue(lhs, e)
		rhs = VERM:evalValue(rhs, e)
		return lhs - rhs
	end,
	["/"] = function(e, lhs, rhs)
		lhs = VERM:evalValue(lhs, e)
		rhs = VERM:evalValue(rhs, e)
		return lhs / rhs
	end,
	["%"] = function(e, lhs, rhs)
		lhs = VERM:evalValue(lhs, e)
		rhs = VERM:evalValue(rhs, e)
		return lhs % rhs
	end,

--	["comma-unlist"] = function(e, ...) end,
	["backquote"] = function(e, v)
		return backquoteEval(e, v)
	end,
--	["comma"] = function(e, ...) end,
--	["get-func"] = function(e, ...) end,
	["quote"] = function(e, v)
		return v
	end,
	["if"] = function(e, cond, v1, v2)
		cond = VERM:evalValue(cond, e)

		if isNIL(cond) then
			return VERM:evalValue(v2, e)
		else
			return VERM:evalValue(v1, e)
		end
	end,
--	["set"] = function(e, ...) end,
--	["setf"] = function(e, ...) end,
	["setq"] = function(e, name, value)
		e[name] = VERM:evalValue(value, e)
	end,
	["lambda"] = lambdaForm,
	["defun"] = defunForm,
	["progn"] = prognForm,
	["defmacro"] = defmacroForm,
	["do"] = function(e, cond, body)
		local c = VERM:eval(cond, e)

		while not isNIL(c) do

			VERM:eval(body, e)
			c = VERM:eval(cond, e)

		end
		return {}
	end,
	["car"] = function(e, ...) end,
	["cdr"] = function(e, ...) end,

	["setq-erm"] = function(e, var, varIndex, value)
		DATA.ERM[VERM:evalValue(var, e)][tostring(VERM:evalValue(varIndex, e))] = VERM:evalValue(value, e)
	end,
}

function VERM:evalValue(v, e)
	if isNIL(v) then
		return v
	elseif type(v) == "table" then
		return self:eval(v, e)
	elseif type(v) == "string" then
		return e[v] or v
	elseif type(v) == "function" then
		error("evalValue do not accept functions")
	else
		return v
    end
end

function VERM:eval(t, e)
	if type(t) ~= "table" then
		logError("Not valid form: ".. to_string(t))
		return {}
	end

	e = e or self.topEnv

	local car = t[1]
	local origCar = car

	if type(car) == "string" then
		car = e[car]
	end

	if type(car) == "table" then
		car = self:eval(car, e)
	end

	if type(car) == "function" then
		return car(e, unpack(t,2))
	else
		logError(to_string(t) .. " is not callable. Car()="..to_string(car))
		logError("Env:"..to_string(e))
		return {}
	end
end

VERM.topEnv = createEnv(specialForms)

return VERM

