find_package(doctest REQUIRED)

# Each test must implement it's own main because we rely on static initialization for type ids
function(build_test TEST_NAME)
  add_executable(${TEST_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME}.cpp)
  target_link_libraries(${TEST_NAME} PRIVATE project::options project::warnings ecs::ecs_shared doctest::doctest)
  add_test(${TEST_NAME} ${TEST_NAME})
endfunction()

build_test(basic_test)
build_test(type_info_test)
build_test(entity_test)
build_test(registry_test)
