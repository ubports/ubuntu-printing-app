
# Install runner
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ubuntu-printing-app.in ${CMAKE_CURRENT_BINARY_DIR}/${RUNNER})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${RUNNER} DESTINATION ${CMAKE_INSTALL_BINDIR})

# Show in QtC
add_custom_target(ubuntu_printing_app_runner_files ALL SOURCES ubuntu-printing-app.in)
