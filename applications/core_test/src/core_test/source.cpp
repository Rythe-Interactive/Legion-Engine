#define RYTHE_ENTRY

#include <core/core.hpp>

using namespace rythe::core;

struct test_struct
{
	int i = 14;
};

rsl::result<void> RYTHE_CCONV init_program(program& program)
{
	auto& engine = program.add_engine_instance();
	engine.get_context().emplace<test_struct>().i = 15;

	rsl::log::debug("{}", engine.get_context().get<test_struct>().i);

	return rsl::error; // Temporarily to stop the engine from running.
}
