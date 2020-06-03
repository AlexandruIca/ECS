#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <utility>

#include "entity.hpp"

TEST_CASE("[ecs::entity] Basic - constructors, operator==, operator=...")
{
    constexpr ecs::id_type default_value = 0;
    constexpr ecs::id_type test_value = 10;

    ecs::entity e{};
    ecs::entity e1{ test_value };
    ecs::entity e2{ e1 };
    ecs::entity e3{ test_value };
    ecs::entity e4{};

    e4 = e3;

    REQUIRE(e.id() == default_value);
    REQUIRE(e1.id() == test_value);
    REQUIRE(e2.id() == e1.id());
    REQUIRE(e4.id() == test_value);
    REQUIRE(e1 == e2);
    REQUIRE(e3 == e4);
}
