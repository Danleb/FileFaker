target_sources(FileFakerObjectLib PRIVATE
	"file_faker_server.c"
	"injector_loader.c"	
)

target_sources(FileFakerClient PRIVATE
	"main_dll.c"
	"file_faker_client.c"
	"file_faker_server.c"
	"injector_loader.c"
	"redirections_manager.c"
	"file_functions_hooker.c"
	"hook_file_functions.c"
	"imports_finder.c"
)

target_sources(FileFakerExecutable PRIVATE
	"main.c"
)