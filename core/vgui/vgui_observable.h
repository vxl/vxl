// This is oxl/vgui/vgui_observable.h
#ifndef vgui_observable_h_
#define vgui_observable_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm
// \brief  Base class for classes that want to broadcast messages to observers.
//
//  Contains classes:  vgui_observable.
//
// \verbatim
//  Modifications:
//    17-Sep-2002 K.Y.McGaul - Added doxygen style comments.
// \endverbatim

#include <vcl_vector.h>
class vgui_observer;
class vgui_message;


//: Base class for classes that want to broadcast messages to observers.
//
//  Objects from classes derived from vgui_observable can broadcast a 
//  vgui_message or an update using notify() to all the vgui_observer's 
//  attached to themselves.
class vgui_observable
{
 public:
  //: Constructor - create a default observable.
  vgui_observable() { }

  //: Destructor.
  virtual ~vgui_observable();

  //: Attach the given observer to receive notify messages.
  void attach(vgui_observer*);

  //: Detach the given observer.
  void detach(vgui_observer*);

  //: Returns a list of all the observers for this observable.
  void get_observers(vcl_vector<vgui_observer*>&) const;

  //: Broadcast an update to all observers of this class.
  virtual void notify() const;

  //: Broadcast a message to all observers of this class. 
  virtual void notify(const vgui_message &) const;

 private:
  //: List of all observers for this observable.
  vcl_vector<vgui_observer*> observers;

  //: Disallow assignment.
  vgui_observable(vgui_observable const&) { }

  //: Disallow assignment.
  void operator==(vgui_observable const&) { }
};

#endif // vgui_observable_h_
