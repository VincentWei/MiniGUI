#!/usr/bin/env python3
"""Check lease lock ordering and real cross-process surface exclusion.

Uses production drawing macros/file-lock helpers, not a DRM device or GPU.
"""
import pathlib
import subprocess
import tempfile

root = pathlib.Path(__file__).resolve().parents[1]
gdi = (root / 'src/newgdi/gdi.c').read_text()
acquire = gdi.split('BOOL drmAcquireDCSurface (', 1)[1].split(
    'BOOL drmReleaseDCSurface (', 1)[0]
release = gdi.split('BOOL drmReleaseDCSurface (', 1)[1].split(
    'HDC drmCreateDCFromNameEx (', 1)[0]
assert acquire.index('BLOCK_DRAW_SEM(') < acquire.index('LOCK(&__mg_gdilock)')
assert acquire.index('failed:') < acquire.index('UNBLOCK_DRAW_SEM(')
assert release.index('poll(&fence') < release.index('GAL_UpdateRects(')
assert release.index('GAL_UpdateRects(') < release.index('UNBLOCK_DRAW_SEM(')
assert release.index('UNLOCK(&__mg_gdilock)') < release.index('UNBLOCK_DRAW_SEM(')
assert release.index('UNBLOCK_DRAW_SEM(') < release.index('GAL_FreeSurface(')

dc = (root / 'src/include/dc.h').read_text()
macros = dc.split('#       define BLOCK_DRAW_SEM(pdc)', 1)[1].split(
    '#       define IS_SCREEN_SURFACE', 1)[0]
locks = (root / 'src/misc/lock_file.c').read_text().split(
    'void __mg_lock_file_for_read', 1)[1].split('#endif', 1)[0]
code = r'''
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
typedef struct { int shared_header, fd; } Surface;
typedef struct { Surface *surface; } DC;
'''
code += 'void __mg_lock_file_for_read' + locks
code += '#define BLOCK_DRAW_SEM(pdc)' + macros
code += r'''
int main(void) {
    alarm(10);
    char name[] = "lease-lock-XXXXXX";
    int fd = mkstemp(name);
    assert(fd >= 0);
    assert(unlink(name) == 0);
    Surface surface = {1, fd};
    DC dc = {&surface};
    int ready[2], resume[2];
    assert(pipe(ready) == 0 && pipe(resume) == 0);
    BLOCK_DRAW_SEM((&dc));
    pid_t child = fork();
    assert(child >= 0);
    if (child == 0) {
        struct flock read_lock = {0};
        read_lock.l_type = F_RDLCK;
        read_lock.l_whence = SEEK_SET;
        /* A compositor reader cannot enter while the lease owns writing. */
        assert(fcntl(fd, F_SETLK, &read_lock) == -1);
        assert(errno == EACCES || errno == EAGAIN);
        assert(write(ready[1], "r", 1) == 1);
        char signal;
        assert(read(resume[0], &signal, 1) == 1);
        __mg_lock_file_for_read(fd);
        assert(write(ready[1], "d", 1) == 1);
        __mg_unlock_file_for_read(fd);
        _exit(0);
    }
    char signal;
    assert(read(ready[0], &signal, 1) == 1 && signal == 'r');
    /* Simulate completion/publication before releasing the lease guard. */
    UNBLOCK_DRAW_SEM((&dc));
    assert(write(resume[1], "f", 1) == 1);
    assert(read(ready[0], &signal, 1) == 1 && signal == 'd');
    int status;
    assert(waitpid(child, &status, 0) == child);
    assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
    close(fd);
    puts("PASS: lease source lock ordering and cross-process write/read exclusion");
}
'''
with tempfile.TemporaryDirectory(prefix='minigui-lease-test-') as directory:
    path = pathlib.Path(directory)
    (path / 'test.c').write_text(code)
    subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror',
                    '-fsanitize=address,undefined', str(path / 'test.c'),
                    '-o', str(path / 'test')], check=True)
    subprocess.run([str(path / 'test')], cwd=directory, check=True, timeout=15)
