add_subdirectory(components)
add_subdirectory(pages)

# Install Main.qml and icon
install(FILES ${MAIN_QML} DESTINATION ${UBUNTU_PRINTING_APP_DATA_DIR})
install(FILES ${ICON_FILE} DESTINATION ${UBUNTU_PRINTING_APP_DATA_DIR})

# Install content-hub json
install(FILES "ubuntu-printing-app-content-hub.json"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/content-hub/peers
    RENAME ${APP_HARDCODE})

# Install desktop file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${DESKTOP_FILE}.in.in ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}.in)
intltool_merge_translations(
  "${DESKTOP_FILE}.in"
  "${DESKTOP_FILE}"
  ALL
  UTF8
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)


# Show in QtCreator
if(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  file(GLOB MAIN_QML_JS_FILES *.apparmor *.js *.json *.qml)
  add_custom_target(ubuntu_printing_app_main_qml_js_files ALL SOURCES ${MAIN_QML_JS_FILES})
endif(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
