// This is core/vidl/gui/vidl_capture_tableau_sptr.h
#ifndef vidl_capture_tableau_sptr_h_
#define vidl_capture_tableau_sptr_h_
//:
// \file
// \author  Matt Leotta
// \brief   Smart-pointer to a vidl_capture_tableau tableau.


#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>

class vidl_capture_tableau;
typedef vgui_tableau_sptr_t<vidl_capture_tableau>  vidl_capture_tableau_sptr;

#endif // vidl_capture_tableau_sptr_h_
