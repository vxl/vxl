#include "QvString.h"
#include "QvDebugError.h"
#include <vcl_iostream.h>
#include <vcl_cstdarg.h>
#include <vcl_cstdio.h>

QvDebugErrorCallback QvDebugError::callback_ = 0;  // mpichler, 19950713

void
QvDebugError::post(const char *methodName, const char *formatString ...)
{
    char    buf[10000];
    vcl_va_list ap;

#if defined(SUN4) || defined(SUN4_GNU)
    va_start(ap);
#else
    va_start(ap, formatString);
#endif
    vcl_vsprintf(buf, formatString, ap);
    va_end(ap);

  if (callback_)
    (*callback_) (methodName, buf);
  else
    vcl_cerr << "VRML error in " << methodName << ": " << buf << '\n';
}
