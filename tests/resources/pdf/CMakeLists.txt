# Show files in QtC
file(GLOB PDF_TEST_FILES *.pdf *.txt)
add_custom_target(test_pdf_files ALL SOURCES ${PDF_TEST_FILES})
