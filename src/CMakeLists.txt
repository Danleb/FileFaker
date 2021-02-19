target_sources(FileFakerObjectLib PRIVATE
	"file_faker_server.c"
	"injector_loader.c"
	"pe_parser.c"
)

target_sources(FileFakerClient PRIVATE
	"main_dll.c"
	"file_faker_client.c"
	"file_faker_server.c"
	"injector_loader.c"
	"file_redirector.c"
	"file_functions_hooker.c"
	"fake_file_functions.c"
	"pe_parser.c"
)

target_sources(FileFakerExecutable PRIVATE
	"main.c"
)