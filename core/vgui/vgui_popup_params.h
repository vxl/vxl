// This is oxl/vgui/vgui_popup_params.h
#ifndef vgui_popup_params_h_
#define vgui_popup_params_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//
// .NAME vgui_popup_params - Undocumented class FIXME
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_popup_params.h
// .FILE vgui_popup_params.cxx
//
// .SECTION Description
//
// vgui_popup_params is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author
//              Philip C. Pritchett, 21 Jan 00
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//
//-----------------------------------------------------------------------------

class vgui_popup_params
{
 public:
  vgui_popup_params();

  // viewport coordinates of mouse when used to activate popup menu
  int x,y;
  bool recurse;
  bool nested;
  bool defaults;
};

#endif // vgui_popup_params_h_
