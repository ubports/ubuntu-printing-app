
# Install runner
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${RUNNER}.in ${CMAKE_CURRENT_BINARY_DIR}/${RUNNER})

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${RUNNER} DESTINATION ${CMAKE_INSTALL_BINDIR})


# Show in QtC
add_custom_target(queue_dialog_runner_files ALL SOURCES ${RUNNER}.in)
