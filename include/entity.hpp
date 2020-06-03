#ifndef ECS_ENTITY_HPP
#define ECS_ENTITY_HPP
#pragma once

#include "id_type.hpp"

namespace ecs {

///
/// \brief Basic wrapper around \ref id_type.
///
class entity
{
private:
    id_type m_id{ 0 };

public:
    constexpr entity() noexcept = default;
    entity(entity const&) noexcept = default;
    entity(entity&&) noexcept = default;
    ~entity() noexcept = default;

    explicit entity(id_type id);

    auto operator=(entity const&) noexcept -> entity& = default;
    auto operator=(entity&&) noexcept -> entity& = default;

    [[nodiscard]] constexpr auto id() const noexcept -> id_type
    {
        return m_id;
    }
};

[[nodiscard]] auto operator==(entity const& a, entity const& b) noexcept -> bool;

[[nodiscard]] inline auto operator!=(entity const& a, entity const& b) noexcept -> bool
{
    return !(a == b);
}

} // namespace ecs

#endif // !ECS_ENTITY_HPP
