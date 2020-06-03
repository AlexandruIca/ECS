#include "entity.hpp"

namespace ecs {

entity::entity(id_type const id)
    : m_id{ id }
{
}

auto operator==(entity const& a, entity const& b) noexcept -> bool
{
    return a.id() == b.id();
}

} // namespace ecs
