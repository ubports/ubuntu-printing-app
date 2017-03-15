set(APP_NAME ubuntu-printing-app)
set(APP_HARDCODE ubuntu-printing-app)
set(DESKTOP_FILE "${APP_NAME}.desktop")
set(ICON_FILE ubuntu-printing-app.svg)
set(MAIN_QML Main.qml)
set(RUNNER ${APP_HARDCODE})
set(SNAP_DESKTOP_FILE snap/gui/${DESKTOP_FILE})

# Set the path for the QML files and the backend
set(UBUNTU_PRINTING_APP_DATA_DIR "${CMAKE_INSTALL_DATADIR}/${APP_HARDCODE}/ubuntu-printing-app")
set(MODULE_PATH ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_LIBRARY_ARCHITECTURE}/${APP_HARDCODE})

# Set the exec path
if(SNAP_MODE)
  set(EXEC "APP_ID=${APP_HARDCODE} qmlscene $@ -I $SNAP/${MODULE_PATH} $SNAP/${CMAKE_INSTALL_PREFIX}/${UBUNTU_PRINTING_APP_DATA_DIR}/${MAIN_QML}")
  set(ICON "$SNAP/${CMAKE_INSTALL_PREFIX}/${UBUNTU_PRINTING_APP_DATA_DIR}/${ICON_FILE}")
else(SNAP_MODE)
  set(EXEC "APP_ID=${APP_HARDCODE} qmlscene $@ -I ${MODULE_PATH} ${CMAKE_INSTALL_PREFIX}/${UBUNTU_PRINTING_APP_DATA_DIR}/${MAIN_QML}")
  set(ICON ${CMAKE_INSTALL_PREFIX}/${UBUNTU_PRINTING_APP_DATA_DIR}/${ICON_FILE})
endif(SNAP_MODE)

add_subdirectory(backend)
add_subdirectory(components)
add_subdirectory(runner)
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

add_custom_target(${DESKTOP_FILE} ALL
  COMMENT "Merging translations into ${DESKTOP_FILE}"
  COMMAND LC_ALL=C ${INTLTOOL_MERGE} -d -u ${CMAKE_SOURCE_DIR}/po ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}.in ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)


# Show in QtCreator
if(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  file(GLOB MAIN_QML_JS_FILES *.apparmor *.js *.json *.qml)
  add_custom_target(ubuntu_printing_app_main_qml_js_files ALL SOURCES ${MAIN_QML_JS_FILES})
endif(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")

# Set variables for use in parent scope
set(UBUNTU_PRINTING_APP_DESKTOP_FILE "${DESKTOP_FILE}" PARENT_SCOPE)
