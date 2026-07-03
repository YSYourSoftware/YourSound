---@meta

---@class Controller
local Controller = {}

--- Get the value of a MIDI CC.
---
---@param cc number MIDI CC number (`0` - `127`)
---@param minValue number Minimum value this controller can represent
---@param maxValue number Maximum value this controller can represent
---@return number `0.0` - `1.0`
function Controller:queryCC(cc, minValue, maxValue) end

--- Bind to an action update defined in the JSON configuration.
---
---@param action string Key of an action with `onInput == "scriptEvent"`
---@param callback fun(value: number) | fun(on: boolean)
function Controller:bind(action, callback) end

---@type Controller
controller = nil

---@class InputPlayer
local InputPlayer = {}

--- Send a parameter update to the target player.
---
---@param action string
---@param value number `0.0` - `1.0`
function InputPlayer:updateParam(action, value) end

---@type InputPlayer
inputPlayer = nil

---@class PatchExplorer
local PatchExplorer = {}

--- Move the patch selector by a certain amount.
---
---@param value number Positive moves down, negative moves up.
function PatchExplorer:moveBy(value) end

--- Move the patch selector to a certain patch.
---
---@param value number Positive moves from top, negative moves from bottom.
function PatchExplorer:moveTo(value) end

--- Load the selected patch, folder or library.
function PatchExplorer:load() end

--- Go back to folder or library selection.
function PatchExplorer:back() end

---@type PatchExplorer
patchExplorer = nil
