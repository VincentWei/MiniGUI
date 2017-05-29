#ifndef GUI_IAL_DAVINCI6446_H
    #define GUI_IAL_DAVINCI6446_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL  InitDavinci6446Input (INPUT* input, const char* mdev, const char* mtype);
void  TermDavinci6446Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_DAVINCI6446_H */


