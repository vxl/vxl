#ifndef vpl_unistd_h_
#define vpl_unistd_h_
// .NAME vpl_unistd
// .INCLUDE vpl/vpl_unistd.h
// .FILE vpl_unistd.cxx
// @author fsm@robots.ox.ac.uk
//
// .SECTION Description
//
// What vpl_unistd.h is meant to do is to include <unistd.h> properly
// on systems that have it. Eg. :
// 1. On some UNIX systems, usleep() is not declared in
//    unistd although the man pages say it is. So we declare it
//    but don't provide a new definition.
// 2. Some older libcs dont't have all the posix functions, such
//    as pread() and pwrite(). So we provide alternative definitions
//    for those platforms where we can't get a system version.
// 3. On windows, unlink() is called _unlink(), buth with
//    vpl_unistd.h it's called vpl_unlink() on both UNIX and 
//    windows.

// This list is from 
// http://www.opengroup.org/onlinepubs/007908799/xsh/unistd.h.html
// and includes also the functions declared in posix sys/stat.h

#include <vxl_config.h>        // must be first!!
#include <vcl/vcl_compiler.h>
#include <vcl/vcl_cstddef.h> // size_t
typedef vcl_size_t vpl_size_t;
#include <vcl/vcl_cstdlib.h>
#include <vcl/vcl_sys/types.h>


// fixme this needs configging
#ifndef _MSC_VER
typedef ssize_t	    vpl_ssize_t;
typedef uid_t	    vpl_uid_t;
typedef gid_t	    vpl_gid_t;
typedef pid_t	    vpl_pid_t; 
#else
typedef size_t	    vpl_ssize_t;
typedef int	    vpl_uid_t;
typedef int	    vpl_gid_t;
typedef int	    vpl_pid_t; 
#endif
typedef off_t	    vpl_off_t;
#if VXL_UNISTD_HAS_USECONDS_T
typedef useconds_t  vpl_useconds_t;
#else
typedef unsigned    vpl_useconds_t;
#endif
#if VXL_UNISTD_HAS_INTPTR_T
typedef intptr_t    vpl_intptr_t;
#else
typedef int         vpl_intptr_t;
#endif

// sys/stat.h:
#include <sys/stat.h>
#ifdef _MSC_VER
typedef struct _stat vpl_stat_t;
#else
typedef struct stat vpl_stat_t;
#endif
#ifndef _MSC_VER
typedef mode_t vpl_mode_t;
#else
typedef int vpl_mode_t;
#endif
typedef dev_t vpl_dev_t;

int    		vpl_chmod(const char *, vpl_mode_t);
int    		vpl_fchmod(int, vpl_mode_t);
int    		vpl_fstat(int, vpl_stat_t *);
int    		vpl_lstat(const char *, vpl_stat_t *);
int    		vpl_mkdir(const char *, vpl_mode_t);
int    		vpl_mkfifo(const char *, vpl_mode_t);
int    		vpl_mknod(const char *, vpl_mode_t, vpl_dev_t);
int    		vpl_stat(const char *, vpl_stat_t *);
vpl_mode_t	vpl_umask(vpl_mode_t);

// unistd.h:
int             vpl_access(const char *, int);
unsigned int    vpl_alarm(unsigned int);
int             vpl_brk(void *);
int             vpl_chdir(const char *);
int             vpl_chown(const char *, vpl_uid_t, vpl_gid_t);
int             vpl_close(int);
vpl_size_t      vpl_confstr(int, char *, vpl_size_t);
char           *vpl_crypt(const char *, const char *);
char           *vpl_ctermid(char *);
int             vpl_dup(int);
int             vpl_dup2(int, int);
void            vpl_encrypt(char[64], int);
int             vpl_execl(const char *, const char *, ...);
int             vpl_execle(const char *, const char *, ...);
int             vpl_execlp(const char *, const char *, ...);
int             vpl_execv(const char *, char *const []);
int             vpl_execve(const char *, char *const [], char *const []);
int             vpl_execvp(const char *, char *const []);
int             vpl_fchown(int, vpl_uid_t, vpl_gid_t);
int             vpl_fchdir(int);
int             vpl_fdatasync(int);
vpl_pid_t       vpl_fork(void);
long int        vpl_fpathconf(int, int);
int             vpl_fsync(int);
int             vpl_ftruncate(int, vpl_off_t);
char           *vpl_getcwd(char *, vpl_size_t);
vpl_gid_t       vpl_getegid(void);
vpl_uid_t       vpl_geteuid(void);
vpl_gid_t       vpl_getgid(void);
int             vpl_getgroups(int, vpl_gid_t []);
long            vpl_gethostid(void);
char           *vpl_getlogin(void);
int             vpl_getlogin_r(char *, vpl_size_t);
int             vpl_getopt(int, char * const [], const char *);
vpl_pid_t       vpl_getpgid(vpl_pid_t);
vpl_pid_t       vpl_getpgrp(void);
vpl_pid_t       vpl_getpid(void);
vpl_pid_t       vpl_getppid(void);
vpl_pid_t       vpl_getsid(vpl_pid_t);
vpl_uid_t       vpl_getuid(void);
char           *vpl_getwd(char *);
int             vpl_isatty(int);
int             vpl_lchown(const char *, vpl_uid_t, vpl_gid_t);
int             vpl_link(const char *, const char *);
int             vpl_lockf(int, int, vpl_off_t);
vpl_off_t       vpl_lseek(int, vpl_off_t, int);
int             vpl_nice(int);
long int        vpl_pathconf(const char *, int);
int             vpl_pause(void);
int             vpl_pipe(int [2]);
vpl_ssize_t     vpl_pread(int, void *, vpl_size_t, vpl_off_t);
int             vpl_pthread_atfork(void (*)(void), void (*)(void), void(*)(void));
vpl_ssize_t     vpl_pwrite(int, const void *, vpl_size_t, vpl_off_t);
vpl_ssize_t     vpl_read(int, void *, vpl_size_t);
int             vpl_readlink(const char *, char *, vpl_size_t);
int             vpl_rmdir(const char *);
void           *vpl_sbrk(vpl_intptr_t);
int             vpl_setgid(vpl_gid_t);
int             vpl_setpgid(vpl_pid_t, vpl_pid_t);
vpl_pid_t       vpl_setpgrp(void);
int             vpl_setregid(vpl_gid_t, vpl_gid_t);
int             vpl_setreuid(vpl_uid_t, vpl_uid_t);
vpl_pid_t       vpl_setsid(void);
int             vpl_setuid(vpl_uid_t);
unsigned int    vpl_sleep(unsigned int);
void            vpl_swab(const void *, void *, vpl_ssize_t);
int             vpl_symlink(const char *, const char *);
void            vpl_sync(void);
long int        vpl_sysconf(int);
vpl_pid_t       vpl_tcgetpgrp(int);
int             vpl_tcsetpgrp(int, vpl_pid_t);
int             vpl_truncate(const char *, vpl_off_t);
char           *vpl_ttyname(int);
int             vpl_ttyname_r(int, char *, vpl_size_t);
vpl_useconds_t  vpl_ualarm(vpl_useconds_t, vpl_useconds_t);
int             vpl_unlink(const char *);
int             vpl_usleep(vpl_useconds_t);
vpl_pid_t       vpl_vfork(void);
vpl_ssize_t     vpl_write(int, const void *, vpl_size_t);

#endif // vpl_unistd_h_
