find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5QuickTest REQUIRED)

find_program(XVFBRUN xvfb-run)
if(NOT XVFBRUN)
  message(FATAL_ERROR "Could not find xvfb-run, please install the xvfb package")
endif()
set(XVFB_COMMAND ${XVFBRUN} -s "-screen 0 640x480x24" -a)

set(TEST tst_QmlTests)
set(SOURCES
    tst_QmlTests.cpp
)
add_executable(${TEST} ${SOURCES})
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/backend/
)
target_link_libraries(${TEST}
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickTest
)

add_test(${TEST} ${XVFB_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/${TEST}
         -input ${CMAKE_CURRENT_SOURCE_DIR}
         -import ${CMAKE_BINARY_DIR}/src)


# Add the qml test files to build dir
set(QMLTEST_FILES
    tst_PrintRow.qml
)

add_custom_target(qmltest_copyFiles ALL
  COMMENT "Copying files: ${QMLTEST_FILES}")

foreach(FILENAME ${QMLTEST_FILES})
  add_custom_command(
    TARGET qmltest_copyFiles
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}" "${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}"
    )
endforeach(FILENAME)

# Show in QtC
add_custom_target(qmltest_files ALL SOURCES ${QMLTEST_FILES})
