// This is core/vgui/vgui_displaylist2D_tableau_sptr.h
#ifndef vgui_displaylist2D_tableau_sptr_h_
#define vgui_displaylist2D_tableau_sptr_h_
//:
// \file
// \author awf@robots.ox.ac.uk
// \date   10 Sep 00
// \brief  Smart-pointer to a vgui_displaylist2D_tableau tableau.

#include <vgui/vgui_displaybase_tableau_sptr.h>

class vgui_displaylist2D_tableau;
typedef vgui_tableau_sptr_t<vgui_displaylist2D_tableau,vgui_displaybase_tableau_sptr> vgui_displaylist2D_tableau_sptr;

#endif // vgui_displaylist2D_tableau_sptr_h_
