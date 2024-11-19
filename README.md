[![rythe logo banner](https://assets.zyrosite.com/dWxb3NO0jWugObXN/logo_for_dark_bg-A3QwL7kkxvfw1ywO.png)](http://rythe-interactive.com)
[![build](https://github.com/Rythe-Interactive/Rythe-Engine/workflows/build/badge.svg)](https://github.com/Rythe-Interactive/Rythe-Engine/actions?query=workflow%3Abuild)
[![analyze](https://github.com/Rythe-Interactive/Rythe-Engine/workflows/analyze/badge.svg)](https://github.com/Rythe-Interactive/Rythe-Engine/actions?query=workflow%3Aanalyze)
[![License-MIT](https://img.shields.io/github/license/Rythe-Interactive/Rythe-Engine)](https://github.com/Legion-Engine/Legion-Engine/blob/main/LICENSE)
[![Discord](https://img.shields.io/discord/682321168610623707.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/unVNRbd)
# Rythe-Engine
Rythe-Engine is a data oriented C++17 game engine built to make optimal use of modern hardware.<br><br>

Rythe's core is built on an async compute minded design to take care of the logic and an ECS to take care of the data. This allows the engine, its other modules, and the editor to utilize all the power they can find and to be extremely modular.

The engine's modules are separated into optional git submodules; links to them can be found in their respective folders in rythe/engine/.

## Features
### Rendering
- Post-processing stack
- Particle system
- PBR
- Imgui
- Automatic exposure
- Modular rendering pipeline
- Custom shader support & shader standard library
- shader precompiler [lgnspre](https://github.com/Legion-Engine/LegionShaderPreprocess)
- GLTF & OBJ support

### Physics
- Convex quick hull generation
- Diviner physics engine
#### Preview Feature
- Dynamic Destruction
![Demo](https://cdn.discordapp.com/attachments/682321169541890070/802090059788582912/fracturebasics.gif)

### ECS
- Data oriented
- Thread-safe
- "Archetype" support

### Eventsystem
- Thread-safe eventbus
- Unique & Persistent events
- easy extensebility

### Audio
- Spatial audio
- Non-spatial audio
- Dopplereffect
- MP3 & WAV support
- Stereo->Mono conversion

### Compute 
- OpenCL frontend with support for buffers & textures
- High level abstractions

### Misc
- Virtual filesystem
- Serialization & Scenes(Alpha)
- Job scheduling
- Pipeline scheduling for multiple main threads
- Modular Processchains
- Custom logging support
- Custom input system
- Extended standard library
- Modular Architecture
- Math extensions to GLM

## CMake
Rythe uses CMake to generate its project files. The CMake script recognizes git submodules and adds configurable options to enable/disable them in the cache.

Using this system, you can easily generate a project with the modules that you need. Adding new modules is also simple and requires no CMake modification, see the Adding new modules section for more.

### Supported configurations
| Platform             | Compiler   |
|----------------------|------------|
| Windows 10+          | LLVM-Clang |
| Ubuntu 20.04         | Clang++    |

All configurations use C++17 on the x64 architecture.

### Building
_Rythe-Engine uses CMake 3.16, make sure to install a CMake version that is the same or higher (https://cmake.org/install/)._

If you haven't yet cloned the repository, start with that:
```
cd repositories/
git clone https://github.com/Rythe-Interactive/Rythe-Engine.git
```

CMake projects are built using a command-line interface, or through the GUI. We'll describe the command-line approach here. Note particularly the `-T ClangCL` parameter; this is to select the LLVM Clang toolchain in Visual Studio.

```
cd repositories
cmake -E make_directory Rythe-Engine-Build
cmake . 
    -G "Visual Studio 16 2019" 
    -S Rythe-Engine/ 
    -B Rythe-Engine-Build/
    -T ClangCL
```
Enable/disable optional parameters by adding them to the last command using `-D<PARAMETER_NAME>=ON` Optional parameters to add to the last command are:
| Parameter                     | Description   |
|--------------------------------|------------|
| RYTHE_BUILD_APPLICATIONS         | Add applications to the project files. The engine provides a sandbox application in the root repository, but modules may also provide their own (sandbox, samples, etc.). |
| RYTHE_BUILD_OPTION_ASAN       | Enable the address sanitizer. Can be useful/important for debugging memory violations.  |
| RYTHE_FORCE_ENABLE_ALL_MODULES       | Forcefully enable every available module. This is mostly used for CI reasons but you may use this for convenience as well. |
| RYTHE_MODULE_\<NAME\>      | If enabled, the module with the given name (the parameter is uppercase, the module lowercase), will be added to the build. |

You may now either open the project, or build the engine using the CLI:
```
cmake --build Rythe-Engine-Build/ --config Debug
```

### Adding new modules
_We recommend using the module template to create new modules, but it is also possible to set up the cmake scripts manually - if you wish to do so, refer to the build system API documentation for expected cmake scripts, and the usage patterns of helper functions._

To add a new module: Create a new repository using the instructions on the repository template at https://github.com/Rythe-Interactive/Rythe-Module-Template.

Assuming you have previously cloned Rythe-Engine, go to its root folder and add the git submodule the following commmand:

`git submodule add <link> rythe/engine/<name> `

This modifies two things; the gitmodules file and a separate commit hash file. Make sure to commit/push these changes to the branch of your choice.

After having added the git submodule, simply configure CMake with `RYTHE_MODULE_<NAME>=ON`, or with the checkbox checked in the CMake GUI as discussed in the building section.

## Setup
Legion already defines the C++ entry point in it's own source code. So in order to start making a program define ``LEGION_ENTRY`` and include any of modules main include files.
eg:
```cpp
#define LEGION_ENTRY
#include <core/core.hpp>
```
Since the entry point is already defined you need to define a different function to start working with Legion. Legion will already start itself, but it won't have any modules attached. In order to attach modules you need to define the ``reportModules`` function like so:
```cpp
#include "mymodule.hpp"
using namespace legion;

void LEGION_CCONV reportModules(Engine* engine)
{
    engine->reportModule<MyModule>();
    engine->reportModule<app::ApplicationModule>();
}
```
Of course in order to link your own modules you need to make one:
```cpp
#include <core/core.hpp>
#include "mysystem.hpp"

class TestModule : public legion::Module
{
public:
    virtual void setup() override
    {
        reportComponentType<my_component>(); // Report a component type
        reportSystem<MySystem>(); // Report a system
    }
};
```
Legion engine uses an ECS for all of it's core functionality. So for your own project you need to define your own systems and components:
```cpp
#include <core/core.hpp>

struct my_component { int myVal; };

class MySystem final : public legion::System<MySystem>
{
    virtual void setup()
    {
        createProcess<&MySystem::update>("Update");
    }
    
    void update(legion::time::span deltaTime)
    {
        // Do stuff every frame on the update thread
        static auto myQuery = createQuery<my_component, position>();
        mQuery.queryEntities();
        for(auto entity : myQuery)
        {
            // Runs for every entity that has both my_component and a position component.
        }
    }
};
```
For more information about the engine usage see the [docs](https://docs.legion-engine.com).
## Dependencies
(All libraries can already be found in the [deps](https://github.com/Legion-Engine/Legion-Engine/tree/main/deps) folder)
* [OpenAL Soft](https://github.com/kcat/openal-soft)
* [GLM](https://glm.g-truc.net/)
* [OpenGL](https://www.khronos.org/opengl/)
* [GLFW](https://www.glfw.org)
* [OpenCL](https://www.khronos.org/opencl/)
* [TinyOBJ](https://github.com/tinyobjloader/tinyobjloader)
* [STB image](https://github.com/nothings/stb)
* [Cereal](http://uscilab.github.io/cereal/)
* [Spdlog](https://github.com/gabime/spdlog)
* [Minimp3](https://github.com/lieff/minimp3)
* [Legion shader preprocessor (lgnspre)](https://github.com/Legion-Engine/LegionShaderPreprocess)

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Authors

* **Glyn Leine** - *Technical director, core architecture, ECS, scheduling, import pipeline, and renderer* - [[Website](https://glynleine.com)] [[Github](https://github.com/GlynLeine)] [[LinkedIn](https://www.linkedin.com/in/glyn-leine-7140a8167/)]
* **Raphael Baier** - *Filesystem, build pipeline, GPGPU compute, input system, meta nonsense* - [[Website](https://rbaier.me)] [[Github](https://github.com/Algo-ryth-mix)] [[LinkedIn](https://www.linkedin.com/in/raphael-baier-26800a188/)]
* **Raphael Priatama** - *Physics* - [[Website](https://developer-the-great.github.io)] [[Github](https://github.com/Developer-The-Great)] [[LinkedIn](https://www.linkedin.com/in/raphael-priatama-78a0a7189/?originalSubdomain=nl)]
* **Jelle Vrieze** - *Audio/3D audio* - [[Website](http://jellevrieze.nl)] [[Github](https://github.com/Jelled1st)] [[LinkedIn](https://www.linkedin.com/in/jelle-vrieze-2467661a7/)]
* **Rowan Ramsey** - *Serialization* - [[Website](https://blazinram.wixsite.com/rowanramsey)] [[Github](https://github.com/Ragingram2)] [[LinkedIn](https://www.linkedin.com/in/rowan-r-42a760125/)]

See also the list of [contributors](AUTHORS.md) who participated in this project.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details



