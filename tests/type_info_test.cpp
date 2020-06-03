#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "type_info.hpp"

TEST_CASE("[ecs::get_id] Basic requirements")
{
    constexpr ecs::id_type int_value = 0;
    constexpr ecs::id_type float_value = 1;
    constexpr ecs::id_type double_value = 2;

    REQUIRE(ecs::impl::type_info::get_id<int>() == int_value);
    REQUIRE(ecs::impl::type_info::get_id<float>() == float_value);
    REQUIRE(ecs::impl::type_info::get_id<int>() == int_value);
    REQUIRE(ecs::impl::type_info::get_id<double>() == double_value);
}
