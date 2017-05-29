#mg_add_all_source_files()

list (APPEND sysres_bmp_sources 
    skin_arrows_shell.bmp.c
    skin_pb_htruck.bmp.c
    skin_sb_vshaft.bmp.c
    classic_check_button.bmp.c
    skin_bborder.bmp.c
    skin_pb_vtrack.gif.c
    skin_sb_vthumb.bmp.c
    classic_radio_button.bmp.c
    skin_bkgnd.bmp.c
    skin_pb_vtruck.bmp.c
    skin_tab.gif.c
    flat_check_button.bmp.c
    skin_caption.gif.c
    skin_pushbtn.gif.c
    skin_tb_horz.gif.c
    flat_radio_button.bmp.c
    skin_checkbtn.bmp.c
    skin_radiobtn.gif.c
    skin_tborder.bmp.c
    skin_cpn_btn.gif.c
    skin_rborder.bmp.c
    skin_tbslider_h.gif.c
    skin_header.bmp.c
    skin_sb_arrows.bmp.c
    skin_tbslider_v.gif.c
    skin_lborder.bmp.c
    skin_sb_hshaft.bmp.c
    skin_tb_vert.gif.c
    skin_arrows.gif.c
    skin_pb_htrack.gif.c
    skin_sb_hthumb.bmp.c
    skin_tree.bmp.c
    )
	 
mg_add_source_files (${sysres_bmp_sources})
mg_commit_source_files()
