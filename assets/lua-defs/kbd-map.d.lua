---@meta

controller = {}

--- Get the value of a MIDI CC.
---
---@param cc number MIDI CC number (`0` - `127`)
---@return number `0.0` - `1.0`
function controller.queryCC(cc) end

--- Bind to an action update defined in the JSON configuration.
---
---@param action string Key of an action with `onInput == "scriptEvent"`
---@param callback fun(value: number)
function controller.bind(action, callback) end

inputPlayer = nil

--- Send a parameter update to the target player.
---
---@param param string
---@param value number `0.0` - `1.0`
function inputPlayer.updateParam(param, value) end

--- Get the value of a parameter of the target player.
---
---@param param string
---@return number `0.0` - `1.0`
function inputPlayer.getParam(param) end

patchExplorer = nil

--- Move the patch selector by a certain amount.
---
---@param value number Positive moves down, negative moves up.
function patchExplorer.moveBy(value) end

--- Move the patch selector to a certain patch.
---
---@param value number Positive moves from top, negative moves from bottom.
function patchExplorer.moveTo(value) end

--- Load the selected patch, folder or library.
function patchExplorer.load() end

--- Go back to folder or library selection.
function patchExplorer.back() end