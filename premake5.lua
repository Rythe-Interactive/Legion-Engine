-- root workspace, all sub-project should be included
workspace "rythe"
    location("build/" .. _ACTION)
    configurations { "Debug", "Debug-no-inline", "Development", "Release", "Debug-asan", "Release-profiling" }

os.chdir(_MAIN_SCRIPT_DIR)

filter("configurations:Debug-no-inline")
    defines { "RYTHE_DISABLE_ALWAYS_INLINE" }

local r = require("premake/rythe")

r.configure()