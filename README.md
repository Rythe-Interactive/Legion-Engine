[![rythe logo banner](https://assets.zyrosite.com/dWxb3NO0jWugObXN/logo_for_dark_bg-A3QwL7kkxvfw1ywO.png)](http://rythe-interactive.com)
[![build](https://github.com/Legion-Engine/Legion-Engine/workflows/build-action/badge.svg)](https://github.com/Legion-Engine/Legion-Engine/actions?query=workflow%3Abuild-action)
[![analyze](https://github.com/Legion-Engine/Legion-Engine/workflows/analyze-action/badge.svg)](https://github.com/Legion-Engine/Legion-Engine/actions?query=workflow%3Aanalyze-action)
[![docs](https://github.com/Legion-Engine/Legion-Engine/workflows/docs-action/badge.svg)](https://docs.legion-engine.com)
[![License-MIT](https://img.shields.io/github/license/Legion-Engine/Legion-Engine)](https://github.com/Legion-Engine/Legion-Engine/blob/main/LICENSE)
[![Discord](https://img.shields.io/discord/682321168610623707.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/unVNRbd)
# Rythe-Engine
Rythe-Engine is a data oriented C++17 OpenGL game engine built to make optimal use of modern hardware.<br><br>
The Rythe-Core is built on an async compute minded design to take care of the logic and an ECS to take care of the data. This allows the engine and editor to utilize all the power they can find and to be extremely modular.

## Features
### Rendering
- Post-processing stack
- Particle system
- PBR
- Imgui
- Automatic exposure
- Modular rendering pipeline
- Custom shader support & shader standard library
- shader precompiler [lgnspre](https://github.com/Rythe-Interactive/LegionShaderPreprocess)
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

## Getting Started
### Prerequisites
The engine is by default build using Visual Studio 19 using the Clang++ compiler and C++17.
For linux we don't provide any default IDE support. However, you can still compile the engine using Clang++.
### Install
You can either build the engine yourself using Premake5 or the already provided Visual Studio 19 solution. As of now Rythe does not support compilation to DLL.
Copy the include folder to your project and link the libraries you compiled.
### Setup
Rythe already defines the C++ entry point in it's own source code. So in order to start making a program define ``LEGION_ENTRY`` and include any of modules main include files.
eg:
```cpp
#define LEGION_ENTRY
#include <core/core.hpp>
```
Since the entry point is already defined you need to define a different function to start working with Rythe. Rythe will already start itself, but it won't have any modules attached. In order to attach modules you need to define the ``reportModules`` function like so:
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
Rythe engine uses an ECS for all of it's core functionality. So for your own project you need to define your own systems and components:
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
(All libraries can already be found in the [deps](https://github.com/Rythe-Interactive/Rythe-Engine/tree/main/deps) folder)
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
* [Legion shader preprocessor (lgnspre)](https://github.com/Rythe-Interactive/LegionShaderPreprocess)

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Authors

* **Glyn Leine** - *Core architecture, ECS, scheduling, import pipeline, and renderer* - [[Website](https://glynleine.com)] [[Github](https://github.com/GlynLeine)] [[LinkedIn](https://www.linkedin.com/in/glyn-leine-7140a8167/)]
* **Raphael Baier** - *Filesystem, build pipeline, GPGPU compute, input system, meta nonsense* - [[Website](https://rbaier.me)] [[Github](https://github.com/Algo-ryth-mix)] [[LinkedIn](https://www.linkedin.com/in/raphael-baier-26800a188/)]
* **Raphael Priatama** - *Physics* - [[Website](https://developer-the-great.github.io)] [[Github](https://github.com/Developer-The-Great)] [[LinkedIn](https://www.linkedin.com/in/raphael-priatama-78a0a7189/?originalSubdomain=nl)]
* **Jelle Vrieze** - *Audio/3D audio* - [[Website](http://jellevrieze.nl)] [[Github](https://github.com/Jelled1st)] [[LinkedIn](https://www.linkedin.com/in/jelle-vrieze-2467661a7/)]
* **Rowan Ramsey** - *Serialization* - [[Website](https://blazinram.wixsite.com/rowanramsey)] [[Github](https://github.com/Ragingram2)] [[LinkedIn](https://www.linkedin.com/in/rowan-r-42a760125/)]

See also the list of [contributors](AUTHORS.md) who participated in this project.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details



