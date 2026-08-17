-- TODO: have central wheel click + scroll do something different

local shifting = false

controller.bind("shift", function(value)
	shifting = value >= 0.5
end)

controller.bind("modwheel", function(value)
	-- Shift + ModWheel = Expression Wheel
	if shifting then
		inputPlayer.updateParam("_midi_cc_11", value)
	else
		inputPlayer.updateParam("_midi_cc_1", value)
	end
end)

controller.bind("centralWheel", function(value)
	-- Central Wheel = Patch / Library Cursor
	if value >= 0.5 then
		patchExplorer.moveBy(1)
	else
		patchExplorer.moveBy(-1)
	end
end)

controller.bind("centralWheel_shift", function(value)
	-- Shift + Central Wheel = Skip to End / Start of Folder / Library
	if value >= 0.5 then
		patchExplorer.moveTo(-1)
	else
		patchExplorer.moveTo(1)
	end
end)

controller.bind("centralWheel_click", function(value)
	-- Click Central Wheel = Load Patch / Open Library / Folder
	if value >= 0.5 then
		patchExplorer.load()
	end
end)

controller.bind("centralWheel_click_shift", function(value)
	-- Shift + Click Central Wheel = Go Back
	if value >= 0.5 then
		patchExplorer.back()
	end
end)

local stepShifting = 0.002
local stepNormal = 0.02

controller.bind("knob1", function(value)
	if value >= 0.5 then
		inputPlayer.updateParam("_qparam_wheellike_1",
			inputPlayer.getParam("_qparam_wheellike_1") + (shifting and stepShifting or stepNormal))
	else
		inputPlayer.updateParam("_qparam_wheellike_1",
			inputPlayer.getParam("_qparam_wheellike_1") - (shifting and stepShifting or stepNormal))
	end
end)

controller.bind("knob2", function(value)
	if value >= 0.5 then
		inputPlayer.updateParam("_qparam_wheellike_2",
			inputPlayer.getParam("_qparam_wheellike_2") + (shifting and stepShifting or stepNormal))
	else
		inputPlayer.updateParam("_qparam_wheellike_2",
			inputPlayer.getParam("_qparam_wheellike_2") - (shifting and stepShifting or stepNormal))
	end
end)

controller.bind("knob3", function(value)
	if value >= 0.5 then
		inputPlayer.updateParam("_qparam_wheellike_3",
			inputPlayer.getParam("_qparam_wheellike_3") + (shifting and stepShifting or stepNormal))
	else
		inputPlayer.updateParam("_qparam_wheellike_3",
			inputPlayer.getParam("_qparam_wheellike_3") - (shifting and stepShifting or stepNormal))
	end
end)

controller.bind("knob4", function(value)
	if value >= 0.5 then
		inputPlayer.updateParam("_qparam_wheellike_4",
			inputPlayer.getParam("_qparam_wheellike_4") + (shifting and stepShifting or stepNormal))
	else
		inputPlayer.updateParam("_qparam_wheellike_4",
			inputPlayer.getParam("_qparam_wheellike_4") - (shifting and stepShifting or stepNormal))
	end
end)

controller.bind("knob5", function(value)
	if value >= 0.5 then
		inputPlayer.updateParam("_qparam_wheellike_5",
			inputPlayer.getParam("_qparam_wheellike_5") + (shifting and stepShifting or stepNormal))
	else
		inputPlayer.updateParam("_qparam_wheellike_5",
			inputPlayer.getParam("_qparam_wheellike_5") - (shifting and stepShifting or stepNormal))
	end
end)

controller.bind("knob6", function(value)
	if value >= 0.5 then
		inputPlayer.updateParam("_qparam_wheellike_6",
			inputPlayer.getParam("_qparam_wheellike_6") + (shifting and stepShifting or stepNormal))
	else
		inputPlayer.updateParam("_qparam_wheellike_6",
			inputPlayer.getParam("_qparam_wheellike_6") - (shifting and stepShifting or stepNormal))
	end
end)

controller.bind("knob7", function(value)
	if value >= 0.5 then
		inputPlayer.updateParam("_qparam_wheellike_7",
			inputPlayer.getParam("_qparam_wheellike_7") + (shifting and stepShifting or stepNormal))
	else
		inputPlayer.updateParam("_qparam_wheellike_7",
			inputPlayer.getParam("_qparam_wheellike_7") - (shifting and stepShifting or stepNormal))
	end
end)

controller.bind("knob8", function(value)
	if value >= 0.5 then
		inputPlayer.updateParam("_qparam_wheellike_8",
			inputPlayer.getParam("_qparam_wheellike_8") + (shifting and stepShifting or stepNormal))
	else
		inputPlayer.updateParam("_qparam_wheellike_8",
			inputPlayer.getParam("_qparam_wheellike_8") - (shifting and stepShifting or stepNormal))
	end
end)
