// This is oxl/vgui/vgui_statusbuf.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   21 Nov 99
// \brief  See vgui_statusbuf.h for a description of this file.

#include "vgui_statusbuf.h"
#include <vgui/vgui_statusbar.h>

int vgui_statusbuf::sync()
{
  long n = pptr () - pbase ();
  return (n && status->write(pbase(), n) != n) ? EOF : 0;
}

int vgui_statusbuf::overflow(int ch)
{ 
  long n = pptr() - pbase();

  if (n && sync())
    return EOF;
  if (ch != EOF) {
    char cbuf[1];
    cbuf[0] = ch;
    if (status->write(cbuf, 1) != 1)
      return EOF;
  }
  pbump (-n);  // Reset pptr().
  return 0; 
}
