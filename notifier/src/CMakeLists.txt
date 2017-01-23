add_definitions(
  -DGETTEXT_PACKAGE=\"${PROJECT_NAME}\"
  -DGETTEXT_LOCALEDIR=\"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LOCALEDIR}\"
  -DG_LOG_DOMAIN=\"${CMAKE_PROJECT_NAME}\"
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${SERVICE_DEPS_INCLUDE_DIRS}
)
link_directories(
  ${SERVICE_DEPS_LIBRARY_DIRS}
)

include (GdbusCodegen)
add_gdbus_codegen(SERVICE_GENERATED_SOURCES cups-cupsd-notifier
                  org.cups.cupsd
                  ${CMAKE_CURRENT_SOURCE_DIR}/org.cups.cupsd.Notifier.xml)

add_library(${SERVICE_LIB_NAME} STATIC
  client.h
  cups-client.cpp
  cups-client.h
  job.h
  notification.cpp
  notification.h
  notify-engine.cpp
  notify-engine.h
  printer.h
  utils.cpp
  utils.h
  ${SERVICE_GENERATED_SOURCES}
)

target_link_libraries(${SERVICE_LIB_NAME}
  ${SERVICE_DEPS_LIBRARIES}
  ${CUPS_LIBS}
)


add_executable(${SERVICE_EXEC_NAME}
  main.cpp
)

target_link_libraries(${SERVICE_EXEC_NAME}
  ${SERVICE_LIB_NAME}
  ${SERVICE_DEPS_LIBRARIES}
  ${CUPS_LIBS}

  ${CMAKE_THREAD_LIBS_INIT}
)

install(
  TARGETS ${SERVICE_EXEC_NAME}
  RUNTIME
  DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/${PROJECT_NAME}
)
