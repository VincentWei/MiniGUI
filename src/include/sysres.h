#ifndef _SYSRES_H
#define _SYSRES_H

#ifndef CURSORSECTION
#define CURSORSECTION    "cursorinfo"
#endif

//BOOL GUIAPI RegisterResFromRes (HDC hdc, const char* filename);
#define RegisterResFromRes(hdc, file)  (LoadResource(file,RES_TYPE_IMAGE, NULL) != NULL)


/*Load system cursor */
#ifdef _MGHAVE_CURSOR
PCURSOR sysres_load_system_cursor (int i);
#else
#define sysres_load_system_cursor(i)  ((PCURSOR)(NULL))
#endif

BOOL sysres_init_inner_resource(void);

const char* sysres_get_system_res_path(void);


#endif /* _SYSRES_H */
