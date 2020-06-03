#include <spdlog/spdlog.h>

#include "id_type.hpp"

auto f(ecs::id_type) -> void
{
    spdlog::info("Logging is working!");
}
