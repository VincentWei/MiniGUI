
# add sub dirs
mg_add_sub_dir_with_condition (fbcon        _MGGAL_FBCON)

mg_add_sub_dir_with_condition (dfb          _MGGAL_DFB)

mg_add_sub_dir_with_condition (dummy        _MGGAL_DUMMY)
mg_add_sub_dir_with_condition (qvfb         _MGGAL_QVFB)
mg_add_sub_dir_with_condition (wvfb         _MGGAL_WVFB)
mg_add_sub_dir_with_condition (commlcd      _MGGAL_COMMLCD)
mg_add_sub_dir_with_condition (shadow       _MGGAL_SHADOW)
mg_add_sub_dir_with_condition (mlshadow     _MGGAL_MLSHADOW)
mg_add_sub_dir_with_condition (em85xxosd    _MGGAL_EM85XXOSD)
mg_add_sub_dir_with_condition (em85xxyuv    _MGGAL_EM85XXYUV)
mg_add_sub_dir_with_condition (em86gfx      _MGGAL_EM86GFX)
mg_add_sub_dir_with_condition (svpxxosd     _MGGAL_SVPXXOSD)
mg_add_sub_dir_with_condition (bf533        _MGGAL_BF533)
mg_add_sub_dir_with_condition (mb93493      _MGGAL_MB93493)
mg_add_sub_dir_with_condition (utpmc        _MGGAL_UTPMC)
mg_add_sub_dir_with_condition (nexus        _MGGAL_NEXUS)
mg_add_sub_dir_with_condition (s3c6410      _MGGAL_S3C6410)
#mg_add_sub_dir_with_condition (customgal    _MGGAL_CUSTOMGAL)
mg_add_sub_dir_with_condition (sigma8654    _MGGAL_SIGMA8654)
mg_add_sub_dir_with_condition (mstar        _MGGAL_MSTAR)
mg_add_sub_dir_with_condition (hisi         _MGGAL_HI3560A)
mg_add_sub_dir_with_condition (gdl          _MGGAL_GDL)
mg_add_sub_dir_with_condition (dfb          _MGGAL_ST7167)
mg_add_sub_dir_with_condition (stgfb        _MGGAL_STGFB)

mg_add_sub_dir_with_condition (pcxvfb       _MGGAL_PCXVFB)
mg_add_sub_dir_with_condition (rtos_xvfb    _MGGAL_RTOSXVFB)

if (_MGGAL_HI3560 OR _MGGAL_HI3510)
    mg_add_sub_source_dirs (hisi)
endif (_MGGAL_HI3560 OR _MGGAL_HI3510)

# add all current sources
list (APPEND newgal_sources 
    RLEaccel.c
    RLEaccel_c.h
    blit.c
    blit.h
    blit_0.c
    blit_1.c
    blit_A.c
    blit_A.h
    blit_N.c
    leaks.h
    pixels.c
    pixels_c.h
    surface.c
    stretch.c
    stretch_c.h
    sysvideo.h
    video.c
    newgal.c
	videomem-bucket.h
	videomem-bucket.c
    )

#mg_add_all_source_files()
mg_add_source_files (${newgal_sources})
mg_commit_source_files()

