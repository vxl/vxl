// This is oxl/vgui/vgui_observable.h
#ifndef vgui_observable_h_
#define vgui_observable_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include <vcl_vector.h>
class vgui_observer;
class vgui_message;

class vgui_observable
{
 public:
  vgui_observable() { }
  virtual ~vgui_observable();

  //
  void attach(vgui_observer*);
  void detach(vgui_observer*);
  void get_observers(vcl_vector<vgui_observer*>&) const;

  virtual void notify() const;
  virtual void notify(const vgui_message &) const;

 private:
  vcl_vector<vgui_observer*> observers;
  // disallow assignment :
  vgui_observable(vgui_observable const&) { }
  void operator==(vgui_observable const&) { }
};

#endif // vgui_observable_h_
