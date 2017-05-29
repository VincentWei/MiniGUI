
#message(STATUS "add source file in server dir start")
list (APPEND server_sources 
        server.c servaccept.c 
        servlisten.c layer.c 
        client.c request.c
    )

mg_add_source_files(${server_sources})
mg_commit_source_files()

#message(STATUS "add source file in server dir ok")

