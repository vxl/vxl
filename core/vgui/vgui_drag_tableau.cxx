// This is oxl/vgui/vgui_drag_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
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

#include "vgui_drag_tableau.h"

//----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_drag_tableau_new.
vgui_drag_tableau::vgui_drag_tableau()
{
}

//----------------------------------------------------------------------------
//: Destructor - called by vgui_drag_tableau_sptr.
vgui_drag_tableau::~vgui_drag_tableau()
{
}

//----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_drag_tableau_new.
bool vgui_drag_tableau::handle(const vgui_event& e)
{
  if (vgui_drag_mixin::handle(e))
    return true;

  return vgui_tableau::handle(e);
}
