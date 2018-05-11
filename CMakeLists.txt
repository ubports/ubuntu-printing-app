project(ubuntu-printing-app)
cmake_minimum_required(VERSION 2.8.9)

# Always Be Testing
#enable_testing()

# Load translation tools
find_package(Intltool REQUIRED)

# Set the vars
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")  # -fno-permissive -pedantic -Wall -Wextra")

# Set the options
option(SNAP_MODE "Build as a snap package" off)

# Find packages
find_package(Qt5Core REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

include(GNUInstallDirs)

# Add subdirs
#add_subdirectory(notifier)
add_subdirectory(ubuntu-printing-app)

# Add queue dialog
add_subdirectory(queue-dialog)

# Setup gettext defs and include po directory
set(GETTEXT_PACKAGE "ubuntu-printing-app")
add_subdirectory(po)

# Show files in QtC
file(GLOB QTC_FILES
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     *.cpp *.js .qml README*)

add_custom_target(ubuntu_printing_app_qtc_files DEPENDS Main.qml SOURCES ${QTC_FILES})
