aux_source_directory(src_file .)

add_library(help_func ${src_file} STATIC)

install(TARGETS help_func
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib/static)