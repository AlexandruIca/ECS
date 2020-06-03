#include <cassert>

#include "type_info.hpp"

auto main(int, char*[]) noexcept -> int
{
    constexpr ecs::id_type int_value = 0;
    constexpr ecs::id_type float_value = 1;
    constexpr ecs::id_type double_value = 2;

    assert(ecs::get_id<int>() == int_value);
    assert(ecs::get_id<float>() == float_value);
    assert(ecs::get_id<int>() == int_value);
    assert(ecs::get_id<double>() == double_value);
}
