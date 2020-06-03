#ifndef REGISTRY_HPP
#define REGISTRY_HPP
#pragma once

#include <atomic>
#include <cstddef>
#include <functional>
#include <unordered_map>
#include <utility>
#include <vector>

#include "entity.hpp"
#include "id_type.hpp"
#include "type_info.hpp"

namespace ecs::impl {

auto log_type_emplaced(id_type id) -> void;
auto log_entity_iterated(entity e) -> void;

} // namespace ecs::impl

namespace ecs {

///
/// \brief Class that manages entities(the 'S' in ECS).
///
/// Basic usage:
/// ```cpp
/// ecs::registry r{};
/// ecs::entity e = r.create_entity();
/// r.emplace<your_type>(e, args, ...);
///
/// r.for_each<your_type>([](your_type& value) -> { /* ... */ });
/// ```
///
class registry
{
private:
    using deleter_t = auto (*)(std::byte*, std::size_t) -> void;
    using raw_bytes_t = std::vector<std::byte>;

    static constexpr id_type max_bytes = 65'536;

    ///
    /// \brief Number incremented for every new instance of \ref ecs::entity.
    ///
    inline static std::atomic<id_type> m_current_entity_id{ 0 };

    ///
    /// \brief Describes layout for how components will be stored.
    ///
    /// For example, after calling \ref registry::emplace for types A, B and C, \ref m_data might look like this:
    /// ```cpp
    /// ecs::registry r{};
    /// ecs::entity e = r.create_entity();
    /// ecs::entity e2 = r.create_entity();
    ///
    /// r.emplace<A>(e);
    /// r.emplace<B>(e);
    /// r.emplace<C>(e);
    /// r.emplace<B>(e2);
    /// r.emplace<C>(e2);
    ///
    /// // Let's say id for A is 0, for B is 1 and for C is 2
    /// // Then m_data looks like:
    /// // 0: [A]
    /// // 1: [B, B]
    /// // 2: [C, C]
    /// //
    /// // Where [T, ...] is an std::vector<std::byte> `reinterpret_cast`'ed to T*
    /// ```
    ///
    std::unordered_map<id_type, raw_bytes_t> m_data{};
    ///
    /// Same as \ref m_data but instead of holding bytes, it holds entities(AKA numbers).
    ///
    std::unordered_map<id_type, std::vector<entity>> m_entities{};
    ///
    /// \brief Helper for destructing memory for each type emplaced.
    ///
    /// Consider implementing the destructor. If we leave it defaulted, no constructor will be called for the types
    /// emplaced, thus leading to memory leaks. This variable stores function pointers to functions that call the
    /// correct destructors for each type emplaced. To see how it's used directly see \ref registry::emplace and \ref
    /// registry::~registry.
    ///
    std::unordered_map<id_type, deleter_t> m_deleter{};

public:
    registry() noexcept = default;
    registry(registry const&) = delete;
    registry(registry&&) noexcept = default;
    ~registry() noexcept;

    auto operator=(registry const&) -> registry& = delete;
    auto operator=(registry&&) noexcept -> registry& = default;

    ///
    /// \return A new entity holding the number of instances of \ref ecs::entity created.
    ///
    [[nodiscard]] auto create_entity() const noexcept -> entity;

    ///
    /// \brief Creates a new \p T placed inside \ref m_data.
    ///
    /// \return A reference to the newly created component.
    ///
    template<typename T, typename... Args>
    auto emplace(entity const e, Args&&... args) -> T&
    {
        id_type const id = get_id<T>();
        impl::log_type_emplaced(id);

        // Store the function that calls destructors correctly for each allocated component in \ref m_data.
        // Invoking a deleter(see \ref registry::~registry):
        // `m_deleter[type_id](m_data[type_id].data(), m_data[type_id].size());`
        m_deleter.try_emplace(id, [](std::byte* const data, std::size_t const size) -> void {
            std::size_t data_index{ 0 };
            while(data_index < size) {
                (*reinterpret_cast<T*>(&data[data_index])).~T();
                data_index += sizeof(T);
            }
        });

        auto& data_for_type = m_data[id];
        auto& entities_for_type = m_entities[id];

        if(data_for_type.empty()) {
            data_for_type.reserve(max_bytes);
            entities_for_type.reserve(max_bytes / sizeof(T));
        }

        // Allocate 0-initialized raw memory that fits a \p T
        for(std::size_t i = 0; i < sizeof(T); ++i) {
            data_for_type.emplace_back(std::byte{ 0 });
        }
        entities_for_type.push_back(e);

        std::byte* location = &data_for_type.back();
        location -= (sizeof(T) - 1);
        new(location) T{ std::forward<Args>(args)... };

        return *reinterpret_cast<T*>(location);
    }

    template<typename T, typename F>
    auto for_each(F&& f) -> void
    {
        id_type const id = get_id<T>();

        auto& data_for_type = m_data[id];
        auto& entities_for_type = m_entities[id];

        std::size_t data_index{ 0 };
        std::size_t entity_index{ 0 };

        while(data_index < data_for_type.size()) {
            impl::log_entity_iterated(entities_for_type[entity_index++]);

            auto& data = *reinterpret_cast<T*>(&data_for_type[data_index]);

            std::invoke(f, data);
            data_index += sizeof(T);
        }
    }
};

} // namespace ecs

#endif // !REGISTRY_HPP
