#ifndef ECS_ID_TYPE_HPP
#define ECS_ID_TYPE_HPP
#pragma once

#include <cstdint>

///
/// \brief All functionality is contained in this namespace.
///
namespace ecs {

///
/// \brief This is the type used to count the instances of entities.
///
using id_type = std::uint64_t;

} // namespace ecs

#endif // !ECS_ID_TYPE_HPP
