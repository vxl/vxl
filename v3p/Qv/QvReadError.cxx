#include "QvReadError.h"
#include "QvString.h"
#include "QvInput.h"
#include <vcl_iostream.h>
#include <vcl_cstdarg.h>

QvReadErrorCallback QvReadError::callback_ = 0;  // mpichler, 19950713

void
QvReadError::post(const QvInput *in, const char *formatString ...)
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

    QvString locstr;
    in->getLocationString(locstr);

  if (callback_)
    (*callback_) (buf, locstr.getString());
  else
  {
    vcl_cerr << "VRML read error: " << buf << '\n' << locstr.getString() << '\n';
  }
}
