#include <spdlog/spdlog.h>

#include "registry.hpp"

namespace ecs::impl {

auto log_type_emplaced(id_type const id) -> void
{
    spdlog::info("Emplaced for type #{}", id);
}

auto log_entity_iterated(entity const e) -> void
{
    spdlog::info("Iterated over entity #{}", e.id());
}

} // namespace ecs::impl

namespace ecs {

auto registry::create_entity() const noexcept -> entity
{
    spdlog::info("Created entity #{}", m_current_entity_id.load());
    return entity{ m_current_entity_id++ };
}

registry::~registry() noexcept
{
    for(auto& [key, fn] : m_deleter) {
        auto& data_for_type = m_data[key];
        fn(data_for_type.data(), data_for_type.size());
    }
}

} // namespace ecs
