#ifndef vcl_unistd_h_
#define vcl_unistd_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

// OK, so this is not really acceptable as <unistd.h> is not
// part of the ISO C++ standard. What vcl_unistd.h is meant
// to do is to include <unistd.h> properly on systems that have 
// it. Eg. :
// 1. On some UNIX systems, usleep() is not declared in
//    unistd although the man pages say it is. So we declare it
//    but don't provide a new definition.
// 2. Some older libcs dont't have all the posix functions, such
//    as pread() and pwrite(). So we provide alternative definitions
//    for those platforms where we can't get a system version.
// 3. On windows, unlink() is called _unlink(), buth with
//    vcl_unistd.h it's called vcl_unlink() on both UNIX and 
//    windows.

// This list is from 
// http://www.opengroup.org/onlinepubs/007908799/xsh/unistd.h.html
// and includes also the functions declared in posix sys/stat.h

#include <vcl/vcl_compiler.h>
#include <vcl/vcl_sys/types.h>
//#include <vxl_unistd_config.h>

typedef size_t	    vcl_size_t;
// fixme this needs configging
#ifndef VCL_WIN32
typedef ssize_t	    vcl_ssize_t;
typedef uid_t	    vcl_uid_t;
typedef gid_t	    vcl_gid_t;
typedef pid_t	    vcl_pid_t; 
#else
typedef size_t	    vcl_ssize_t;
typedef int	    vcl_uid_t;
typedef int	    vcl_gid_t;
typedef int	    vcl_pid_t; 
#endif
typedef off_t	    vcl_off_t;
#if VCL_UNISTD_HAS_USECONDS_T
typedef useconds_t  vcl_useconds_t;
#else
typedef unsigned    vcl_useconds_t;
#endif
#if VCL_UNISTD_HAS_INTPTR_T
typedef intptr_t    vcl_intptr_t;
#else
typedef int         vcl_intptr_t;
#endif

// sys/stat.h:
#include <sys/stat.h>
#ifdef VCL_WIN32
typedef struct _stat vcl_stat_t;
#else
typedef struct stat vcl_stat_t;
#endif
#ifndef VCL_WIN32
typedef mode_t vcl_mode_t;
#else
typedef int vcl_mode_t;
#endif
typedef dev_t vcl_dev_t;

int    		vcl_chmod(const char *, vcl_mode_t);
int    		vcl_fchmod(int, vcl_mode_t);
int    		vcl_fstat(int, vcl_stat_t *);
int    		vcl_lstat(const char *, vcl_stat_t *);
int    		vcl_mkdir(const char *, vcl_mode_t);
int    		vcl_mkfifo(const char *, vcl_mode_t);
int    		vcl_mknod(const char *, vcl_mode_t, vcl_dev_t);
int    		vcl_stat(const char *, vcl_stat_t *);
vcl_mode_t	vcl_umask(vcl_mode_t);

// unistd.h:
int             vcl_access(const char *, int);
unsigned int    vcl_alarm(unsigned int);
int             vcl_brk(void *);
int             vcl_chdir(const char *);
int             vcl_chown(const char *, vcl_uid_t, vcl_gid_t);
int             vcl_close(int);
vcl_size_t      vcl_confstr(int, char *, vcl_size_t);
char           *vcl_crypt(const char *, const char *);
char           *vcl_ctermid(char *);
int             vcl_dup(int);
int             vcl_dup2(int, int);
void            vcl_encrypt(char[64], int);
int             vcl_execl(const char *, const char *, ...);
int             vcl_execle(const char *, const char *, ...);
int             vcl_execlp(const char *, const char *, ...);
int             vcl_execv(const char *, char *const []);
int             vcl_execve(const char *, char *const [], char *const []);
int             vcl_execvp(const char *, char *const []);
int             vcl_fchown(int, vcl_uid_t, vcl_gid_t);
int             vcl_fchdir(int);
int             vcl_fdatasync(int);
vcl_pid_t       vcl_fork(void);
long int        vcl_fpathconf(int, int);
int             vcl_fsync(int);
int             vcl_ftruncate(int, vcl_off_t);
char           *vcl_getcwd(char *, vcl_size_t);
vcl_gid_t       vcl_getegid(void);
vcl_uid_t       vcl_geteuid(void);
vcl_gid_t       vcl_getgid(void);
int             vcl_getgroups(int, vcl_gid_t []);
long            vcl_gethostid(void);
char           *vcl_getlogin(void);
int             vcl_getlogin_r(char *, vcl_size_t);
int             vcl_getopt(int, char * const [], const char *);
vcl_pid_t       vcl_getpgid(vcl_pid_t);
vcl_pid_t       vcl_getpgrp(void);
vcl_pid_t       vcl_getpid(void);
vcl_pid_t       vcl_getppid(void);
vcl_pid_t       vcl_getsid(vcl_pid_t);
vcl_uid_t       vcl_getuid(void);
char           *vcl_getwd(char *);
int             vcl_isatty(int);
int             vcl_lchown(const char *, vcl_uid_t, vcl_gid_t);
int             vcl_link(const char *, const char *);
int             vcl_lockf(int, int, vcl_off_t);
vcl_off_t       vcl_lseek(int, vcl_off_t, int);
int             vcl_nice(int);
long int        vcl_pathconf(const char *, int);
int             vcl_pause(void);
int             vcl_pipe(int [2]);
vcl_ssize_t     vcl_pread(int, void *, vcl_size_t, vcl_off_t);
int             vcl_pthread_atfork(void (*)(void), void (*)(void), void(*)(void));
vcl_ssize_t     vcl_pwrite(int, const void *, vcl_size_t, vcl_off_t);
vcl_ssize_t     vcl_read(int, void *, vcl_size_t);
int             vcl_readlink(const char *, char *, vcl_size_t);
int             vcl_rmdir(const char *);
void           *vcl_sbrk(vcl_intptr_t);
int             vcl_setgid(vcl_gid_t);
int             vcl_setpgid(vcl_pid_t, vcl_pid_t);
vcl_pid_t       vcl_setpgrp(void);
int             vcl_setregid(vcl_gid_t, vcl_gid_t);
int             vcl_setreuid(vcl_uid_t, vcl_uid_t);
vcl_pid_t       vcl_setsid(void);
int             vcl_setuid(vcl_uid_t);
unsigned int    vcl_sleep(unsigned int);
void            vcl_swab(const void *, void *, vcl_ssize_t);
int             vcl_symlink(const char *, const char *);
void            vcl_sync(void);
long int        vcl_sysconf(int);
vcl_pid_t       vcl_vcl_tcgetpgrp(int);
int             vcl_vcl_tcsetpgrp(int, vcl_pid_t);
int             vcl_vcl_truncate(const char *, vcl_off_t);
char           *vcl_vcl_ttyname(int);
int             vcl_vcl_ttyname_r(int, char *, vcl_size_t);
vcl_useconds_t  vcl_ualarm(vcl_useconds_t, vcl_useconds_t);
int             vcl_unlink(const char *);
int             vcl_usleep(vcl_useconds_t);
vcl_pid_t       vcl_vfork(void);
vcl_ssize_t     vcl_write(int, const void *, vcl_size_t);

#endif
