---@meta

---@class UI
local UI = {}

--- Create a text block.
---
---@param text string
function UI:text(text)
end

--- Create a button.
---
---@param text string
---@param callback fun()
function UI:button(text, callback)
end

--- Create a checkbox.
---
---@param currentValue boolean
---@param callback fun(value: boolean)
function UI:checkbox(currentValue, callback)
end

--- Create a numerical slider (fader).
---
---@param currentValue number
---@param minValue number
---@param maxValue number
---@param inputStep number
---@param callback fun(value: number)
function UI:slider(currentValue, minValue, maxValue, inputStep, callback)
end

--- Create a numerical knob (encoder).
---
---@param currentValue number
---@param minValue number
---@param maxValue number
---@param inputStep number
---@param callback fun(value: number)
function UI:knob(currentValue, minValue, maxValue, inputStep, callback)
end

---@type UI
ui = nil

---@class Player
local Player = {}

--- Bind to an action. Valid actions are:
--- ```lua
--- init() -- Initial loading of player
--- destroy() -- Destruction of player
--- noteOn(midiNote: number, velocity: number) -- Note pressed, velocity is from 0.0 to 1.0
--- noteOff(midiNote: number) -- Note released
--- render(sampleCount: number) -- Render some audio
--- store(storeReferences: boolean) -- Store the player data to be recalled later
--- load() -- Recall player data
--- setBPM(bpm: number) -- Update BPM
--- setSampleRate(sampleRate: number) -- Update sample rate
--- getParameters() -- Register player params
--- renderUI() -- Render player UI
--- reset() -- Reset player state
--- ```
---
---@param action string
---@param callback fun(...)
function Player:bind(action, callback)
end

--- Bind to a parameter. Value is from `0.0` to `1.0`.
--- To bind to MIDI CCs, use `_midi_cc_[n]` (e.g. `_midi_cc_1` is modwheel).
--- To bind to the pitch wheel, use `_pitch_wheel`.
---
---@param param string
---@param callback fun(value: number)
function Player:bindParam(param, callback)
end

--- Log a message.
---
---@param message string
function Player:log(message)
end

--- Log a warning message.
---
---@param message string
function Player:warn(message)
end

--- Log an error message and display an error box to the user.
---
---@param message string
function Player:error(message)
end

--- Throw an exception, display an error box to the user, and stop execution of code.
---
---@param message string
function Player:exception(message)
end

--- Write a sample pair (`-1.0` - `1.0`).
---
---@param left number Left channel sample
---@param right number Right channel sample
function Player:writeSamples(left, right)
end

--- Store a null-terminated UTF-8 string in recall data.
---
---@param value string
function Player:storeString(value)
end

--- Store a 64-bit floating-point number in recall data.
---
---@param value number
function Player:storeNumber(value)
end

--- Load a null-terminated UTF-8 string from recall data.
---
---@return string
function Player:loadString()
end

--- Load a 64-bit floating-point number from recall data.
---
---@return number
function Player:loadNumber()
end

--- Register a new parameter.
--- Parameters do not need to be registered to receive events,
--- this only needs to be used to allow the parameter to appear in the built-in UI.
---
---@param param string
function Player:registerParam(param)
end

---@type Player
player = nil