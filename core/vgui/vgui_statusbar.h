// This is oxl/vgui/vgui_statusbar.h
#ifndef vgui_statusbar_h_
#define vgui_statusbar_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//
// .NAME vgui_statusbar - Undocumented class FIXME
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_statusbar.h
// .FILE vgui_statusbar.cxx
//
// .SECTION Description
//
// vgui_statusbar is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author
//              Philip C. Pritchett, 21 Nov 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//
//-----------------------------------------------------------------------------

class vgui_statusbar
{
 public:

  vgui_statusbar();
  virtual ~vgui_statusbar();

  virtual int write(const char* text, int n) = 0;
  virtual int write(const char* text) = 0;
};

#endif // vgui_statusbar_h_
