#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "registry.hpp"

struct position
{
    double x{ 0 };
    double y{ 0 };
};

struct velocity
{
    double v{ 0 };
};

TEST_CASE("[ecs::registry] Basic - creating two entities and iterating over them")
{
    constexpr double value1 = 0.5;
    constexpr double value2 = 1.0;
    constexpr double value3 = 1.5;

    ecs::registry r{};

    [[maybe_unused]] auto e = r.create_entity();
    auto& pos = r.emplace<position>(e, value1, value2);
    auto& vel = r.emplace<velocity>(e, value2);

    REQUIRE(e.id() == 0);

    [[maybe_unused]] auto e2 = r.create_entity();
    auto& vel2 = r.emplace<velocity>(e2, value3);

    REQUIRE(e2.id() == 1);

    REQUIRE(pos.x == value1);
    REQUIRE(pos.y == value2);
    REQUIRE(vel.v == value2);
    REQUIRE(vel2.v == value3);

    r.for_each<position>([](position& p) -> void { MESSAGE("Inner position: (" << p.x << ", " << p.y << ')'); });
    r.for_each<velocity>([](velocity& v) -> void { MESSAGE("Inner velocity: " << v.v); });
}
