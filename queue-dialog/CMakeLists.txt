set(APP_HARDCODE queue-dialog)
set(APP_NAME ubuntu-queue-dialog)
set(DESKTOP_FILE "${APP_NAME}.desktop")
set(ICON_FILE ${APP_NAME}.svg)
set(MAIN_QML Main.qml)
set(RUNNER ${APP_NAME})
set(SNAP_DESKTOP_FILE snap/gui/${DESKTOP_FILE})

# Set the path for the QML files and the backend
set(QUEUE_DIALOG_DATA_DIR "${CMAKE_INSTALL_DATADIR}/ubuntu-printing-app/queue-dialog")

# Set the exec path
if(SNAP_MODE)
  set(EXEC "APP_ID=${APP_NAME} qmlscene $@ $SNAP/${CMAKE_INSTALL_PREFIX}/${QUEUE_DIALOG_DATA_DIR}/${MAIN_QML}")
  set(ICON "$SNAP/${CMAKE_INSTALL_PREFIX}/${QUEUE_DIALOG_DATA_DIR}/${ICON_FILE}")
else(SNAP_MODE)
  set(EXEC "APP_ID=${APP_NAME} qmlscene $@ ${CMAKE_INSTALL_PREFIX}/${QUEUE_DIALOG_DATA_DIR}/${MAIN_QML}")
  set(ICON ${CMAKE_INSTALL_PREFIX}/${QUEUE_DIALOG_DATA_DIR}/${ICON_FILE})
endif(SNAP_MODE)

add_subdirectory(components)
add_subdirectory(runner)
add_subdirectory(pages)

# Add tests
find_package(Qt5Test REQUIRED)
#enable_testing()
#add_subdirectory(tests)

# Install Main.qml and icon
install(FILES ${MAIN_QML} DESTINATION ${QUEUE_DIALOG_DATA_DIR})
install(FILES ${ICON_FILE} DESTINATION ${QUEUE_DIALOG_DATA_DIR})

# Install desktop file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${DESKTOP_FILE}.in.in ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}.in)
intltool_merge_translations(
  "${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}"
  ALL
  UTF8
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

# Install the url dispatcher
# For now any printing:/// uri will open the queue dialog
install(FILES ${APP_NAME}.url-dispatcher DESTINATION ${CMAKE_INSTALL_DATADIR}/url-dispatcher/urls)

# Show in QtCreator
if(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  file(GLOB QUEUE_MAIN_QML_JS_FILES *.apparmor *.js *.json *.qml)
  add_custom_target(ubuntu_queue_dialog_main_qml_js_files ALL SOURCES ${QUEUE_MAIN_QML_JS_FILES})
endif(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")

# Set variables for use in parent scope
set(UBUNTU_QUEUE_DIALOG_DESKTOP_FILE "${DESKTOP_FILE}" PARENT_SCOPE)
