#ifndef vgui_statusbuf_h_
#define vgui_statusbuf_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
// \brief Undocumented class FIXME
//
// \author
//              Philip C. Pritchett, 21 Nov 99
//              Robotics Research Group, University of Oxford
//-----------------------------------------------------------------------------

#include <vcl_streambuf.h>
class vgui_statusbar;

class vgui_statusbuf : public vcl_streambuf {
public:
  vgui_statusbuf (vgui_statusbar* s) { status = s; }
  int sync ();
  int overflow (int ch);
  int underflow(){return 0;}
  //int xsputn( char* text, int n);
  //streamsize xsputn(char* text, streamsize n);
  //streamsize xsputn(const char* text, streamsize n);

private:
  vgui_statusbar* status;
};

#endif // vgui_statusbuf_h_
