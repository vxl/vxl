// This is oxl/vgui/impl/Fl/vgui_Fl_statusbar.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 19 Nov 99
//
//-----------------------------------------------------------------------------

#include "vgui_Fl_statusbar.h"

vgui_Fl_statusbar::vgui_Fl_statusbar()
  : Fl_Output(0,0,0,0,0)
  , statusbuf(new vgui_statusbuf(this))
  , out(statusbuf)
{
}

vgui_Fl_statusbar::vgui_Fl_statusbar(int x, int y, int w, int h, const char* l)
  : Fl_Output(x,y,w,h,l)
  , statusbuf(new vgui_statusbuf(this))
  , out(statusbuf)
{
}


vgui_Fl_statusbar::~vgui_Fl_statusbar()
{
}

int vgui_Fl_statusbar::write(const char* text, int n)
{
  if (n == 1) {
    if (text[0] == '\n') {
      value(linebuffer.c_str());

      linebuffer = "";
    }
    else
      linebuffer += text[0];
  }
  else {
    linebuffer.append(text, n);
    if (linebuffer.find('\n')) {
      value(linebuffer.c_str());

      linebuffer = "";
    }
  }
  return n;
}

int vgui_Fl_statusbar::write(const char* text)
{
  value(text);
  return 1;
}
