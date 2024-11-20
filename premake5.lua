-- root workspace, all sub-project should be included
workspace "rythe"
    location("build/" .. _ACTION)
    configurations { "Debug", "Development", "Release", "Debug-asan", "Release-profiling" }

os.chdir(_MAIN_SCRIPT_DIR)

local r = require("premake/rythe")

r.configure()