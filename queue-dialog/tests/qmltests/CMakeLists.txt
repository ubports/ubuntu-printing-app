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

find_program(QMLTESTRUNNER qmltestrunner)
if(NOT QMLTESTRUNNER)
    message(FATAL_ERROR "Could not find qmltestrunner, please install the package qtdeclarative5-dev-tools")
endif()

set(TEST tst_QueueDialogQmlTests)
add_test(${TEST} ${XVFB_COMMAND} ${QMLTESTRUNNER} -qt=qt5
         -input ${CMAKE_CURRENT_SOURCE_DIR})


# Add the qml test files to build dir
set(QUEUE_QMLTEST_FILES
    tst_QueueDelegate.qml
    tst_QueuePage.qml
    tst_QueueView.qml
)

add_custom_target(queue_qmltest_copyFiles ALL
  COMMENT "Copying files: ${QUEUE_QMLTEST_FILES}")

foreach(FILENAME ${QUEUE_QMLTEST_FILES})
  add_custom_command(
    TARGET queue_qmltest_copyFiles
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}" "${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}"
    )
endforeach(FILENAME)

# Show in QtC
add_custom_target(queue_qmltest_files ALL SOURCES ${QUEUE_QMLTEST_FILES})
