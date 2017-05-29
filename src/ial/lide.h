#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define MAX_KEYPAD_CODE     0x08
#define KEY_1           2
#define KEY_2           3
#define KEY_3           4
#define KEY_4           5
#define KEY_5           6
#define KEY_6           7
#define KEY_7           8
BOOL  InitlideInput (INPUT* input, const char* mdev, const char* mtype);
void  TermlideInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */



