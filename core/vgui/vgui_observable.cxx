// This is core/vgui/vgui_observable.cxx
//:
// \file
// \author fsm
// \brief  See vgui_observable.h for a description of this file.

#include <algorithm>
#include "vgui_observable.h"
#include <vgui/vgui_observer.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//
// Base class for classes which want to broadcast messages to vgui_observers
// using notify(m).
//

//------------------------------------------------------------------------------

vgui_observable::~vgui_observable()
{
}

//------------------------------------------------------------------------------

void vgui_observable::attach(vgui_observer *o)
{
  observers.push_back(o);
}

void vgui_observable::detach(vgui_observer *o)
{
  std::vector<vgui_observer*>::iterator o_iter = std::find(observers.begin(),
                                                         observers.end(),
                                                         o);

  if (o_iter != observers.end())
    observers.erase(o_iter);
}

void vgui_observable::get_observers(std::vector<vgui_observer*> &o_list) const
{
  for  (std::vector<vgui_observer*>::const_iterator o_iter = observers.begin();
        o_iter != observers.end(); ++o_iter)
    o_list.push_back(*o_iter);
}

void vgui_observable::notify() const
{
  // send an update() to each observer :
  for (std::vector<vgui_observer*>::const_iterator oi = observers.begin();
       oi != observers.end(); ++oi)
    (*oi) -> update(this);
}

void vgui_observable::notify(const vgui_message &m) const
{
  // send an update(m) message to each observer :
  for (std::vector<vgui_observer*>::const_iterator oi = observers.begin();
       oi != observers.end(); ++oi)
    (*oi) -> update(m);
}

//------------------------------------------------------------------------------
