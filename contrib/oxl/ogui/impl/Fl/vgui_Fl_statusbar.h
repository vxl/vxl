// This is oxl/ogui/impl/Fl/vgui_Fl_statusbar.h
#ifndef vgui_Fl_statusbar_h_
#define vgui_Fl_statusbar_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   19 Nov 99
// \brief  The FLTK implementation of vgui_statusbar.
//
//  Contains classes:  vgui_Fl_statusbar
//
// \verbatim
//  Modifications:
//    17-Sep-2002 K.Y.McGaul - Changed to doxygen style comments.
// \endverbatim

#include <vcl_string.h>
#include <vcl_iosfwd.h>
#include <FL/Fl_Output.H>
#include <vgui/vgui_statusbuf.h>
#include <vgui/vgui_statusbar.h>

//: The FLTK implementation of vgui_statusbar.
class vgui_Fl_statusbar : public Fl_Output, public vgui_statusbar
{
 public:
  vgui_Fl_statusbar();
  vgui_Fl_statusbar(int x, int y, int w, int h, const char* l=0);
  ~vgui_Fl_statusbar();

  int write(const char* text, int n);
  int write(const char* text);

  vcl_string linebuffer;
  vgui_statusbuf* statusbuf;
  vcl_ostream out;
};

#endif // vgui_Fl_statusbar_h_
