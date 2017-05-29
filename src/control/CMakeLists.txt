
#append all files to _source_list_.
#mg_add_all_source_files ()
list (APPEND control_sources 
    ctrlmisc.c
    static.c button.c edit.c listbox.c progressbar.c
    menubutton.c trackbar.c combobox.c propsheet.c
    newtoolbar.c scrolled.c scrollwnd.c scrollview.c
    textedit.c listmodel.c scrollbar.c bidiedit.c
    )

mg_add_source_files (${control_sources})

#commit to parent
mg_commit_source_files ()

