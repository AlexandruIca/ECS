set(ECS_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)

add_library(ecs STATIC ${ECS_SOURCE_FILES})
add_library(ecs_shared SHARED ${ECS_SOURCE_FILES})

add_library(ecs::ecs ALIAS ecs)
add_library(ecs::ecs_shared ALIAS ecs_shared)

target_include_directories(ecs PUBLIC ${ECS_INCLUDE_DIR})
target_include_directories(ecs_shared PUBLIC ${ECS_INCLUDE_DIR})

target_link_libraries(ecs PRIVATE project::options project::warnings)
target_link_libraries(ecs_shared PRIVATE project::options project::warnings)
