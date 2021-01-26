target_sources(FileFakerTests PRIVATE
	"ProcessRunnerTests.cpp"
	"ProcessRunner.cpp"
)

target_include_directories(FileFakerTests PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)