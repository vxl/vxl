// This is core/vgui/vgui_blackbox_tableau.h
#ifndef vgui_blackbox_tableau_h_
#define vgui_blackbox_tableau_h_
//:
// \file
// \brief  Tableau for event record and playback.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   13 Oct 99
//
//  Contains classes  vgui_blackbox_tableau  vgui_blackbox_tableau_new
//
// \verbatim
//  Modifications
//   13-OCT-1999  P.Pritchett - Initial version.
//   26-APR-2002  K.Y.McGaul  - Converted to and added doxygen style comments.
//   01-OCT-2002  K.Y.McGaul  - Moved vgui_blackbox to vgui_blackbox_tableau.
//                            - Removed 'playback' - doesn't seem to be used.
// \endverbatim

#include <vector>
#include "vgui_blackbox_tableau_sptr.h"
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_event.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Tableau for event record and playback.
//
// The user can control event recording and playback by pressing the
// following keys inside the rendering area:
//
// - `,'  start/stop record
// - `.'           playback
// - `s'   playback w. dump
// - `/'       print events
// - `#'       clear events
class vgui_blackbox_tableau : public vgui_wrapper_tableau
{
 public:
  //: Constructor - don't use this, use vgui_blackbox_tableau_new.
  vgui_blackbox_tableau(vgui_tableau_sptr const&);

  //: Handle all events used by this tableau.
  //  In particular, this tableau uses the key-strokes ',' (start/stop
  //  record), '.' (playback), 's' (playback w. dump), '/' (print
  //  events), '#' (clear events).
  //  Other events are passed to the child tableau.
  virtual bool handle(const vgui_event& event);

  //: Return the name of this tableau ('vgui_blackbox_tableau').
  virtual std::string type_name() const;

 protected:
  //: Destructor - called by smart-pointer vgui_blackbox_tableau_sptr.
 ~vgui_blackbox_tableau();

  //: True if events are being recorded.
  bool recording;

  //: List of recorded events.
  std::vector<vgui_event> events;
};

//: Create a smart-pointer to a vgui_blackbox_tableau.
struct vgui_blackbox_tableau_new : public vgui_blackbox_tableau_sptr
{
  typedef vgui_blackbox_tableau_sptr base;

  //: Constructor - creates a smart-pointer to a vgui_blackbox_tableau.
  //  Takes the single child tableau as a parameter.
  vgui_blackbox_tableau_new(vgui_tableau_sptr const& a) : base(new vgui_blackbox_tableau(a)) { }
};

#endif // vgui_blackbox_tableau_h_
