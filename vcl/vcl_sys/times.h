#ifndef vcl_sys_times_h_
#define vcl_sys_times_h_

#include <vcl_compiler.h>

#if defined(VCL_WIN32) && !defined(__CYGWIN__)
# include <sys/timeb.h>
#elif defined(__CYGWIN__)
# include <sys/times.h>
#elif defined(GNU_LIBSTDCXX_V3)
# define __restrict /* */
# include <sys/times.h>
# undef __restrict
#elif !defined(VCL_NO_SYS_TIME_H)
# include <sys/times.h>
#else
# include <vcl_sys/time.h>
struct tms {
    clock_t tms_utime;		/* User CPU time.  */
    clock_t tms_stime;		/* System CPU time.  */

    clock_t tms_cutime;		/* User CPU time of dead children.  */
    clock_t tms_cstime;		/* System CPU time of dead children.  */
};

extern clock_t times(struct tms *);
#endif

#endif // vcl_sys_times_h_
