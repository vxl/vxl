#ifndef easy2D_sequencer_h
#define easy2D_sequencer_h
//--------------------------------------------------------------------------------
//:
// \file
// \brief   Holds sequences of geometry.
// \author
//   Marko Bacic, Oxford RRG
// \date
//   Marko Bacic 06-SEP-2000    Initial version. Future release should use some form of caching.
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vgui/vgui_easy2D_tableau.h>
class easy2D_sequencer {
  vcl_vector<vgui_easy2D_tableau_sptr> sequence_;
  vcl_vector<vgui_easy2D_tableau_sptr>::iterator pos_;
public:
  easy2D_sequencer(vcl_vector<vgui_easy2D_tableau_sptr>);
  vgui_easy2D_tableau_sptr next();
  vgui_easy2D_tableau_sptr prev();
};

#endif // easy2D_sequencer_h
