#include <vpl/vpl_unistd.h>

//#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <vcl_cstdio.h>
#include <vcl_cstdarg.h>
#include <vcl_cassert.h>
#include <vcl_vector.h>

// Fix broken unistd.h headers.

// seems not to be part of POSIX, so we don't export it from this file.
static long vpl_tell(int fd) {
  return vpl_lseek(fd, 0, SEEK_CUR);
}

//--------------------------------------------------------------------------------

// native SGI CC does not allow void functions to return values or
// non-void functions not to return values. so UNIMP must be given
// a dummy return value. fsm
#define UNIMPr(ret, f, arglist, args, retvalue) ret vpl_ ## f arglist \
{ fprintf(stderr, __FILE__ ": unimplemented function [" #f "]\n"); return retvalue; }

#define UNIMP(ret, f, arglist, args)  ret vpl_ ## f arglist \
{ fprintf(stderr, __FILE__ ": unimplemented function [" #f "]\n"); return -1; }


// win32 -- add leading underscores
#include <windows.h>
#include <direct.h>
#include <io.h>
#define PASSTHRU(ret, f, arglist, args) ret vpl_ ## f arglist { return _ ## f args ; }
#define PASSTHRUvoid(f, arglist, args) void vpl_ ## f arglist { _ ## f args ; }

/*

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


 */

PASSTHRU(int, chmod, (const char *  a0,vpl_mode_t  a1), (a0, a1))
UNIMP(int, fchmod, ( int  a0, vpl_mode_t  a1 ), (a0, a1))
PASSTHRU(int, fstat, (int  a0,vpl_stat_t *  a1), (a0, a1))
UNIMP(int, lstat, (const char *  a0,vpl_stat_t *  a1), (a0, a1));
int vpl_mkdir(const char * s, vpl_mode_t mode)
{
  _mkdir(s);
  return 0;
}
UNIMP(int, mkfifo, ( const char *  a0, vpl_mode_t  a1 ), (a0, a1))
UNIMP(int, mknod, (const char *  a0,vpl_mode_t  a1,vpl_dev_t  a2), (a0, a1, a2))
PASSTHRU(int, stat, (const char *  a0,vpl_stat_t *  a1), (a0, a1))
PASSTHRU(vpl_mode_t, umask, (vpl_mode_t  a0), (a0))

PASSTHRU(int, access, (const char *  a0,int  a1), (a0, a1));
UNIMP(unsigned int, alarm, (unsigned int  a0), (a0));
UNIMP(int, brk, (void *  a0), (a0));
PASSTHRU(int, chdir, (const char * s), (s));
UNIMP(int, chown, (const char *  a0,vpl_uid_t  a1,vpl_gid_t  a2), (a0, a1, a2));
PASSTHRU(int, close, (int  a0), (a0));
UNIMPr(vpl_size_t, confstr, (int  a0,char *  a1,vpl_size_t  a2), (a0, a1, a2), 0);
UNIMPr(char           *, crypt, (const char *  a0,const char *  a1), (a0, a1), 0);
UNIMPr(char           *, ctermid, (char *  a0), (a0), 0);
PASSTHRU(int, dup, (int  a0), (a0));
PASSTHRU(int, dup2, (int  a0,int  a1), (a0, a1));
UNIMPr(void, encrypt, (char a0[64],int  a1), (a0, a1), VCL_VOID_RETURN);

#include <process.h>
UNIMPr(int, execl, (const char *, const char *,...), (), 0);
UNIMPr(int, execle, (const char *,const char *,...), (), 0);
#if 0
UNIMPr(int, execlp, (const char *,const char *,...), (), 0);
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
  return ::execvp(file, args.begin());
}
#endif
PASSTHRU(int, execv, (const char *  a0,char *const a1[]), (a0, a1));
PASSTHRU(int, execve, (const char *  a0,char *const a1[],char *const a2[]), (a0, a1, a2));
PASSTHRU(int, execvp, (const char *  a0,char *const a1[]), (a0, a1));
UNIMP(int, fchown, (int  a0,vpl_uid_t  a1,vpl_gid_t  a2), (a0, a1, a2));
UNIMP(int, fchdir, (int  a0), (a0));
// PASSTHRU(int, fdatasync, (  int  a0  ), (a0));
UNIMP(vpl_pid_t, fork, (), ());
UNIMP(long int, fpathconf, (  int  a0,  int  a1  ), (a0, a1));
UNIMP(int, fsync, (int  a0), (a0));
UNIMP(int, ftruncate, (  int  a0,  vpl_off_t  a1  ), (a0, a1));
PASSTHRU(char           *, getcwd, (char *  a0,vpl_size_t  a1), (a0, a1));
//UNIMP(vpl_gid_t, getegid, (void), ());
//UNIMP(vpl_uid_t, geteuid, (void), ());
//UNIMP(vpl_gid_t, getgid, (void), ());
//UNIMP(int, getgroups, (  int  a0,  vpl_gid_t a1[]  ), (a0, a1));
//UNIMP(long, gethostid, (void), ());
//UNIMP(char *, getlogin, (void), ());
//// PASSTHRU(int, getlogin_r, (   char *  a0,   vpl_size_t  a1   ), (a0, a1));
//// PASSTHRU(int, getopt, (int  a0,char * const a1[],const char *  a2), (a0, a1, a2));
//PASSTHRU(vpl_pid_t, getpgid, (vpl_pid_t  a0), (a0));
//PASSTHRU(vpl_pid_t, getpgrp, (void), ());
//PASSTHRU(vpl_pid_t, getpid, (void), ());
//PASSTHRU(vpl_pid_t, getppid, (void), ());
//PASSTHRU(vpl_pid_t, getsid, (vpl_pid_t  a0), (a0));
//PASSTHRU(vpl_uid_t, getuid, (void), ());
//PASSTHRU(char           *, getwd, (char *  a0), (a0));
PASSTHRU(int, isatty, (int  a0), (a0));

UNIMPr(int, lchown, (const char *  a0,vpl_uid_t  a1,vpl_gid_t  a2), (a0, a1, a2), 0);
//PASSTHRU(int, link, (const char *  a0,const char *  a1), (a0, a1));
//PASSTHRU(int, lockf, (int  a0,int  a1,vpl_off_t  a2), (a0, a1, a2));
PASSTHRU(vpl_off_t, lseek, (int  a0,vpl_off_t  a1,int  a2), (a0, a1, a2));
//PASSTHRU(int, nice, (int  a0), (a0));
//PASSTHRU(long int, pathconf, ( const char *  a0, int  a1 ), (a0, a1));
//PASSTHRU(int, pause, (void), ());
// pipe is in io.h, just not sure what the params are..
UNIMP(int, pipe, (int a0[2]), (a0));

// pread() and pwrite() sometimes don't exist. in those cases so we fake
// them using tell and seek. it won't work for multithreaded programs,
// where we should lock the fd.
vpl_ssize_t vpl_pread(int fd, void       *buf, vpl_size_t nbyte, vpl_off_t offset) {
  // should lock fd here (if multithreaded).
  vpl_off_t orig = vpl_tell(fd);               // get current file position
  vpl_lseek(fd, offset, SEEK_SET);             // seek to requested position
  vpl_ssize_t rv = vpl_read(fd, buf, nbyte);   // write
  vpl_lseek(fd, orig, SEEK_SET);               // seek back to original position
  // should unlock fd here (if multithreaded).
  return rv;
}

vpl_ssize_t vpl_pwrite(int fd, void const *buf, vpl_size_t nbyte, vpl_off_t offset) {
  // should lock fd here (if multithreaded).
  vpl_off_t orig = vpl_tell(fd);               // get current file position
  vpl_lseek(fd, offset, SEEK_SET);             // seek to requested position
  vpl_ssize_t rv = vpl_write(fd, buf, nbyte);  // write
  vpl_lseek(fd, orig, SEEK_SET);               // seek back to original position
  // should unlock fd here (if multithreaded).
  return rv;
}

// PASSTHRU(int, pthread_atfork, (void (*a0)(void), void (*a1)(void), void (*a2)(void)), (a0, a1, a2));
PASSTHRU(vpl_ssize_t, read, (int  a0,void *  a1,vpl_size_t  a2), (a0, a1, a2));
UNIMP(int, readlink, ( const char *  a0, char *  a1, vpl_size_t  a2 ), (a0, a1, a2));
PASSTHRU(int, rmdir, (const char * s), (s));
UNIMPr(void           *, sbrk, (vpl_intptr_t  a0), (a0), 0);
UNIMP(int, setgid, (vpl_gid_t  a0), (a0));
UNIMP(int, setpgid, (vpl_pid_t  a0,vpl_pid_t  a1), (a0, a1));
UNIMP(vpl_pid_t, setpgrp, (void), ());
UNIMP(int, setregid, ( vpl_gid_t  a0, vpl_gid_t  a1 ), (a0, a1));
UNIMP(int, setreuid, ( vpl_uid_t  a0, vpl_uid_t  a1 ), (a0, a1));
UNIMP(vpl_pid_t, setsid, (void), ());
UNIMP(int, setuid, (vpl_uid_t  a0), (a0));
unsigned int vpl_sleep(unsigned int  a0) {
  Sleep(long(a0) * 1000);
  return  0;
}

// PASSTHRU(void, swab, (const void *  a0,void *  a1,vpl_ssize_t  a2), (a0, a1, a2));
UNIMP(int, symlink, (const char *  a0,const char *  a1), (a0, a1));
void vpl_sync() {}
UNIMP(long int, sysconf, (int  a0), (a0));
UNIMP(vpl_pid_t, tcgetpgrp, (  int  a0  ), (a0));
UNIMP(int, tcsetpgrp, (  int  a0,  vpl_pid_t  a1  ), (a0, a1));
UNIMP(int, truncate, ( const char *  a0, vpl_off_t  a1 ), (a0, a1));
UNIMPr(char           *, ttyname, (int  a0), (a0), 0);
// PASSTHRU(int, ttyname_r, (  int  a0,  char *  a1,  vpl_size_t  a2  ), (a0, a1, a2));

UNIMPr(vpl_useconds_t, ualarm, (vpl_useconds_t  a0,vpl_useconds_t  a1), (a0, a1), 0);

PASSTHRU(int, unlink, (const char * s), (s));

int vpl_usleep(vpl_useconds_t s) {
  Sleep(s / 1000);
  return 0;
}

UNIMP(vpl_pid_t, vfork, (void), ());

PASSTHRU(vpl_ssize_t, write, (int  a0,const void *  a1,vpl_size_t  a2), (a0, a1, a2));
