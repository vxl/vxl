// This is oxl/vgui/vgui_blackbox.h
#ifndef vgui_blackbox_h_
#define vgui_blackbox_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   13 Oct 99
// \brief  Tableau for event record and playback.
//
//  Contains classes:  vgui_blackbox  vgui_blackbox_new
//
// \verbatim
//  Modifications:
//    13-OCT-1999  P.Pritchett - Initial version.
//    26-APR-2002  K.Y.McGaul - Converted to and added doxygen style comments.
// \endverbatim.

#include "vgui_blackbox_sptr.h"
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_event.h>
#include <vcl_vector.h>

//: Tableau for event record and playback.
//
// - `,'  start/stop record 
// - `.'           playback 
// - `s'   playback w. dump 
// - `/'       print events 
// - `#'       clear events 
class vgui_blackbox : public vgui_wrapper_tableau
{
 public:
  //: Constructor - don't use this, use vgui_blackbox_new.
  vgui_blackbox(vgui_tableau_sptr const&);

  //: Handle all events used by this tableau.
  //  In particular, this tableau uses the key-strokes ',' (start/stop
  //  record), '.' (playback), 's' (playback w. dump), '/' (print
  //  events), '#' (clear events).
  //  Other events are passed to the child tableau.
  virtual bool handle(const vgui_event& event);

  //: Return the name of this tableau ('vgui_blackbox').
  virtual vcl_string type_name() const;

 protected:
  //: Destructor - called by smart-pointer vgui_blackbox_sprt.
 ~vgui_blackbox();
  //: True if events are being recorded.
  bool recording;
  //: Not used ????
  bool playback;

  //: List of recorded events.
  vcl_vector<vgui_event> events;
};

//: Create a pointer to a vgui_blackbox.
struct vgui_blackbox_new : public vgui_blackbox_sptr
{
  typedef vgui_blackbox_sptr base;
  vgui_blackbox_new(vgui_tableau_sptr const& a) : base(new vgui_blackbox(a)) { }
};

#endif // vgui_blackbox_h_
