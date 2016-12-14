
# Install runner
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ubuntu-printing-app.in ${CMAKE_CURRENT_BINARY_DIR}/ubuntu-printing-app)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ubuntu-printing-app DESTINATION ${CMAKE_INSTALL_BINDIR})

# Show in QtC
add_custom_target(ubuntu_printing_app_runner_files ALL SOURCES ubuntu-printing-app.in)
