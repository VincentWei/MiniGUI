#mg_add_all_source_files()
list (APPEND sysres_icon_sources
    excalmatory-flat.ico.c
    folder.ico.c
    help-flat.ico.c
    warning-flat.ico.c
    excalmatory.ico.c
    fold-flat.ico.c
    help.ico.c
    textfile-flat.ico.c
    warning.ico.c
    failed-flat.ico.c
    fold.ico.c
    textfile.ico.c
    failed.ico.c
    form-flat.ico.c
    unfold-flat.ico.c
    folder-flat.ico.c
    form.ico.c
    unfold.ico.c)

mg_add_source_files (${sysres_icon_sources})

mg_commit_source_files()
