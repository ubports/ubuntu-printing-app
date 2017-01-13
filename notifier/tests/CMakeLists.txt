find_package(GMock REQUIRED)

include_directories(
  ${CMAKE_SOURCE_DIR}/notifier/src
  ${SERVICE_DEPS_INCLUDE_DIRS}
)
link_directories(
  ${SERVICE_DEPS_LIBRARY_DIRS}
)

add_executable(test-${SERVICE_LIB_NAME}
  test_utils.cpp
)
target_link_libraries(test-${SERVICE_LIB_NAME}
  ${SERVICE_LIB_NAME}
  ${SERVICE_DEPS_LIBRARIES}
  ${CUPS_LIBS}

  ${GTEST_LIBRARIES}
  ${GMOCK_LIBRARIES}

  ${CMAKE_THREAD_LIBS_INIT}
)
add_test(test-${SERVICE_LIB_NAME} test-${SERVICE_LIB_NAME})
