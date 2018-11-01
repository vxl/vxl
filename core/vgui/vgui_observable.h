// This is core/vgui/vgui_observable.h
#ifndef vgui_observable_h_
#define vgui_observable_h_
//:
// \file
// \brief  Base class for classes that want to broadcast messages to observers.
// \author fsm
//
//  Contains class  vgui_observable.
//
// \verbatim
//  Modifications
//   17-Sep-2002 K.Y.McGaul - Added doxygen style comments.
//   31-Jul-2010 Peter Vanroose - minor bug fix (operator= instead of operator==)
// \endverbatim

#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
  void get_observers(std::vector<vgui_observer*>&) const;

  //: Broadcast an update to all observers of this class.
  virtual void notify() const;

  //: Broadcast a message to all observers of this class.
  virtual void notify(const vgui_message &) const;

 private:
  //: List of all observers for this observable.
  std::vector<vgui_observer*> observers;

  //: Disallow assignment.
  vgui_observable(vgui_observable const&) { }

  //: Disallow assignment.
  vgui_observable& operator=(vgui_observable const&) { return *this; }
};

#endif // vgui_observable_h_
