add_subdirectory(pdf)

# Copy the test pdf files to the build dir
add_custom_target(test_pdf_copyFiles ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/pdf ${CMAKE_CURRENT_BINARY_DIR}/pdf
  DEPENDS ${PDF_TEST_FILES})
