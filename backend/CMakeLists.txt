include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
#    /usr/include/cups/
#    /usr/include/poppler/qt5/
#    ${QT_INCLUDES}
#    ${Qt5PrintSupport_INCLUDE_DIRS}
)

#link_directories(${Qt5PrintSupport_INCLUDE_DIRS})

#add_definitions(
#  -DGETTEXT_PACKAGE=\"${PROJECT_NAME}\"
#  -DGETTEXT_LOCALEDIR=\"${CMAKE_INSTALL_LOCALEDIR}\"
#)

set(LIBNAME Ubuntu_Printing_Appbackend)

set(
  Ubuntu_Printing_App_SRC
  Ubuntu_Printing_App/backend.cpp
  Ubuntu_Printing_App/document.cpp
  Ubuntu_Printing_App/popplerimageprovider.cpp
  Ubuntu_Printing_App/printer.cpp
  Ubuntu_Printing_App/printerinfo.cpp
)

add_library(${LIBNAME} SHARED ${Ubuntu_Printing_App_SRC})

message("Qt5PrintSupport ${Qt5PrintSupport_INCLUDE_DIRS}")

qt5_use_modules(${LIBNAME} Core Gui Qml Quick PrintSupport)
target_link_libraries(${LIBNAME}
    cups
    poppler-qt5
#    Qt5::Core
#    Qt5::Gui
#    Qt5::Qml
#    Qt5::Quick
#    Qt5::PrintSupport
#    -lQt5PrintSupport
#    -lcups
#    -lpoppler-qt5
)

set_target_properties(${LIBNAME} PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY Ubuntu_Printing_App
)

# Copy the qmldir file to the build dir
add_custom_target(Ubuntu_Printing_App_qmldir ALL
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Ubuntu_Printing_App/qmldir ${CMAKE_CURRENT_BINARY_DIR}/Ubuntu_Printing_App
  DEPENDS ${QMLFILES})

# Install the plugin file
install(TARGETS ${LIBNAME} DESTINATION ${MODULE_PATH})
install(FILES Ubuntu_Printing_App/qmldir DESTINATION ${MODULE_PATH})
