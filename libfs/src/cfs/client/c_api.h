// The C front API of the library.
//

#ifndef __STAMNOS_CFS_CLIENT_C_FRONT_API
#define __STAMNOS_CFS_CLIENT_C_FRONT_API

#include <sys/types.h>
#include <stdint.h>

#define FRONTAPI(fname) cfs_##fname

#ifdef __cplusplus 
extern "C" { 
#endif


int FRONTAPI(init) (int argc, char* argv[]);
int FRONTAPI(init2) (const char* xdst);
int FRONTAPI(shutdown) ();
int FRONTAPI(open) (const char* path, int flags);
int FRONTAPI(open2) (const char* path, int flags, mode_t mode);
int FRONTAPI(close) (int fd);
int FRONTAPI(dup) (int oldfd);
int FRONTAPI(dup2) (int oldfd, int newfd);
int FRONTAPI(mount) (const char* source, const char* target, const char* fstype, uint32_t flags);
int FRONTAPI(umount) (const char* target);
int FRONTAPI(link) (const char* oldpath, const char* newpath);
int FRONTAPI(unlink) (const char* pathname);
int FRONTAPI(mkdir) (const char* path, int mode);
int FRONTAPI(rmdir) (const char* path);
int FRONTAPI(chdir) (const char* path);
char* FRONTAPI(getcwd) (char* buf, size_t size);

ssize_t FRONTAPI(read) (int fd, void *buf, size_t count);
ssize_t FRONTAPI(write) (int fd, const void *buf, size_t count);
ssize_t FRONTAPI(pread) (int fd, void *buf, size_t count, off_t offset);
ssize_t FRONTAPI(pwrite) (int fd, const void *buf, size_t count, off_t offset);
off_t FRONTAPI(lseek) (int fd, off_t offset, int whence);
int FRONTAPI(stat) (const char *path, struct stat *buf);
int FRONTAPI(sync) ();
int FRONTAPI(fsync) (int fd);

#ifdef __cplusplus 
} 
#endif


#endif /* __STAMNOS_CFS_CLIENT_C_FRONT_API_H */
