#include <vpl/vpl_unistd.h>

#include <vcl_cstdio.h>
#include <vcl_cstdarg.h>
#include <vcl_cstddef.h>
#include <vcl_cstdlib.h>
#include <vcl_cassert.h>
#include <vcl_vector.h>

// Include system headers for UNIX-like operating system :
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

// Fix broken unistd.h headers.

// #if defined(__linux__)
// # if _POSIX_VERSION <= 199309L
// // linuces without ualarm() and lchown(). eg antonio@robots
// static vpl_useconds_t ualarm(...) { assert(false); return 0; }
// static int lchown(...) { assert(false); return 0; }
// # elif _POSIX_VERSION >= 199506L
// // linuces with ualarm() and lchown(). eg viola@robots
// # else
// // fsm@robots: did you end up here? consult /usr/include/unistd.h to
// // find out (a) what the value of _POSIX_VERSION is and (b) whether
// // or not it declares ualarm() and lchown(). then fix the above logic.
// #  error "dunno about the unistd.h on this linux"
// # endif
// #endif

#if defined(__GNUC__) && (__GNUC_MINOR__ >= 97)
# define VPL_THROW(args) throw args
#else
# define VPL_THROW(args) /* */
#endif
// /data/az5/fsm/GCC3.0/target/vxl/vpl/os_unix/vpl_unistd.cxx:135: declaration of
//    'int brk(void*)' throws different exceptions
// /usr/include/unistd.h:860: than previous declaration 'int brk(void*) throw ()'


// Some functions are provided in the libraries, described in
// the man pages but not declared in the header. In that case,
// just copy the declaration from the man page into this file
// and hope the prototype is not too different from what the
// function expects.
extern "C" {
#if defined(__linux__) // viola@robots
  pid_t getpgid(pid_t pid) VPL_THROW(());
  pid_t getsid(pid_t pid) VPL_THROW(());
#endif
#if defined(__sparc__) || defined(VCL_SUNPRO_CC) // kate@robots, ariel@robots
  long gethostid(void);
  int setregid(gid_t rgid, gid_t egid);
  int setreuid(uid_t ruid, uid_t euid);
  unsigned int ualarm(unsigned int usecs, unsigned int interval);
# if VXL_UNISTD_USLEEP_IS_VOID
  void usleep(unsigned int useconds);
# else
  int usleep(unsigned int useconds);
# endif // # if VXL_UNISTD_USLEEP_IS_VOID
  char *getwd(char *pathname);
#endif
};

// seems not to be part of POSIX, so we don't declare it in vpl_unistd.h
// (dc) use of VXL_UNISTD_HAS_TELL ignored because libc5 system has no tell() at all

/*static*/ long vpl_tell(int fd) {
//#if VXL_UNISTD_HAS_TELL
//  return ::tell(fd);
//#else
  return vpl_lseek(fd, 0, SEEK_CUR);
//#endif
}

//--------------------------------------------------------------------------------

// native SGI CC does not allow void functions to return values or
// non-void functions not to return values. so UNIMP must be given
// a dummy return value. fsm
#define UNIMP(ret, f, arglist, args, retvalue) ret vpl_ ## f arglist \
{ fprintf(stderr, __FILE__ ": unimplemented function [" #f "]\n"); return retvalue; }

// native SGI CC does not allow "return sync();" in a function
// returning void (even though sync() returns void). fsm
#define PASSTHRU(ret, f, arglist, args) ret vpl_ ## f arglist { return f args ; }
#define PASSTHRUvoid(f, arglist, args) void vpl_ ## f arglist { f args ; }

#if 0 // this is emacs lisp

(defun f ()
  (interactive)
  (re-search-forward "PASSTHRU.*(")
  (let ((a 0)
        (done nil))
    (while (not done)
      (next-line 1)
      (cond ((looking-at "[a-zA-Z_]")
             (save-excursion
               (if (looking-at ".*,$")
                   (progn
                     (end-of-line)
                     (forward-char -1)
                     )
                 (end-of-line))
               (insert (format "  a%d" a)))
             (setq a (1+ a)))
            ((looking-at ")")
             (forward-char 1)
             (insert ", (")
             (let ((b 0))
               (while (< b a)
                 (if (> b 0)
                     (insert ", "))
                 (insert (format "a%d" b))
                 (setq b (1+ b))))
             (insert ")")
             (setq done t)
             )
            (t (error "zoiks")))
      )))
  ;; at end
#endif

PASSTHRU(int, chmod, (const char *  a0,vpl_mode_t  a1), (a0, a1))
PASSTHRU(int, fchmod, ( int  a0, vpl_mode_t  a1 ), (a0, a1))
PASSTHRU(int, fstat, (int  a0,vpl_stat_t *  a1), (a0, a1))
PASSTHRU(int, lstat, (const char *  a0,vpl_stat_t *  a1), (a0, a1))
PASSTHRU(int, mkdir, (const char * s, vpl_mode_t mode), (s, mode))
PASSTHRU(int, mkfifo, ( const char *  a0, vpl_mode_t  a1 ), (a0, a1))
    extern "C" int mknod (const char*, vpl_mode_t, vpl_dev_t) VPL_THROW(());
PASSTHRU(int, mknod, (const char *  a0,vpl_mode_t  a1,vpl_dev_t  a2), (a0, a1, a2))
PASSTHRU(int, stat, (const char *  a0,vpl_stat_t *  a1), (a0, a1))
PASSTHRU(mode_t, umask, (vpl_mode_t  a0), (a0))

PASSTHRU(int, access, (const char *  a0,int  a1), (a0, a1));
PASSTHRU(unsigned int, alarm, (unsigned int  a0), (a0));
#if defined(__FreeBSD__)
extern "C" char* brk( const char *) VPL_THROW(());
PASSTHRU(char *, brk, (const char* a0), (a0));
#elif defined(VCL_GCC_30)
extern "C" int brk (char*) VPL_THROW(());
PASSTHRU(int, brk, (void *  a0), ((char*)a0));
#else
extern "C" int brk (void*) VPL_THROW(());
PASSTHRU(int, brk, (void *  a0), (a0));
#endif
PASSTHRU(int, chdir, (const char * s), (s));
PASSTHRU(int, chown, (const char *  a0,vpl_uid_t  a1,vpl_gid_t  a2), (a0, a1, a2));
PASSTHRU(int, close, (int  a0), (a0));
UNIMP(vcl_size_t, confstr, (int /*a0*/,char* /*a1*/,vcl_size_t /*a2*/), (a0, a1, a2), 0);
UNIMP(char           *, crypt, (const char* /*a0*/,const char* /*a1*/), (a0, a1), 0);
PASSTHRU(char           *, ctermid, (char *  a0), (a0));
PASSTHRU(int, dup, (int  a0), (a0));
PASSTHRU(int, dup2, (int  a0,int  a1), (a0, a1));
UNIMP(void, encrypt, (char /*a0*/[64],int /*a1*/), (a0, a1), /* void */);
UNIMP(int, execl, (const char *, const char *,...), (), 0);
UNIMP(int, execle, (const char *,const char *,...), (), 0);
#if 0
UNIMP(int, execlp, (const char *,const char *,...), (), 0);
#else
// fsm@robots.
// Perhaps a #define in vpl_unistd.h would be better?
//int execlp(const char *file, const char *arg0, ...,
//           const char *argn, char * /*NULL*/);
int vpl_execlp(char const *file, char const *arg0, ...) {
  vcl_vector<char *> args;
  args.push_back((char*)arg0);
  va_list ap;
  va_start(ap, arg0);
  while (true) {
    char *arg = va_arg(ap, char *);
    args.push_back(arg);
    if (!arg)
      break;
  }
  va_end(ap);
  return ::execvp(file, &args[0]);
}
#endif
PASSTHRU(int, execv, (const char *  a0,char *const a1[]), (a0, a1));
PASSTHRU(int, execve, (const char *  a0,char *const a1[],char *const a2[]), (a0, a1, a2));
PASSTHRU(int, execvp, (const char *  a0,char *const a1[]), (a0, a1));
PASSTHRU(int, fchown, (int  a0,vpl_uid_t  a1,vpl_gid_t  a2), (a0, a1, a2));
extern "C" int fchdir (int) VPL_THROW(());
PASSTHRU(int, fchdir, (int  a0), (a0));
// PASSTHRU(int, fdatasync, (  int  a0  ), (a0));
PASSTHRU(vpl_pid_t, fork, (), ());
PASSTHRU(long int, fpathconf, (  int  a0,  int  a1  ), (a0, a1));
PASSTHRU(int, fsync, (int  a0), (a0));
PASSTHRU(int, ftruncate, (  int  a0,  vpl_off_t  a1  ), (a0, a1));
PASSTHRU(char           *, getcwd, (char *  a0,vcl_size_t  a1), (a0, a1));
PASSTHRU(vpl_gid_t, getegid, (void), ());
PASSTHRU(vpl_uid_t, geteuid, (void), ());
PASSTHRU(vpl_gid_t, getgid, (void), ());
PASSTHRU(int, getgroups, (  int  a0,  vpl_gid_t a1[]  ), (a0, a1));
//PASSTHRU(long, gethostid, (void), ());
PASSTHRU(char *, getlogin, (void), ());
// PASSTHRU(int, getlogin_r, (   char *  a0,   vcl_size_t  a1   ), (a0, a1));
// PASSTHRU(int, getopt, (int  a0,char * const a1[],const char *  a2), (a0, a1, a2));
PASSTHRU(vpl_pid_t, getpgid, (vpl_pid_t  a0), (a0));
PASSTHRU(vpl_pid_t, getpgrp, (void), ());
PASSTHRU(vpl_pid_t, getpid, (void), ());
PASSTHRU(vpl_pid_t, getppid, (void), ());
PASSTHRU(vpl_pid_t, getsid, (vpl_pid_t  a0), (a0));
PASSTHRU(vpl_uid_t, getuid, (void), ());
//PASSTHRU(char           *, getwd, (char *  a0), (a0));
PASSTHRU(int, isatty, (int  a0), (a0));

#if VXL_UNISTD_HAS_LCHOWN
PASSTHRU(int, lchown, (const char *  a0,vpl_uid_t  a1,vpl_gid_t  a2), (a0, a1, a2));
#else
UNIMP(int, lchown, (const char* /*a0*/,vpl_uid_t /*a1*/,vpl_gid_t /*a2*/), (a0, a1, a2), 0);
#endif

PASSTHRU(int, link, (const char *  a0,const char *  a1), (a0, a1));
PASSTHRU(int, lockf, (int  a0,int  a1,vpl_off_t  a2), (a0, a1, a2));
PASSTHRU(vpl_off_t, lseek, (int  a0,vpl_off_t  a1,int  a2), (a0, a1, a2));
PASSTHRU(int, nice, (int  a0), (a0));
PASSTHRU(long int, pathconf, ( const char *  a0, int  a1 ), (a0, a1));
PASSTHRU(int, pause, (void), ());
PASSTHRU(int, pipe, (int a0[2]), (a0));

// pread() and pwrite() sometimes don't exist. in those cases so we fake
// them using tell and seek. it won't work for multithreaded programs,
// where we should lock the fd.
#if VXL_UNISTD_HAS_PREAD
extern "C" ssize_t pread (int, void*, vcl_size_t, vpl_off_t) VPL_THROW(());
PASSTHRU(vpl_ssize_t, pread,(int a0,void* a1,vcl_size_t a2,vpl_off_t a3), (a0, a1, a2, a3));
#else
vpl_ssize_t vpl_pread(int fd, void       *buf, vcl_size_t nbyte, vpl_off_t offset) {
  // should lock fd here (if multithreaded).
  vpl_off_t orig = vpl_tell(fd);               // get current file position
  vpl_lseek(fd, offset, SEEK_SET);             // seek to requested position
  vpl_ssize_t rv = vpl_read(fd, buf, nbyte);   // write
  vpl_lseek(fd, orig, SEEK_SET);               // seek back to original position
  // should unlock fd here (if multithreaded).
  return rv;
}
#endif
#if VXL_UNISTD_HAS_PWRITE
extern "C" ssize_t pwrite (int, void const*, vcl_size_t, vpl_off_t) VPL_THROW(());
PASSTHRU(vpl_ssize_t, pwrite, (int  a0,void const * a1,vcl_size_t a2,vpl_off_t a3), (a0, a1, a2, a3));
#else
vpl_ssize_t vpl_pwrite(int fd, void const *buf, vcl_size_t nbyte, vpl_off_t offset) {
  // should lock fd here (if multithreaded).
  vpl_off_t orig = vpl_tell(fd);               // get current file position
  vpl_lseek(fd, offset, SEEK_SET);             // seek to requested position
  vpl_ssize_t rv = vpl_write(fd, buf, nbyte);  // write
  vpl_lseek(fd, orig, SEEK_SET);               // seek back to original position
  // should unlock fd here (if multithreaded).
  return rv;
}
#endif

// PASSTHRU(int, pthread_atfork, (void (*a0)(void), void (*a1)(void), void (*a2)(void)), (a0, a1, a2));
PASSTHRU(vpl_ssize_t, read, (int  a0,void *  a1,vcl_size_t  a2), (a0, a1, a2));
PASSTHRU(int, readlink, ( const char *  a0, char *  a1, vcl_size_t  a2 ), (a0, a1, a2));
PASSTHRU(int, rmdir, (const char * s), (s));
PASSTHRU(void           *, sbrk, (vpl_intptr_t  a0), (a0));
PASSTHRU(int, setgid, (vpl_gid_t  a0), (a0));
PASSTHRU(int, setpgid, (vpl_pid_t  a0,vpl_pid_t  a1), (a0, a1));
#if defined(__FreeBSD__) || defined(VCL_GCC_30)
PASSTHRU(vpl_pid_t, setpgrp, (vpl_pid_t a0, vpl_pid_t a1), (a0, a1));
#else
PASSTHRU(vpl_pid_t, setpgrp, (void), ());
#endif
PASSTHRU(int, setregid, ( vpl_gid_t  a0, vpl_gid_t  a1 ), (a0, a1));
PASSTHRU(int, setreuid, ( vpl_uid_t  a0, vpl_uid_t  a1 ), (a0, a1));
PASSTHRU(vpl_pid_t, setsid, (void), ());
PASSTHRU(int, setuid, (vpl_uid_t  a0), (a0));
PASSTHRU(unsigned int, sleep, (unsigned int  a0), (a0));
// PASSTHRU(void, swab, (const void *  a0,void *  a1,vpl_ssize_t  a2), (a0, a1, a2));
PASSTHRU(int, symlink, (const char *  a0,const char *  a1), (a0, a1));
PASSTHRUvoid(sync, (void), ());
PASSTHRU(long int, sysconf, (int  a0), (a0));
PASSTHRU(vpl_pid_t, tcgetpgrp, (  int  a0  ), (a0));
PASSTHRU(int, tcsetpgrp, (  int  a0,  vpl_pid_t  a1  ), (a0, a1));
PASSTHRU(int, truncate, ( const char *  a0, vpl_off_t  a1 ), (a0, a1));
PASSTHRU(char           *, ttyname, (int  a0), (a0));
// PASSTHRU(int, ttyname_r, (  int  a0,  char *  a1,  vcl_size_t  a2  ), (a0, a1, a2));

#if VXL_UNISTD_HAS_UALARM
PASSTHRU(vpl_useconds_t, ualarm, (vpl_useconds_t  a0,vpl_useconds_t  a1), (a0, a1));
#else
UNIMP(vpl_useconds_t, ualarm, (vpl_useconds_t /*a0*/,vpl_useconds_t /*a1*/), (a0, a1), 0);
#endif

PASSTHRU(int, unlink, (const char * s), (s));

#if VXL_UNISTD_HAS_USLEEP
// libc has a usleep(), whether declared in unistd.h or not.
# if VXL_UNISTD_USLEEP_IS_VOID
// if usleep() returns void, we have to fake the return value :
#if !defined(VCL_SUNPRO_CC_50)
// Error: Only one of a set of overloaded functions can be extern "C".
// Error: usleep(unsigned), returning void, was previously declared returning int.
// Error: Overloading ambiguity between "usleep(unsigned)" and "usleep(unsigned)".
extern "C" void usleep (vpl_useconds_t) VPL_THROW(());
//was: extern "C" void usleep (unsigned vpl_useconds_t);
#endif
int vpl_usleep(vpl_useconds_t s) { ::usleep(s); return 0; }
# else
// it's a kosher (posix, anyway) usleep(), so we just pass it through :
extern "C" int usleep (vpl_useconds_t);
PASSTHRU(int, usleep, (vpl_useconds_t s), (s));
# endif // usleep() is void
#else
// libc has no usleep(). too bad
UNIMP(int, usleep, (vpl_useconds_t /*s*/), (s), 0);
#endif

#if defined(__sgi) // IRIX has no BSD vfork(); its vfork() == fork() - PVr
vpl_pid_t vpl_vfork(void) { return fork(); }
#else
PASSTHRU(vpl_pid_t, vfork, (void), ());
#endif

PASSTHRU(vpl_ssize_t, write, (int  a0,const void *  a1,vcl_size_t  a2), (a0, a1, a2));
