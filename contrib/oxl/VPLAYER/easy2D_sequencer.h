#ifndef easy2D_sequencer_h
#define easy2D_sequencer_h
//--------------------------------------------------------------------------------
// .NAME easy2D_sequencer
// .SECTION Description:
//   Holds sequences of geometry.
// .SECTION Author
//   Marko Bacic, Oxford RRG
// .SECTION Modifications:
//   Marko Bacic 06-SEP-2000    Initial version. Future release should use
//                              some form of caching.
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vgui/vgui_easy2D.h>
class easy2D_sequencer {
  vcl_vector<vgui_easy2D *> sequence_;
  vcl_vector<vgui_easy2D *>::iterator pos_;
public:
  easy2D_sequencer(vcl_vector<vgui_easy2D *>);
  vgui_easy2D *next();
  vgui_easy2D *prev();
};

#endif // easy2D_sequencer_h
