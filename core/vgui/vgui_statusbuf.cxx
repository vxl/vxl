// This is core/vgui/vgui_statusbuf.cxx
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   21 Nov 1999
// \brief  See vgui_statusbuf.h for a description of this file.

#include "vgui_statusbuf.h"
#include <vgui/vgui_statusbar.h>

int vgui_statusbuf::sync()
{
  long n = pptr () - pbase ();
  return (n && status->write(pbase(), n) != n) ? -1 : 0;
}

int vgui_statusbuf::overflow(int ch)
{
  long n = pptr() - pbase();

  if (n && sync())
    return -1;
  if (ch != -1) {
    char cbuf[1];
    cbuf[0] = (char)ch;
    if (status->write(cbuf, 1) != 1)
      return -1;
  }
  pbump (-n);  // Reset pptr().
  return 0;
}
