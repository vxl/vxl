#ifndef vplayer_h_
#define vplayer_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief  Video player; Plays sequences of images with underlying geometry
// \author
//   Marko Bacic, Oxford RRG
// \date
//   Marko Bacic 07-SEP-2000    Initial version.
//--------------------------------------------------------------------------------

#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_rubberbander_sptr.h>
vgui_image_tableau_sptr get_image_tableau_at(unsigned col, unsigned row);
vgui_easy2D_tableau_sptr get_easy2D_at(unsigned col, unsigned row);
vgui_rubberbander_sptr get_rubberbander_at(unsigned col, unsigned row);
void get_current(unsigned* col, unsigned* row);
// No need to export xcv_tab:
//class xcv_tableau;
//typedef vgui_tableau_sptr_t<xcv_tableau> xcv_tableau_sptr;
//extern xcv_tableau_sptr xcv_tab;
#endif // vplayer_h_
