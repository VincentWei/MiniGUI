#ifndef GUI_IAL_PX255B_H
    #define GUI_IAL_PX255B_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitPX255BInput (INPUT* input, const char* mdev, const char* mtype);
void    TermPX255BInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_PX255B_H */


