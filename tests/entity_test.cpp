#include <cassert>
#include <utility>

#include "entity.hpp"

auto main(int, char*[]) noexcept -> int
{
    constexpr ecs::id_type default_value = 0;
    constexpr ecs::id_type test_value = 10;

    ecs::entity e{};
    ecs::entity e1{ test_value };
    ecs::entity e2{ e1 };
    ecs::entity e3{ test_value };
    ecs::entity e4{};

    e4 = e3;

    assert(e.id() == default_value);
    assert(e1.id() == test_value);
    assert(e2.id() == e1.id());
    assert(e4.id() == test_value);
    assert(e1 == e2);
    assert(e3 == e4);
}
