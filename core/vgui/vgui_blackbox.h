// This is ./oxl/vgui/vgui_blackbox.h

//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   13 Oct 99
// \brief  Event record and playback.
//
// \verbatim
//  Modifications:
//    13-OCT-1999  P.Pritchett - Initial version.
//    26-APR-2002  K.Y.McGaul - Converted to and added doxygen style comments.
// \endverbatim.

#ifndef vgui_blackbox_h_
#define vgui_blackbox_h_
#ifdef __GNUC__
#pragma interface
#endif

#include "vgui_blackbox_sptr.h"
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_event.h>
#include <vcl_vector.h>

//: Event record and playback.
//
// - `,'  start/stop record 
// - `.'           playback 
// - `s'   playback w. dump 
// - `/'       print events 
// - `#'       clear events 
class vgui_blackbox : public vgui_wrapper_tableau
{
public:
  vgui_blackbox(vgui_tableau_sptr const&);

  virtual bool handle(const vgui_event& event);
  virtual vcl_string type_name() const;

protected:
 ~vgui_blackbox();
  //: true if events are being recorded.
  bool recording;
  //: Not used ????
  bool playback;

  //: List of recorded events.
  vcl_vector<vgui_event> events;
};

struct vgui_blackbox_new : public vgui_blackbox_sptr {
  typedef vgui_blackbox_sptr base;
  vgui_blackbox_new(vgui_tableau_sptr const& a) : base(new vgui_blackbox(a)) { }
};

#endif // vgui_blackbox_h_
