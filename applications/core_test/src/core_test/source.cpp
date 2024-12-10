#define RYTHE_ENTRY

#include <chrono>
#include <iostream>
#include <rsl/logging>
#include <rsl/math>
#include <rsl/time>

#include <core/core.hpp>

enum struct test_error : rsl::errc
{
    no_error = 0,
    hi,
    no,
    fuck,
    you
};

void assert_handler(
	std::string_view expression, std::string_view file, rsl::size_type line, std::string_view message, bool soft
)
{
	rsl::log::undecoratedln(
		soft ? rsl::log::severity::warn : rsl::log::severity::fatal,
		"Assertion failed:\t{}\nExpected:\t\t{}\nSource:\t\t\t{}, line {}", message, expression, file, line
	);

	__debugbreak();
}

rsl::result<void> RYTHE_CCONV init_program(rythe::core::Program& program)
{
	rsl::asserts::assert_handler = &assert_handler;

	rsl::result<void> result;

	rsl::log::debug("Initilizing Core-Application");
	program.addEngineInstance();

    rsl::append_error(result, test_error::no, "NOPE");
    rsl::append_warning(result, test_error::hi, "HEEEEYYYYY");

    return result;
}
