/*
**  $Id: mlshadow.h 8035 2007-11-02 07:46:07Z alexwang $
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
    

/* Private display data */

struct GAL_PrivateVideoData {
    pthread_t update_th;
    GAL_VideoDevice* _real_device;
    GAL_Surface* _real_surface;
    GAL_Surface* swap_surface;
    /*pthread_mutex_t update_lock; */
    /*MLMLSHADOW global static variable*/
    unsigned int _real_def_bgcolor;
    };

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif 

