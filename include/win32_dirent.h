#ifndef _DIRENT_WIN32_
#define _DIRENT_WIN32_

#ifdef __cplusplus
extern "C" {
#endif
    
#define DLL_EXPORT    __declspec(dllexport)
#include <stdio.h>
#include <time.h>
    
typedef int ff_t;
typedef int mode_t;
typedef int dev_t;
typedef int nlink_t;
typedef int uid_t;
typedef int st_gid;
typedef int gid_t;
typedef struct DIR DIR;

struct dirent
{
    int d_ino;
    ff_t d_off;
    signed short int d_reclen;
    unsigned char d_type;
    char d_name[256];    
};

#define S_ISLNK(st_mode) ((st_mode) & 0020000) ? 1 : 0
#define S_ISREG(st_mode) ((st_mode) & 0100000) ? 1 : 0
#define S_ISDIR(st_mode) ((st_mode) & 0040000) ? 1 : 0

DLL_EXPORT DIR *opendir(const char *name);

DLL_EXPORT int closedir(DIR *dir);

DLL_EXPORT struct dirent *readdir(DIR *dir);

DLL_EXPORT int chdir(const char *path);

DLL_EXPORT char *getcwd(char *buf, size_t size);

DLL_EXPORT void rewinddir(DIR *dir);

DLL_EXPORT int mkdir(const char *pathname, mode_t mode);

DLL_EXPORT int rmdir(const char *pathname);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /*_DIRENT_WIN32_*/
