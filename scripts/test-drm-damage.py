#!/usr/bin/env python3
"""Compile actual DRM copy/state helpers with mock buffers and flip events.
No display device or libdrm is used. This is not a target driver build.
"""
import pathlib, subprocess, tempfile, sys
root = pathlib.Path(sys.argv[1]) if len(sys.argv)>1 else pathlib.Path(__file__).resolve().parents[1]
s = (root/'src/newgal/drm/drmvideo.c').read_text()
def section(a,b): return s[s.index(a):s.index(b,s.index(a))]
code = r'''
#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <semaphore.h>
#define IS_SHAREDFB_SCHEMA_PROCS 0
#define retry_syscall_for_eintr(call) do { } while ((call) < 0 && errno == EINTR)
#define _WRN_PRINTF(...) ((void)0)
struct dma_buf_sync { uint64_t flags; };
#define DMA_BUF_SYNC_READ 1
#define DMA_BUF_SYNC_WRITE 2
#define DMA_BUF_SYNC_START 0
#define DMA_BUF_SYNC_END 4
#define DMA_BUF_IOCTL_SYNC 99
static int sync_calls;
static int ioctl(int fd, int request, struct dma_buf_sync *sync) {
 assert(fd==42 && request==DMA_BUF_IOCTL_SYNC);
 assert(sync->flags==(DMA_BUF_SYNC_READ|DMA_BUF_SYNC_START) ||
        sync->flags==(DMA_BUF_SYNC_READ|DMA_BUF_SYNC_END));
 sync_calls++; return 0;
}
typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define _ERR_PRINTF(...) ((void)0)
typedef struct {int left,top,right,bottom;} RECT;
typedef struct {int x,y,w,h;} GAL_Rect;
#define RECTW(r) ((r).right-(r).left)
#define RECTH(r) ((r).bottom-(r).top)
static int IsRectEmpty(const RECT *r){return r->left>=r->right||r->top>=r->bottom;}
static void SetRectEmpty(RECT *r){memset(r,0,sizeof(*r));}
static int IntersectRect(RECT *d,const RECT *a,const RECT *b){
 RECT r={a->left>b->left?a->left:b->left,a->top>b->top?a->top:b->top,
 a->right<b->right?a->right:b->right,a->bottom<b->bottom?a->bottom:b->bottom};*d=r;return !IsRectEmpty(d);}
static void GetBoundRect(RECT *d,const RECT *a,const RECT *b){
 RECT r={a->left<b->left?a->left:b->left,a->top<b->top?a->top:b->top,
 a->right>b->right?a->right:b->right,a->bottom>b->bottom?a->bottom:b->bottom};*d=r;}
typedef struct {uint8_t *vaddr;int offset,pitch;unsigned width,height,fb_id;int prime_fd;} DrmSurfaceBuffer;
typedef struct {int BytesPerPixel;} Format;
typedef struct {void *hwdata;uint8_t *pixels;int pixels_off,pitch,w,h;Format *format;} GAL_Surface;
typedef struct {int (*copy_buff)(void*,DrmSurfaceBuffer*,const GAL_Rect*,DrmSurfaceBuffer*,const GAL_Rect*,int);void (*flush)(void*,DrmSurfaceBuffer*,const GAL_Rect*);} Ops;
typedef struct {GAL_Surface *shadow_screen,*real_screen,*flip_screen;DrmSurfaceBuffer *flip_buff,*curr_buff,*pending_flip;RECT buffer_damage[2];void *driver;Ops *driver_ops;int min_pixels_using_hwaccl,dev_fd;unsigned scanout_buff_id;struct {unsigned crtc_id;} *saved_crtc;int dbl_buff;sem_t *update_lock;RECT dirty_rc;} DrmVideoData;
typedef struct {DrmVideoData *hidden;} Device;
#define _THIS Device *this
#define BLIT_COPY_TRANSLATE 0
#define DRM_MODE_PAGE_FLIP_EVENT 1
static void refresh_cursor(Device *d,const GAL_Rect *r,GAL_Surface *s){(void)d;(void)r;(void)s;}
typedef struct {int version; void (*page_flip_handler)(int,unsigned,unsigned,unsigned,void*);} drmEventContext;
static int event_ready,flip_fail;
static void *event_data;
static int mock_poll(struct pollfd *p, unsigned long n,int timeout){(void)n;(void)timeout;p->revents=event_ready?POLLIN:0;return event_ready;}
#define poll mock_poll
static int drmHandleEvent(int fd,drmEventContext *e){event_ready=0;e->page_flip_handler(fd,0,0,0,event_data);return 0;}
static int drmModePageFlip(int fd,unsigned crtc,unsigned fb,unsigned flags,void *data){(void)fd;(void)crtc;(void)fb;(void)flags;if(flip_fail)return -1;event_data=data;return 0;}
'''
code += section('static void update_real_screen_memcpy(', '\n#if 0\nstatic void DRM_OnBeforeUpdate')
code += section('static void flip_complete(', '\n#define retry_syscall_for_eintr')
code += section('static BOOL DRM_SyncUpdate(_THIS)\n{', '\nstatic BOOL DRM_SyncUpdateAsync')
code += r'''
int main(void){
 enum {W=8,H=6,SP=40,DP=48,OFF=8};
 uint8_t src[OFF+SP*H+8], a[OFF+DP*H+8], b[OFF+DP*H+8];
 memset(src,0x31,sizeof(src));memset(a,0xA5,sizeof(a));memset(b,0xA5,sizeof(b));
 Format fmt={4}; DrmSurfaceBuffer real={a,OFF,DP,W,H,1,-1},flip={b,OFF,DP,W,H,2,-1};
 GAL_Surface shadow={0,src,OFF,SP,W,H,&fmt},screen={&real,a,OFF,DP,W,H,&fmt};
 DrmVideoData v={0};v.shadow_screen=&shadow;v.real_screen=&screen;v.curr_buff=&real;v.flip_buff=&flip;
 struct {unsigned crtc_id;} crtc={7};v.saved_crtc=(void*)&crtc;
 Device dev={&v};v.buffer_damage[0]=(RECT){0,0,W,H};v.buffer_damage[1]=v.buffer_damage[0];
 GAL_Rect r={1,1,1,1};
 update_real_screen_helper(&dev,&r);assert(b[OFF]==0x31);assert(a[OFF]==0xA5);
 assert(drm_page_flip(&v)==0);assert(v.curr_buff==&real && v.pending_flip==&flip);
 assert(!finish_pending_flip(&v,0));assert(v.curr_buff==&real);
 event_ready=1;assert(finish_pending_flip(&v,0));assert(v.curr_buff==&flip);
 update_real_screen_helper(&dev,&r);assert(a[OFF]==0x31);
 assert(drm_page_flip(&v)==0);event_ready=1;assert(finish_pending_flip(&v,0));
 /* Alternate disjoint changes. Each buffer must accumulate the intervening
    frame's damage; compare all visible pixels against the canonical shadow. */
 for(int frame=0;frame<40;frame++){
  int x=frame%W,y=(frame*3+frame/8)%H;
  memset(src+OFF+y*SP+x*4,frame+50,4);r=(GAL_Rect){x,y,1,1};
  DrmSurfaceBuffer *target=v.curr_buff==&flip?&real:&flip;
  uint8_t before[sizeof(a)];memcpy(before,target->vaddr,sizeof(before));
  update_real_screen_helper(&dev,&r);
  for(int row=0;row<H;row++){
   assert(!memcmp(target->vaddr+OFF+row*DP,src+OFF+row*SP,W*4));
   for(int pad=W*4;pad<DP;pad++)assert(target->vaddr[OFF+row*DP+pad]==0xA5);
  }
  assert(target->vaddr[0]==0xA5 && target->vaddr[OFF+DP*H]==0xA5);
  if(frame==10){flip_fail=1;assert(drm_page_flip(&v)==-1);assert(!v.pending_flip);flip_fail=0;}
  assert(drm_page_flip(&v)==0);event_ready=1;assert(finish_pending_flip(&v,0));
 }
 /* Single-buffer copy honors clipping and leaves untouched pixels/padding. */
 v.flip_buff=NULL;v.curr_buff=&real;memset(a,0xA5,sizeof(a));
 r=(GAL_Rect){-2,-1,4,3};update_real_screen_memcpy(&dev,&r);
 for(int y=0;y<H;y++)for(int x=0;x<W*4;x++)
  assert(a[OFF+y*DP+x]==((y<2 && x<8)?src[OFF+y*SP+x]:0xA5));
 r=(GAL_Rect){W+2,H+2,3,3};uint8_t saved[sizeof(a)];memcpy(saved,a,sizeof(a));
 update_real_screen_memcpy(&dev,&r);assert(!memcmp(saved,a,sizeof(a)));
 /* A real scanout with no shadow must never flip to untouched storage. */
 v.shadow_screen=NULL;v.flip_buff=&flip;v.dbl_buff=0;
 memset(a,0x77,sizeof(a));memset(b,0,sizeof(b));
 v.dirty_rc=(RECT){0,0,W,H};
 assert(DRM_SyncUpdate(&dev));assert(!v.pending_flip);
 assert(v.curr_buff==&real && a[OFF]==0x77 && b[OFF]==0);
 /* Exercise the synchronous double-buffer path and mocked PRIME sync. */
 DrmSurfaceBuffer source={src,OFF,SP,W,H,0,42};shadow.hwdata=&source;
 v.shadow_screen=&shadow;v.dbl_buff=1;v.update_lock=SEM_FAILED;
 v.dirty_rc=(RECT){0,0,W,H};
 assert(DRM_SyncUpdate(&dev));assert(v.pending_flip==&flip);
 event_ready=1;assert(finish_pending_flip(&v,0));
 assert(sync_calls==2);assert(b[OFF]==src[OFF]);
 puts("PASS: initial buffers, 40 alternating damage frames, pitch/offset/clipping guards, pending/failed flips, single-buffer SyncUpdate and DMA-BUF sync");
}
'''
with tempfile.TemporaryDirectory(prefix='minigui-drm-test-') as d:
 p=pathlib.Path(d);(p/'test.c').write_text(code)
 subprocess.run(['cc','-std=c11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-I'+str(root/'src/newgal/drm'),str(p/'test.c'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
