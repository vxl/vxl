// This is ./oxl/vgui/vgui_statusbuf.h
#ifndef vgui_statusbuf_h_
#define vgui_statusbuf_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   21 Nov 99
// \brief  Statusbar buffer?
//
//  Contains classes: vgui_statusbuf
//        
// \verbatim
//  Modifications:
//    21-Nov-1999 P.Pritchett - Initial version.
// \endverbatim

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
