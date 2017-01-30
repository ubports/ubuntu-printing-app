project(ubuntu-printing-app)
cmake_minimum_required(VERSION 2.8.9)

# Load translation tools
find_program(INTLTOOL_MERGE intltool-merge)
if(NOT INTLTOOL_MERGE)
    message(FATAL_ERROR "Could not find intltool-merge, please install the intltool package")
endif()

find_program(INTLTOOL_EXTRACT intltool-extract)
if(NOT INTLTOOL_EXTRACT)
    message(FATAL_ERROR "Could not find intltool-extract, please install the intltool package")
endif()

# Set the vars
set(APP_NAME ubuntu-printing-app)
set(APP_HARDCODE ubuntu-printing-app)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")  # -fno-permissive -pedantic -Wall -Wextra")
set(DESKTOP_FILE "${APP_NAME}.desktop")
set(ICON_FILE ubuntu-printing-app.png)
set(MAIN_QML Main.qml)
set(RUNNER ${APP_HARDCODE})
set(SNAP_DESKTOP_FILE setup/gui/${DESKTOP_FILE})

# Set the options
option(SNAP_MODE "Build as a snap package" off)

# Find packages
find_package(Qt5Core REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

include(GNUInstallDirs)


# Set the path for the QML files and the backend
set(UBUNTU_PRINTING_APP_DATA_DIR "${CMAKE_INSTALL_DATADIR}/${APP_HARDCODE}")
execute_process(
COMMAND qmake -query QT_INSTALL_QML
        OUTPUT_VARIABLE QT_IMPORTS_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(MODULE_PATH ${QT_IMPORTS_DIR}/Ubuntu_Printing_App)

# Set the exec path
if(SNAP_MODE)
  set(EXEC "APP_ID=${APP_HARDCODE} qmlscene $@ -I $SNAP/${MODULE_PATH} $SNAP/${CMAKE_INSTALL_PREFIX}/${UBUNTU_PRINTING_APP_DATA_DIR}/${MAIN_QML}")
  set(ICON "$SNAP/${CMAKE_INSTALL_PREFIX}/${UBUNTU_PRINTING_APP_DATA_DIR}/${ICON_FILE}")
else(SNAP_MODE)
  set(EXEC "APP_ID=${APP_HARDCODE} qmlscene $@ -I ${MODULE_PATH} ${CMAKE_INSTALL_PREFIX}/${UBUNTU_PRINTING_APP_DATA_DIR}/${MAIN_QML}")
  set(ICON ${CMAKE_INSTALL_PREFIX}/${UBUNTU_PRINTING_APP_DATA_DIR}/${ICON_FILE})
endif(SNAP_MODE)


# Add subdirs
add_subdirectory(backend)
add_subdirectory(runner)
add_subdirectory(ubuntu-printing-app)

# Setup gettext defs and include po directory
set(GETTEXT_PACKAGE "ubuntu-printing-app")
add_definitions(-DI18N_DOMAIN="${GETTEXT_PACKAGE}")

add_subdirectory(po)

# Add tests
find_package(Qt5Test REQUIRED)
enable_testing()
add_subdirectory(tests)


# Add run command for QtC
add_custom_target("run" /usr/bin/qmlscene -I ${CMAKE_BINARY_DIR}/backend  ${CMAKE_SOURCE_DIR}/ubuntu-printing-app/Main.qml
                  DEPENDS Ubuntu_Printing_App Ubuntu_Printing_App_qmldir
                  WORKING_DIRECTORY ./ubuntu-printing-app)

# Show files in QtC
file(GLOB QTC_FILES
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     *.cpp *.js .qml README*)

add_custom_target(ubuntu_printing_app_qtc_files DEPENDS Main.qml SOURCES ${QTC_FILES})
