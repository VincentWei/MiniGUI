
#mg_add_all_source_files()
list (APPEND textedit_sources 
    object.c mbtree.c mtextedit.c mwidget.c)

mg_add_source_files (${textedit_sources})
mg_commit_source_files()
