#ifndef segv_vtol2D_tableau_sptr_h_
#define segv_vtol2D_tableau_sptr_h_
//:
// \file
// \author J.L. Mundy
// \date   November 28, 2002
// \brief  Smart-pointer to a segv_vtol2D_tableau tableau.

#include <vgui/vgui_tableau_sptr.h>

class segv_vtol2D_tableau;
typedef vgui_tableau_sptr_t<segv_vtol2D_tableau, vgui_tableau_sptr> segv_vtol2D_tableau_sptr;

#endif // segv_vtol2D_tableau_sptr_h_
