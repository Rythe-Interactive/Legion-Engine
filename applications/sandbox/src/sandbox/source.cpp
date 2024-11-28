#define RYTHE_ENTRY
#define RYTHE_LOG_DEBUG

#if defined(NDEBUG)
	#define RYTHE_KEEP_CONSOLE
#endif

#include <core/core.hpp>
#include <rsl/type_traits>

template <typename T>
struct foo
{
};

int RYTHE_CCONV reportModules(rythe::core::Program& program)
{
	using namespace rythe;
	program.addEngineInstance();

	[[maybe_unused]] constexpr rsl::constexpr_string A = "Something";
	[[maybe_unused]] constexpr rsl::constexpr_string B = "Other";
	[[maybe_unused]] constexpr rsl::constexpr_string result = A + B;

	[[maybe_unused]] constexpr rsl::constexpr_string hello_world = "hello world";
	[[maybe_unused]] constexpr rsl::constexpr_string shorten = hello_world.filter_if([](char y) { return ' ' != y; });
	[[maybe_unused]] constexpr rsl::constexpr_string optimal = shorten.refit<shorten.size() + 1>();

	constexpr rsl::constexpr_string typeName = rsl::type_name<std::string>();
	[[maybe_unused]] constexpr auto shrunk = typeName.refit<typeName.size() + 1>();
	constexpr rsl::id_type typeHash = rsl::type_id<std::string>();

	rsl::log::debug(
		"type info: {} : {}, {}, {}, {}", rsl::string_view(typeName), typeHash, typeName.size(), typeName.capacity(),
		shrunk.capacity()
	);

	return -1;
}
