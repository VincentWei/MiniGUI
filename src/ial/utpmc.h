#ifndef GUI_IAL_UTPMC_H
    #define GUI_IAL_UTPMC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define PEN_X_THRESHOLD    4
#define PEN_Y_THRESHOLD    4

BOOL    InitUTPMCInput (INPUT* input, const char* mdev, const char* mtype);
void    TermUTPMCInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_UTPMC_H */


