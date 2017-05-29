#ifndef GUI_IAL_TSLIB_H
    #define GUI_IAL_TSLIB_H
#define MAX_KEYPAD_CODE     0x08

#define KEY_1           2
#define KEY_2           3
#define KEY_3           4
#define KEY_4           5
#define KEY_5           6
#define KEY_6           7
#define KEY_7           8



#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL  InitJZ4740Input (INPUT* input, const char* mdev, const char* mtype);
void  TermJZ4740Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_TSLIB_H */


