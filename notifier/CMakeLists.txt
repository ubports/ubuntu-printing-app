### Need to have own copy of gdbuscodegen module
list (APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

### Install path variables
include (GNUInstallDirs)
if (EXISTS "/etc/debian_version") # Workaround for libexecdir on debian
  set (CMAKE_INSTALL_LIBEXECDIR "${CMAKE_INSTALL_LIBDIR}")
  set (CMAKE_INSTALL_FULL_LIBEXECDIR "${CMAKE_INSTALL_FULL_LIBDIR}")
endif ()
set (CMAKE_INSTALL_PKGLIBEXECDIR "${CMAKE_INSTALL_LIBEXECDIR}/${CMAKE_PROJECT_NAME}")
set (CMAKE_INSTALL_FULL_PKGLIBEXECDIR "${CMAKE_INSTALL_FULL_LIBEXECDIR}/${CMAKE_PROJECT_NAME}")

### Always Be Testing
enable_testing()

### Set variables for our binary build targets
set(SERVICE_LIB_NAME "printing-notifier")
set(SERVICE_EXEC_NAME "printing-notifier-service")

### Some default compile flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -g -Wextra -Wall -Werror -Werror=conversion-null -Wno-ignored-qualifiers -fPIC")

### Find deps
find_package(PkgConfig REQUIRED)
find_package(Threads)

pkg_check_modules(SERVICE_DEPS REQUIRED
  gio-unix-2.0>=2.36
  glib-2.0>=2.36
  libnotify>=0.7.6
  properties-cpp
)

### Find libcups
find_program(CUPS_CONFIG cups-config)
execute_process(COMMAND ${CUPS_CONFIG} --cflags OUTPUT_VARIABLE CUPS_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${CUPS_CONFIG} --libs OUTPUT_VARIABLE CUPS_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

### Add sub-directories
add_subdirectory(data)
add_subdirectory(src)
add_subdirectory(tests)

### Enable coverage reporting
find_package(CoverageReport)
ENABLE_COVERAGE_REPORT(
  TARGETS ${SERVICE_LIB_NAME} ${SERVICE_EXEC_NAME}
  TESTS test-${SERVICE_LIB_NAME}
  FILTER /usr/include ${CMAKE_BINARY_DIR}/*
)
