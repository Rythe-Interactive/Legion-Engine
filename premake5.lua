os.chdir(_MAIN_SCRIPT_DIR)

filter("configurations:Debug-no-inline")
    defines { "RYTHE_DISABLE_ALWAYS_INLINE" }

local r = require("premake/rythe")

local workspace = {
    name = "rythe",
    location = "build/" .. _ACTION,
    configurations = { "Debug", "Debug-no-inline", "Development", "Release", "Debug-asan", "Release-profiling" }
}

r.configure({workspace})