---@meta

local ui = {}

--- Create a text block.
---
---@param text string
function ui.text(text)
end

--- Create a button.
---
---@param text string
---@param callback fun()
function ui.button(text, callback)
end

--- Create a checkbox.
---
---@param currentValue boolean
---@param callback fun(value: boolean)
function ui.checkbox(currentValue, callback)
end

--- Create a numerical slider (fader).
---
---@param currentValue number
---@param minValue number
---@param maxValue number
---@param inputStep number
---@param callback fun(value: number)
function ui.slider(currentValue, minValue, maxValue, inputStep, callback)
end

--- Create a numerical knob (encoder).
---
---@param currentValue number
---@param minValue number
---@param maxValue number
---@param inputStep number
---@param callback fun(value: number)
function ui.knob(currentValue, minValue, maxValue, inputStep, callback)
end

local player = {}

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
function player.bind(action, callback)
end

--- Bind to a parameter. Value is from `0.0` to `1.0`.
--- To bind to MIDI CCs, use `_midi_cc_[n]` (e.g. `_midi_cc_1` is modwheel).
--- To bind to the pitch wheel, use `_pitch_wheel`.
---
---@param param string
---@param callback fun(value: number)
function player.bindParam(param, callback)
end

--- Log a message.
---
---@param message string
function player.log(message)
end

--- Log a warning message.
---
---@param message string
function player.warn(message)
end

--- Log an error message and display an error box to the user.
---
---@param message string
function player.error(message)
end

--- Throw an exception, display an error box to the user, and stop execution of code.
---
---@param message string
function player.exception(message)
end

--- Write a sample pair (`-1.0` - `1.0`).
---
---@param left number Left channel sample
---@param right number Right channel sample
function player.writeSamples(left, right)
end

--- Store a null-terminated UTF-8 string in recall data.
---
---@param value string
function player.storeString(value)
end

--- Store a 64-bit floating-point number in recall data.
---
---@param value number
function player.storeNumber(value)
end

--- Load a null-terminated UTF-8 string from recall data.
---
---@return string
function player.loadString()
end

--- Load a 64-bit floating-point number from recall data.
---
---@return number
function player.loadNumber()
end

--- Register a new parameter.
--- Parameters do not need to be registered to receive events,
--- this only needs to be used to allow the parameter to appear in the built-in UI.
---
---@param param string
function player.registerParam(param)
end

---@class Vec2
local Vec = {}