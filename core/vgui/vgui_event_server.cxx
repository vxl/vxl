// This is core/vgui/vgui_event_server.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   29 Sep 99
// \brief  See vgui_event_server.h for a description of this file.

#include "vgui_event_server.h"

#include <vgui/vgui.h>
#include <vgui/vgui_wrapper_tableau.h>

// the default is 'false'. don't check in 'true'.
//static bool debug = false;

class vgui_event_server_interpose_tableau : public vgui_wrapper_tableau
{
 public:
  vgui_event_server* the_server_;

  vgui_event_server_interpose_tableau(vgui_event_server* s)
    : vgui_wrapper_tableau(0), // child to be added later
      the_server_(s) {}

  bool handle(const vgui_event& event);
  vcl_string type_name() const {return "vgui_event_server";}
};

//: Interaction (i.e. mouse/kb) events are saved, and said to be consumed.
bool vgui_event_server_interpose_tableau::handle(const vgui_event& event)
{
  // Pass draw events down -- we're just grabbing the interactions
  if (event.type == vgui_DRAW)
    return child.handle(event);

  // Do not grab TIMERs
  if (event.type == vgui_TIMER)
    return false;

  // Grab anything else
  the_server_->last_event_ = event;
  the_server_->use_event_ = true;
  return true;
}

//: Construct a vgui_event_server, which grabs all events intended for tableau t
vgui_event_server::vgui_event_server(vgui_tableau_sptr const& t)
  : grabber_(0)
{
  use_event_ = false;
  grabber_ = new vgui_event_server_interpose_tableau(this);
  grabber_reference_ = grabber_;

  // link up grabber
  vgui_parent_child_link::replace_child_everywhere(t, grabber_);

  // Link t to grabber as child
  grabber_->add_child(t);
}

vgui_event_server::~vgui_event_server()
{
  // Replace the grabber with its child
  vgui_parent_child_link::replace_child_everywhere(grabber_, grabber_->child);
}

void vgui_event_server::reset()
{
}

bool vgui_event_server::next()
{
  use_event_ = false;
  while (!use_event_)
    vgui::run_one_event();

  return true;
}
