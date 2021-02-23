add_executable(TestFileReaderProgram "TestFileReaderProgram.cpp")

target_include_directories(TestFileReaderProgram PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(FileFakerTests "")

target_link_libraries(FileFakerTests
	FileFakerObjectLib
	${CONAN_LIBS}
)

target_sources(FileFakerTests PRIVATE
	"FileFakerTestsSameProcessF.cpp"
	"FileFakerTestsChildProcessF.cpp"
)

target_include_directories(FileFakerTests PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory("utils")

enable_testing()
add_test(NAME FileFakerTests
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMAND FileFakerTests
)

message("BINARY="${CMAKE_BINARY_DIR})