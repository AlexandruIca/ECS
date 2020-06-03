#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <type_traits>

#include "id_type.hpp"

TEST_CASE("Verifying doctest works correctly")
{
    static_assert(std::is_same_v<ecs::id_type, std::uint64_t>);
}
