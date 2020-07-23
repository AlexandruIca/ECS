#ifndef REGISTRY_HPP
#define REGISTRY_HPP
#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <functional>
#include <iterator>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ecs/entity.hpp"
#include "ecs/id_type.hpp"
#include "ecs/type_info.hpp"

namespace ecs::impl {

auto log_type_emplaced(id_type id) -> void;
auto log_entity_iterated(entity e) -> void;
auto log_entity_union(std::vector<id_type> const& v) -> void;

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
    std::atomic<id_type> m_current_entity_id{ 0 };

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
    registry(registry&&) noexcept = delete;
    ~registry() noexcept;

    auto operator=(registry const&) -> registry& = delete;
    auto operator=(registry&&) noexcept -> registry& = delete;

    ///
    /// \return A new entity holding the number of instances of \ref ecs::entity created.
    ///
    [[nodiscard]] auto create_entity() noexcept -> entity;

    ///
    /// \brief Creates a new \p T placed inside \ref m_data.
    ///
    /// \return A reference to the newly created component.
    ///
    template<typename T, typename... Args>
    auto emplace(entity const e, Args&&... args) -> T&
    {
        static_assert(std::is_nothrow_move_assignable_v<T>, "Registry types must be nothrow move assignable");

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

    ///
    /// \brief Iterate over entities with only _one_ component of type \p T.
    ///
    /// This is the fastest iteration. If you don't need to iterate through entities with multiple components don't do
    /// it, this is very efficient since it iterates through contiguous memory.
    ///
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

    ///
    /// \brief Iterate over entities with multiple components.
    ///
    /// When calling `registry::for_each<T1, T2, ..., Tn>([]{...})` the callback will receive n references to types
    /// `T1, ..., Tn` of the entities that have at least `T1, ..., Tn` components.
    ///
    template<typename T1, typename T2, typename... Ts, typename F>
    auto for_each(F&& f) -> void
    {
        std::array<id_type, sizeof...(Ts) + 2> const ids = { get_id<T1>(), get_id<T2>(), get_id<Ts>()... };
        std::vector<id_type> intersection{};

        intersection.reserve(m_entities[ids[0]].size());

        for(auto const e : m_entities[ids[0]]) {
            intersection.push_back(e.id());
        }

        for(id_type entity_index = 1; entity_index < ids.size(); ++entity_index) {
            std::vector<id_type> entities_for_type{};

            entities_for_type.reserve(m_entities[ids[entity_index]].size());

            for(auto const e : m_entities[ids[entity_index]]) {
                entities_for_type.push_back(e.id());
            }

            std::vector<id_type> tmp_intersection{};
            tmp_intersection.reserve(intersection.size());

            std::set_intersection(intersection.begin(),
                                  intersection.end(),
                                  entities_for_type.begin(),
                                  entities_for_type.end(),
                                  std::back_inserter(tmp_intersection));

            intersection = std::move(tmp_intersection);
        }

        impl::log_entity_union(intersection);

        static_cast<void>(f);
    }
};

} // namespace ecs

#endif // !REGISTRY_HPP
