set(QUEUE_PAGES_QML_JS_FILES
    QueuePage.qml
)

install(FILES ${QUEUE_PAGES_QML_JS_FILES} DESTINATION ${QUEUE_DIALOG_DATA_DIR}/pages)

# Show in QtCreator
add_custom_target(queue_pages_qml_js_files ALL SOURCES ${QUEUE_PAGES_QML_JS_FILES})

