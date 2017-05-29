/*
**  $Id: mlshadow.h 7548 2007-09-12 06:31:34Z weiym $
**  
**  Copyright (C) 2007 Feynman Software.
*/

#ifndef _GAL_MLSHADOW_H
#define _GAL_MLSHADOW_H

#include "sysvideo.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Hidden "this" pointer for the video functions */
#define _THIS    GAL_VideoDevice *this

/* MLShadow_node.DWORD FLAGS  */
    
#define MLSF_ENABLED    0x01
#define MLSF_COLORKEY   0x02
#define MLSF_ALPHA      0x04

#define LEN_ENGINE_NAME 10

/* Private display data */

struct GAL_PrivateVideoData {
    BOOL dirty;
    RECT update;
    pthread_t update_th;
    pthread_mutex_t update_lock;
    /*MLMLSHADOW global static variable*/
    unsigned int _real_def_bgcolor;
    GAL_VideoDevice* _real_device;
    GAL_Surface* swap_surface;
    GAL_Surface* _real_surface;
    list_t _mlshadow_list;
};

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _GAL_MLSHADOW_H */
