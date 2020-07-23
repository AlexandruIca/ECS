#ifndef TYPE_INFO_HPP
#define TYPE_INFO_HPP
#pragma once

#include <atomic>
#include <type_traits>

#include "id_type.hpp"

namespace ecs::impl {

///
/// \brief Helper class to generate a unique id for a given type.
///
class type_info
{
private:
    inline static std::atomic<id_type> m_current_id{ 0 };

public:
    ///
    /// This is the function that actually gets the id.
    ///
    /// \tparam T Type for which to generate the unique id.
    ///
    /// \return A unique id for \p T in the sense that if you call this multiple times for the same type the id will be
    ///         the same.
    ///
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

///
/// \brief Simple wrapper around \ref type_info.
///
/// This exists so that you don't have to write `ecs::impl::type_info::get_id<T>();`, you can use it like this:
/// ```cpp
/// ecs::get_id<int>();
/// ```
///
template<typename T>
[[nodiscard]] inline auto get_id() noexcept -> id_type
{
    return impl::type_info::template get_id<T>();
}

} // namespace ecs

#endif // !TYPE_INFO_HPP
