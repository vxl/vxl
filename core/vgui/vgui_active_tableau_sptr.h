// This is core/vgui/vgui_active_tableau_sptr.h
#ifndef vgui_active_tableau_sptr_h_
#define vgui_active_tableau_sptr_h_
//:
// \file
// \brief  Smart-pointer to a vgui_active_tableau tableau.

#include <vgui/vgui_tableau_sptr.h>

class vgui_active_tableau;

//: Smart-pointer to a vgui_active_tableau tableau.
typedef vgui_tableau_sptr_t<vgui_active_tableau> vgui_active_tableau_sptr;

#endif // vgui_active_tableau_sptr_h_
