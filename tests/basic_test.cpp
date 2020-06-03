#include <type_traits>

#include "id_type.hpp"

auto main(int, char*[]) noexcept -> int
{
    static_assert(std::is_same_v<ecs::id_type, std::uint64_t>);
}
