#ifndef	_EM86_H_
#define _EM86_H_

#define MOUSE_MOVE_PIXEL    5
#define MOUSE_MOVE_PIXEL_F  15

#define MOUSE_MAX_X         639
#define MOUSE_MAX_Y         464

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
BOOL    InitEm86Input (INPUT* input, const char* mdev, const char* mtype);
void    TermEm86Input (void);

#define IR_IOCTL 0x00450000
#define IR_IOCTL_READ_KEY               (IR_IOCTL + 0x00)
#define IR_IOCTL_FLUSH_KEY              (IR_IOCTL + 0x01)

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif	/* _EM86_H_ */


