// This is core/vgui/vgui_popup_params.h
#ifndef vgui_popup_params_h_
#define vgui_popup_params_h_
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   21 Jan 00
// \brief contains class vgui_popup_params
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

class vgui_popup_params
{
 public:
  vgui_popup_params() : x(-1), y(-1), recurse(true), nested(false), defaults(false) {}
  // viewport coordinates of mouse when used to activate popup menu
  int x,y;
  bool recurse;
  bool nested;
  bool defaults;
};

#endif // vgui_popup_params_h_
