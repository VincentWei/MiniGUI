
# add source files to 'newgdi_sources', depend on variable of 'MG_MINIMALGDI'
if (MG_MINIMALGDI)
    set (newgdi_sources
            gdi.c attr.c clip.c coor.c rect.c
            bitmap.c
            pixel.c pixel_ops.c
            region.c polygon.c
        )
else ()
    set (newgdi_sources
            gdi.c attr.c clip.c map.c coor.c rect.c
            palette.c readbmp.c icon.c screen.c bitmap.c
            pixel.c line.c arc.c pixel_ops.c
            region.c generators.c polygon.c flood.c
            advapi.c midash.c mispans.c miwideline.c
            mifillarc.c mifpolycon.c miarc.c rotatebmp.c
            text.c glyph.c bidi.c
            textout.c tabbedtextout.c drawtext.c
        )
endif (MG_MINIMALGDI)

#add hander files
list (APPEND newgdi_sources
        drawtext.h mi.h midc.h mistruct.h miwideline.h
        pixel_ops.h mifillarc.h mispans.h polygon.h mifpoly.h 
     )

# add files to _source_list_
mg_add_source_files (${newgdi_sources})

#commit to parent
mg_commit_source_files ()
