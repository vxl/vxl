/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vgui_observable.h"
#include "vgui_observer.h"
#include <vcl_algorithm.h>

//
// Base class for classes which want to broadcast messages to vgui_observers
// using notify(m).
//

//--------------------------------------------------------------------------------

vgui_observable::~vgui_observable()
{
}

//--------------------------------------------------------------------------------

void vgui_observable::attach(vgui_observer *o)
{
  observers.push_back(o);
}

void vgui_observable::detach(vgui_observer *o)
{
  vcl_vector<vgui_observer*>::iterator o_iter = vcl_find(observers.begin(),
                                                 observers.end(),
                                                 o);

  if (o_iter != observers.end())
    observers.erase(o_iter);
}

void vgui_observable::get_observers(vcl_vector<vgui_observer*> &o_list) const
{
  for  (vcl_vector<vgui_observer*>::const_iterator o_iter = observers.begin();
        o_iter != observers.end(); ++o_iter) {
    o_list.push_back(*o_iter);
  }
}

void vgui_observable::notify() const
{
  // send an update() to each observer :
  for (vcl_vector<vgui_observer*>::const_iterator oi = observers.begin();
       oi != observers.end(); ++oi)

    (*oi) -> update(this);
}

void vgui_observable::notify(const vgui_message &m) const
{
  // send an update(m) message to each observer :
  for (vcl_vector<vgui_observer*>::const_iterator oi = observers.begin();
       oi != observers.end(); ++oi)

    (*oi) -> update(m);
}

//--------------------------------------------------------------------------------
