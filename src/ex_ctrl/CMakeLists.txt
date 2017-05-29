
#mg_add_all_source_files()
list (APPEND ex_ctrl_sources
	treeview.c
	treeview_rdr.c
	monthcalendar.c
    spinbox.c
	coolbar.c
	listview.c
    gif89a.c
	animation.c
	iconview.c
	gridview.c
	gridviewcelltype.c
    )
mg_add_source_files (${ex_ctrl_sources})

mg_commit_source_files()
