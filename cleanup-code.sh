#!/bin/bash

tab2space() {
    sed -i 's/\t/    /g' `find . -name '*.c'`
    sed -i 's/\t/    /g' `find . -name '*.h'`

    sed -i 's/\s\+$//g' `find . -name '*.c'`
    sed -i 's/\s\+$//g' `find . -name '*.h'`
}

# templates
# sed -i 's/\<AAA\>/__AAA/g' `grep AAA -rl include/ src/`
# sed -i 's/\<AAA\>/__mgAAA/g' `grep AAA -rl include/ src/`
# sed -i 's/\<AAA\>/__mg_AAA/g' `grep AAA -rl include/ src/`
# sed -i 's/\<AAA\>/dbg_AAA/g' `grep AAA -rl include/ src/`

# sed -i 's/\<kernel_alloc_z_order_info\>/__kernel_alloc_z_order_info/g' `grep kernel_alloc_z_order_info -rl include/ src/`
# sed -i 's/\<kernel_free_z_order_info\>/__kernel_free_z_order_info/g' `grep kernel_free_z_order_info -rl include/ src/`
# sed -i 's/\<kernel_change_z_order_mask_rect\>/__kernel_change_z_order_mask_rect/g' `grep kernel_change_z_order_mask_rect -rl include/ src/`
# sed -i 's/\<kernel_get_window_region\>/__kernel_get_window_region/g' `grep kernel_get_window_region -rl include/ src/`
# sed -i 's/\<kernel_get_next_znode\>/__kernel_get_next_znode/g' `grep kernel_get_next_znode -rl include/ src/`
# sed -i 's/\<kernel_get_prev_znode\>/__kernel_get_prev_znode/g' `grep kernel_get_prev_znode -rl include/ src/`

# sed -i 's/\<gui_open_ime_window\>/__gui_open_ime_window/g' `grep gui_open_ime_window -rl include/ src/`

# sed -i 's/\<_begin_fill_bitmap\>/__mg_begin_fill_bitmap/g' `grep _begin_fill_bitmap -rl include/ src/`
# sed -i 's/\<_fill_bitmap_scanline\>/__mg_fill_bitmap_scanline/g' `grep _fill_bitmap_scanline -rl include/ src/`
# sed -i 's/\<_end_fill_bitmap\>/__mg_end_fill_bitmap/g' `grep _end_fill_bitmap -rl include/ src/`
# sed -i 's/\<DumpCtrlClassInfoTable\>/dbg_DumpCtrlClassInfoTable/g' `grep DumpCtrlClassInfoTable -rl include/ src/`
# sed -i 's/\<GetPixelUnderCursor\>/kernel_GetPixelUnderCursor/g' `grep GetPixelUnderCursor -rl include/ src/`
# sed -i 's/\<update_secondary_dc\>/__mg_update_secondary_dc/g' `grep update_secondary_dc -rl include/ src/`

# sed -i 's/\<initialize_scripteasy\>/font_InitializeScripteasy/g' `grep initialize_scripteasy -rl include/ src/`
# sed -i 's/\<uninitialize_scripteasy\>/font_UninitializeScripteasy/g' `grep uninitialize_scripteasy -rl include/ src/`

# sed -i 's/\<free_window_element_data\>/__mg_free_window_element_data/g' `grep free_window_element_data -rl include/ src/`
# sed -i 's/\<append_window_element_data\>/__mg_append_window_element_data/g' `grep append_window_element_data -rl include/ src/`
# sed -i 's/\<set_window_element_data\>/__mg_set_window_element_data/g' `grep set_window_element_data -rl include/ src/`
# sed -i 's/\<__compsor_check_znodes\>/__mg_compsor_check_znodes/g' `grep __compsor_check_znodes -rl include/ src/`

# sed -i 's/\<mg_linux_tty_init\>/__mg_linux_tty_init/g' `grep mg_linux_tty_init -rl include/ src/`
# sed -i 's/\<mg_linux_tty_enable_vt_switch\>/__mg_linux_tty_enable_vt_switch/g' `grep mg_linux_tty_enable_vt_switch -rl include/ src/`
# sed -i 's/\<mg_linux_tty_disable_vt_switch\>/__mg_linux_tty_disable_vt_switch/g' `grep mg_linux_tty_disable_vt_switch -rl include/ src/`
# sed -i 's/\<mg_linux_tty_switch_vt\>/__mg_linux_tty_switch_vt/g' `grep mg_linux_tty_switch_vt -rl include/ src/`
# sed -i 's/\<mg_linux_tty_fini\>/__mg_linux_tty_fini/g' `grep mg_linux_tty_fini -rl include/ src/`

# sed -i 's/\<DumpMenu\>/dbg_DumpMenu/g' `grep DumpMenu -rl include/ src/`

# sed -i 's/\<sysres_init_inner_resource\>/__sysres_init_inner_resource/g' `grep sysres_init_inner_resource -rl include/ src/`
# sed -i 's/\<sysres_get_system_res_path\>/__sysres_get_system_res_path/g' `grep sysres_get_system_res_path -rl include/ src/`

tab2space

exit 0
