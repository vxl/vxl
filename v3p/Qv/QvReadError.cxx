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
#include "QvInput.h"

// changed Gerbert Orasche 260695
#ifdef __PC__
#include "stdafx.h"
#endif

#include "QvReadError.h"

QvReadErrorCallback QvReadError::callback_ = 0;  // mpichler, 19950713

void
QvReadError::post(const QvInput *in, const char *formatString ...)
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

    QvString locstr;
    in->getLocationString(locstr);

  if (callback_)
    (*callback_) (buf, locstr.getString());
  else
  {
#ifdef __PC__
    TRACE("VRML read error: %s\n%s\n", buf, locstr.getString());
#else
    fprintf(stderr, "VRML read error: %s\n%s\n", buf, locstr.getString());
#endif
  }
}
