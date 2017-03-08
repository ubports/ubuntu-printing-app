include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    /usr/include/poppler/qt5/
)

set(LIBNAME UbuntuPrintingAppbackend)

set(
  UbuntuPrintingAppSRC
  UbuntuPrintingApp/backend.cpp
  UbuntuPrintingApp/document.cpp
  UbuntuPrintingApp/popplerimageprovider.cpp
  UbuntuPrintingApp/pagehelper.cpp
)

add_library(${LIBNAME} SHARED ${UbuntuPrintingAppSRC})

qt5_use_modules(${LIBNAME} Core Gui Qml Quick)
target_link_libraries(${LIBNAME}
    poppler-qt5
)

set_target_properties(${LIBNAME} PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY UbuntuPrintingApp
)

# Copy the qmldir file to the build dir
add_custom_target(UbuntuPrintingApp_qmldir ALL
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/UbuntuPrintingApp/qmldir ${CMAKE_CURRENT_BINARY_DIR}/UbuntuPrintingApp
  DEPENDS ${QMLFILES})

# Install the plugin file
install(TARGETS ${LIBNAME} DESTINATION ${MODULE_PATH}/UbuntuPrintingApp)
install(FILES UbuntuPrintingApp/qmldir DESTINATION ${MODULE_PATH}/UbuntuPrintingApp)
