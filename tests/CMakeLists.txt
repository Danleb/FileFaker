add_executable(FileFakerTests "")

target_link_libraries(FileFakerTests 
	FileFakerObjectLib
	${CONAN_LIBS}
)

enable_testing()
add_test(FileFakerTests
	${CMAKE_BINARY_DIR}
	FileFakerTests
)