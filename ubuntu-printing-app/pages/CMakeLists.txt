set(PAGES_QML_JS_FILES
    ContentPeerPickerPage.qml
    EmptyPage.qml
    PrintPage.qml
)

install(FILES ${PAGES_QML_JS_FILES} DESTINATION ${UBUNTU_PRINTING_APP_DATA_DIR}/pages)

# Show in QtCreator
add_custom_target(ubuntu_printing_app_pages_qml_js_files ALL SOURCES ${PAGES_QML_JS_FILES})
