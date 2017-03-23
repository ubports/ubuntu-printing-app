set(QUEUE_COMPONENTS_QML_JS_FILES
    QueueDelegate.qml
    QueueHelper.qml
    QueueView.qml
)

install(FILES ${QUEUE_COMPONENTS_QML_JS_FILES} DESTINATION ${QUEUE_DIALOG_DATA_DIR}/components)

# Show in QtCreator
add_custom_target(queue_components_qml_js_files ALL SOURCES ${QUEUE_COMPONENTS_QML_JS_FILES})

