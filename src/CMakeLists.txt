target_sources(FileFakerObjectLib PRIVATE
	"file_faker_server.c"
	"injector_loader.c"
)

target_sources(FileFakerClient PRIVATE
	"main_dll.c"
	"file_faker_client.c"
)

target_sources(FileFakerExecutable PRIVATE
	"main.c"
)