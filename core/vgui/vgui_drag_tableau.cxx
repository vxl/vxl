// This is ./oxl/vgui/vgui_drag_tableau.cxx

//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   11 Jan 00
// \brief  See vgui_drag_tableau.h for a description of this file.
//
// \verbatim
//  Modifications:
//    11-JAN-2000 A.Fitzgibbon - Initial version
// \endverbatim

#ifdef __GNUC__
#pragma implementation
#endif


#include "vgui_drag_tableau.h"

//: Default ctor
vgui_drag_tableau::vgui_drag_tableau()
{
}

//: Destructor
vgui_drag_tableau::~vgui_drag_tableau()
{
}

bool vgui_drag_tableau::handle(const vgui_event& e)
{
  if (vgui_drag_mixin::handle(e))
    return true;

  return vgui_tableau::handle(e);
}
