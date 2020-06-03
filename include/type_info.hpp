#ifndef TYPE_INFO_HPP
#define TYPE_INFO_HPP
#pragma once

#include <atomic>
#include <type_traits>

#include "id_type.hpp"

namespace ecs::impl {

class type_info
{
private:
    inline static std::atomic<id_type> m_current_id{ 0 };

public:
    template<typename T>
    [[nodiscard]] static auto get_id() noexcept -> id_type
    {
        static_assert(!std::is_const_v<T>, "ECS types can't be `const`");
        static_assert(!std::is_volatile_v<T>, "ECS types can't be `volatile`");
        static_assert(!std::is_reference_v<T>, "ECS types can't be references");

        static id_type const id = m_current_id++;
        return id;
    }
};

} // namespace ecs::impl

namespace ecs {

template<typename T>
[[nodiscard]] auto get_id() noexcept -> id_type
{
    return impl::type_info::template get_id<T>();
}

} // namespace ecs

#endif // !TYPE_INFO_HPP
