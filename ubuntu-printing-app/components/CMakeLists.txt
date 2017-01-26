set(COMPONENT_QML_JS_FILES
    CheckBoxRow.qml
    LabelRow.qml
    PreviewRow.qml
    PrintRow.qml
    PrintingHelper.qml
    SelectorRow.qml
    TextFieldRow.qml
)

install(FILES ${COMPONENT_QML_JS_FILES} DESTINATION ${UBUNTU_PRINTING_APP_DATA_DIR}/components)

# Show in QtCreator
add_custom_target(ubuntu_printing_app_components_qml_js_files ALL SOURCES ${COMPONENT_QML_JS_FILES})
