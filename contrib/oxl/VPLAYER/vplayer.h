#ifndef vplayer_h_
#define vplayer_h_
//--------------------------------------------------------------------------------
// .NAME vplayer - Video player
// .SECTION Description:
//   Plays sequences of images with underlying geometry
// .SECTION Author
//   Marko Bacic, Oxford RRG
// .SECTION Modifications:
//   Marko Bacic 07-SEP-2000    Initial version.
//--------------------------------------------------------------------------------


#include <vgui/vgui_image_tableau_ref.h>
#include <vgui/vgui_easy2D_ref.h>
#include <vgui/vgui_rubberbander_ref.h>
vgui_image_tableau_ref get_image_tableau_at(unsigned col, unsigned row);
vgui_easy2D_ref get_easy2D_at(unsigned col, unsigned row);
vgui_rubberbander_ref get_rubberbander_at(unsigned col, unsigned row);
void get_current(unsigned* col, unsigned* row);
class xcv_tableau_ref;
extern xcv_tableau_ref xcv_tab;
#endif // vplayer_h_
