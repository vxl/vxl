#ifdef SUN4
# include <varargs.h>
#else  /* makedepend does not understand || */
# ifdef SUN4_GNU
#  include <varargs.h>
# else
#  include <vcl_cstdarg.h>
# endif
#endif

#include "QvString.h"
#include "QvDebugError.h"
#include <vcl_iostream.h>

QvDebugErrorCallback QvDebugError::callback_ = 0;  // mpichler, 19950713

void
QvDebugError::post(const char *methodName, const char *formatString ...)
{
    char    buf[10000];
    va_list ap;

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
    vcl_cerr << "VRML error in " << methodName << ": " << buf << '\n';
}
