
# Install runner
install(FILES "ubuntu-printing-app" DESTINATION ${CMAKE_INSTALL_BINDIR})

# Show in QtC
add_custom_target(ubuntu_printing_app_runner_files ALL SOURCES ubuntu-printing-app)
