DATA = DATA or {}
local DATA = DATA

DATA.ERM = DATA.ERM or {}

DATA.ERM.F = DATA.ERM.F or {}
DATA.ERM.MDATA = DATA.ERM.MDATA or {}
DATA.ERM.Q = DATA.ERM.Q or {}
DATA.ERM.v = DATA.ERM.v or {}
DATA.ERM.z = DATA.ERM.z or {}

local ERM =
{
	M = {}
}

local ERM_MT =
{
	__index = DATA.ERM
}

setmetatable(ERM, ERM_MT)

local MACROS_MT =
{
	__index = function(M, key)
		address = rawget(ERM.MDATA, key)
		assert(address, "Macro "..key.." is not defined")
		return ERM[address.name][address.index]
	end,
	__newindex = function(M, key, value)
		address = rawget(ERM.MDATA, key)
		assert(address, "Macro "..key.." is not defined")
		ERM[address.name][address.index] = value
	end
}

setmetatable(ERM.M, MACROS_MT)

function ERM:addMacro(name, varName, varIndex)
	assert(varName == "v" or varName == "z", "Macro for "..varName.. "[...] is not allowed")
	rawset(self.MDATA, name, {name = varName, index = varIndex})
end

local y = {}

ERM.getY = function(key)
	y[key] = y[key] or {}
	return y[key]
end

local Receivers = {}

local function createReceiverLoader(name)
	local loader = function(...)
		Receivers[name] = Receivers[name] or require("core:erm."..name)
		local o = Receivers[name]:new(...)
		o.ERM = ERM
		return o
	end
	return loader
end

--[[

AR
BA
BF
BG
BH
CA
CD
CE
CM
DL
CO
EA
EX
GE
HE
HL
HO
HT
LE
MO
MR
MW
OB
OW
PM
PO
QW
SS
TL
TM
TR
UN

]]

ERM.BM = createReceiverLoader("BM")
ERM.BU = createReceiverLoader("BU")
ERM.IF = createReceiverLoader("IF")
ERM.MA = createReceiverLoader("MA")
ERM.MF = createReceiverLoader("MF")
ERM.VR = createReceiverLoader("VR")

local Triggers = {}

local function createTriggerLoader(name)
	local loader = function(...)
		Triggers[name] = Triggers[name] or require("core:erm."..name.."_T")
		Triggers[name].ERM = ERM
		return Triggers[name]
	end
	return loader
end

--[[
!?AE
!?BA
!?BF
!?BG
!?BR
!?CM client only?
!?CO
!?DL client only?
!?GE
!?GM
!?HE
!?HL
!?HM
!?LE (!$LE)
!?MG
!?MM client only?
!?MR
!?MW
!?OB (!$OB)
!?SN client only?
!?TH client only?
!?TL client only? depends on time limit feature
!?TM
]]

local TriggerLoaders = {}

TriggerLoaders.PI = createTriggerLoader("PI")
TriggerLoaders.MF = createTriggerLoader("MF")


function ERM:addTrigger(t)
	local name = t.name
	local fn = t.fn

	local trigger = TriggerLoaders[name]()

	table.insert(trigger.fn, fn)
end

function ERM:callInstructions(cb)
	if not DATA.ERM.instructionsCalled then
		cb()
		self:callTrigger("PI")
		DATA.ERM.instructionsCalled = true
	end
end

function ERM:callTrigger(name)
	local trigger = TriggerLoaders[name]()
	trigger:call()
end

return ERM
