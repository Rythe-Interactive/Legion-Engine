#define RYTHE_ENTRY

#include <chrono>
#include <iostream>
#include <rsl/logging>
#include <rsl/math>
#include <rsl/time>

#include <core/core.hpp>

void RYTHE_CCONV reportModules(rythe::core::Program* program)
{
	rsl::log::debug("Initilizing Core-Application");
	program->addEngineInstance();
}
