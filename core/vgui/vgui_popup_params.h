// This is oxl/vgui/vgui_popup_params.h
#ifndef vgui_popup_params_h_
#define vgui_popup_params_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   21 Jan 00
// \brief 
// 
//  Contains classes:  vgui_popup_params
//

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
