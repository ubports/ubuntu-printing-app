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
set(UBUNTU_MANIFEST_PATH "manifest.json.in" CACHE INTERNAL "Tell QtCreator the location and name o the manifest file")
set(DESKTOP_FILE "${APP_NAME}.desktop")
set(ICON_FILE ubuntu-printing-app.png)
set(MAIN_QML Main.qml)
set(RUNNER ${APP_HARDCODE})
set(SNAP_DESKTOP_FILE setup/gui/${DESKTOP_FILE})

# Set the options
option(CLICK_MODE "Build as a click package" off)
option(SNAP_MODE "Build as a snap package" off)

# Find packages
find_package(Qt5Core)
find_package(Qt5Qml)
find_package(Qt5Quick)
find_package(Qt5PrintSupport)

include(GNUInstallDirs)


# Set the path for the QML files and the backend
if(CLICK_MODE)
  message("CLICK_MODE is not supported at this time!")
else(CLICK_MODE)
  set(UBUNTU_PRINTING_APP_DATA_DIR "${CMAKE_INSTALL_DATADIR}/${APP_HARDCODE}")
  execute_process(
    COMMAND qmake -query QT_INSTALL_QML
            OUTPUT_VARIABLE QT_IMPORTS_DIR
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  set(ICON ${CMAKE_INSTALL_PREFIX}/${UBUNTU_PRINTING_APP_DATA_DIR}/${ICON_FILE})
  set(MODULE_PATH ${QT_IMPORTS_DIR}/Ubuntu_Printing_App)
endif(CLICK_MODE)

# Set the exec path
if (CLICK_MODE)
  message("CLICK_MODE is not supported at this time!")
else(CLICK_MODE)
  if(SNAP_MODE)
    set(EXEC "APP_ID=${APP_HARDCODE} qmlscene $@ -I $SNAP/${MODULE_PATH} $SNAP/${CMAKE_INSTALL_PREFIX}/${UBUNTU_PRINTING_APP_DATA_DIR}/${MAIN_QML}")
  else(SNAP_MODE)
    set(EXEC "APP_ID=${APP_HARDCODE} qmlscene $@ -I ${MODULE_PATH} ${CMAKE_INSTALL_PREFIX}/${UBUNTU_PRINTING_APP_DATA_DIR}/${MAIN_QML}")
  endif(SNAP_MODE)
endif(CLICK_MODE)


# Find translatable files
file(GLOB_RECURSE I18N_SRC_FILES
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/po
     *.qml *.js *.cpp)
list(APPEND I18N_SRC_FILES ${DESKTOP_FILE}.in.in.h)
list(SORT I18N_SRC_FILES)


# Add subdirs
add_subdirectory(backend)
add_subdirectory(runner)
add_subdirectory(ubuntu-printing-app)


# Add run command for QtC
add_custom_target("run" /usr/bin/qmlscene -I ${CMAKE_BINARY_DIR}/backend  ${CMAKE_SOURCE_DIR}/ubuntu-printing-app/Main.qml
                  DEPENDS Ubuntu_Printing_App Ubuntu_Printing_App_qmldir
                  WORKING_DIRECTORY ./ubuntu-printing-app)

# Show files in QtC
file(GLOB QTC_FILES
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     *.cpp *.js .qml README*)

add_custom_target(ubuntu_printing_app_qtc_files DEPENDS Main.qml SOURCES ${QTC_FILES})
