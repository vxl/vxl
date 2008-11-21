// This is brl/bbas/vidl2/gui/vidl2_capture_tableau_sptr.h
#ifndef vidl2_capture_tableau_sptr_h_
#define vidl2_capture_tableau_sptr_h_
//:
// \file
// \author  Matt Leotta
// \brief   Smart-pointer to a vidl2_capture_tableau tableau.


#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>

class vidl2_capture_tableau;
typedef vgui_tableau_sptr_t<vidl2_capture_tableau>  vidl2_capture_tableau_sptr;

#endif // vidl2_capture_tableau_sptr_h_
