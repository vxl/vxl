//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// .NAME vgui_statusbuf
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 21 Nov 99
//
//-----------------------------------------------------------------------------

#include "vgui_statusbuf.h"
#include <vgui/vgui_statusbar.h>

int vgui_statusbuf::sync()
{
  int n = pptr () - pbase ();
  return (n && status->write(pbase(), n) != n) ? EOF : 0;
}

int vgui_statusbuf::overflow(int ch)
{ 
  int n = pptr() - pbase();

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
 
//int vgui_statusbuf::xsputn(char* text, int n)
// streamsize vgui_statusbuf::xsputn(char* text, streamsize n)
// { 
//   cerr << "xsputn " << text << " " << n << endl;
//   return sync() == EOF ? 0 : status->write(text, n);
// }

// streamsize vgui_statusbuf::xsputn(const char* text, streamsize n) {
//   cerr << "xsputn const " << text << " " << n << endl;
//   return sync() == EOF ? 0 : status->write(text, n);
// }
