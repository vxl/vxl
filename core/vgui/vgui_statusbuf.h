// This is core/vgui/vgui_statusbuf.h
#ifndef vgui_statusbuf_h_
#define vgui_statusbuf_h_
//:
// \file
// \brief  Statusbar buffer?
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   21 Nov 99
//
//  Contains class vgui_statusbuf
//
// \verbatim
//  Modifications
//   21-Nov-1999 P.Pritchett - Initial version.
// \endverbatim

#include <streambuf>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class vgui_statusbar;

class vgui_statusbuf : public std::streambuf
{
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
