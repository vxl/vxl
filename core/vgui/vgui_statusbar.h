// This is oxl/vgui/vgui_statusbar.h
#ifndef vgui_statusbar_h_
#define vgui_statusbar_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   21 Nov 99
// \brief  vgui_statusbar is a class that Phil hasnt documented properly. FIXME
//
//  Contains classes: vgui_statusbar


class vgui_statusbar
{
 public:

  vgui_statusbar();
  virtual ~vgui_statusbar();

  virtual int write(const char* text, int n) = 0;
  virtual int write(const char* text) = 0;
};

#endif // vgui_statusbar_h_
