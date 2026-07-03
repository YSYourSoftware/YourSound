-- TO GET SHIFT BOOL: controller:queryCC(9, 0, 127) >= 0.5

-- "fader" types need function(value: float[0.0 -> 1.0])
-- "button" and "toggle" types need function(on: bool)

-- TODO: fader1-4, knob1-8
-- TODO: idea: have central wheel click + scroll do something different

local oldCentralWheelValue = controller:queryCC(114, 0, 127)

controller:bind("modwheel", function(value)
    -- Shift + ModWheel = Expression Wheel
    if controller:queryCC(9, 0, 127) >= 0.5 then
        inputPlayer:updateParam("_midi_cc_11", value)
    else
        inputPlayer:updateParam("_midi_cc_1", value)
    end
end)

controller:bind("centralWheel", function(value)
    -- Central Wheel = Patch / Library Cursor
    if oldCentralWheelValue > value then
        patchExplorer:moveBy(1)
    else
        patchExplorer:moveBy(-1)
    end

    oldCentralWheelValue = value
end)

controller:bind("centralWheel_shift", function(value)
    -- Shift + Central Wheel = Skip to End / Start of Folder / Library
    if oldCentralWheelValue > value then
        patchExplorer:moveTo(-1)
    else
        patchExplorer:moveTo(1)
    end

    oldCentralWheelValue = value
end)

controller:bind("centralWheel_click", function(on)
    -- Click Central Wheel = Load Patch / Open Library / Folder
    if on then
        patchExplorer:load()
    end
end)

controller:bind("centralWheel_click_shift", function(on)
    -- Shift + Click Central Wheel = Go Back
    if on then
        patchExplorer:back()
    end
end)

controller:bind("pad1", function(on)
    if on then
        inputPlayer:updateParam("_button_hold_1", 1.0)
    else
        inputPlayer:updateParam("_button_hold_1", 0.0)
    end
end)

controller:bind("pad2", function(on)
    if on then
        inputPlayer:updateParam("_button_hold_2", 1.0)
    else
        inputPlayer:updateParam("_button_hold_2", 0.0)
    end
end)

controller:bind("pad3", function(on)
    if on then
        inputPlayer:updateParam("_button_hold_3", 1.0)
    else
        inputPlayer:updateParam("_button_hold_3", 0.0)
    end
end)

controller:bind("pad4", function(on)
    if on then
        inputPlayer:updateParam("_button_hold_4", 1.0)
    else
        inputPlayer:updateParam("_button_hold_4", 0.0)
    end
end)

controller:bind("pad5", function(on)
    if on then
        inputPlayer:updateParam("_button_hold_5", 1.0)
    else
        inputPlayer:updateParam("_button_hold_5", 0.0)
    end
end)

controller:bind("pad6", function(on)
    if on then
        inputPlayer:updateParam("_button_hold_6", 1.0)
    else
        inputPlayer:updateParam("_button_hold_6", 0.0)
    end
end)

controller:bind("pad7", function(on)
    if on then
        inputPlayer:updateParam("_button_hold_7", 1.0)
    else
        inputPlayer:updateParam("_button_hold_7", 0.0)
    end
end)

controller:bind("pad8", function(on)
    if on then
        inputPlayer:updateParam("_button_hold_8", 1.0)
    else
        inputPlayer:updateParam("_button_hold_8", 0.0)
    end
end)

controller:bind("pad1_toggle", function(on)
    if on then
        inputPlayer:updateParam("_button_toggle_1", 1.0)
    else
        inputPlayer:updateParam("_button_toggle_1", 0.0)
    end
end)

controller:bind("pad2_toggle", function(on)
    if on then
        inputPlayer:updateParam("_button_toggle_2", 1.0)
    else
        inputPlayer:updateParam("_button_toggle_2", 0.0)
    end
end)

controller:bind("pad3_toggle", function(on)
    if on then
        inputPlayer:updateParam("_button_toggle_3", 1.0)
    else
        inputPlayer:updateParam("_button_toggle_3", 0.0)
    end
end)

controller:bind("pad4_toggle", function(on)
    if on then
        inputPlayer:updateParam("_button_toggle_4", 1.0)
    else
        inputPlayer:updateParam("_button_toggle_4", 0.0)
    end
end)

controller:bind("pad5_toggle", function(on)
    if on then
        inputPlayer:updateParam("_button_toggle_5", 1.0)
    else
        inputPlayer:updateParam("_button_toggle_5", 0.0)
    end
end)

controller:bind("pad6_toggle", function(on)
    if on then
        inputPlayer:updateParam("_button_toggle_6", 1.0)
    else
        inputPlayer:updateParam("_button_toggle_6", 0.0)
    end
end)

controller:bind("pad7_toggle", function(on)
    if on then
        inputPlayer:updateParam("_button_toggle_7", 1.0)
    else
        inputPlayer:updateParam("_button_toggle_7", 0.0)
    end
end)

controller:bind("pad8_toggle", function(on)
    if on then
        inputPlayer:updateParam("_button_toggle_8", 1.0)
    else
        inputPlayer:updateParam("_button_toggle_8", 0.0)
    end
end)