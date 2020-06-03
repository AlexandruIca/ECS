enable_testing()

function(build_test TEST_NAME)
  add_executable(${TEST_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME}.cpp)
  target_link_libraries(${TEST_NAME} PRIVATE project::options project::warnings ecs::ecs)
  add_test(${TEST_NAME} ${TEST_NAME})
endfunction()

build_test(basic_test)
