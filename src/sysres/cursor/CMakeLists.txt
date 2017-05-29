#mg_add_all_source_files()

list (APPEND sysres_cursor_sources
    cursors.c)
mg_add_source_files (${sysres_cursor_sources})
mg_commit_source_files()
