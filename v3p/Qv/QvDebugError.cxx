#ifdef SUN4
# include <varargs.h>
#else  /* makedepend does not understand || */
# ifdef SUN4_GNU
#  include <varargs.h>
# else
#  include <vcl/vcl_cstdarg.h>
# endif
#endif

#include "QvString.h"

// changed Gerbert Orasche 260695
#ifdef __PC__
#include "stdafx.h"
#endif

#include "QvDebugError.h"

QvDebugErrorCallback QvDebugError::callback_ = 0;  // mpichler, 19950713

void
QvDebugError::post(const char *methodName, const char *formatString ...)
{
    char	buf[10000];
    va_list	ap;

#if defined(SUN4) || defined(SUN4_GNU)
    va_start(ap);
#else
    va_start(ap, formatString);
#endif
    vsprintf(buf, formatString, ap);
    va_end(ap);

  if (callback_)
    (*callback_) (methodName, buf);
  else
    fprintf(stderr, "VRML error in %s: %s\n", methodName, buf);
}
