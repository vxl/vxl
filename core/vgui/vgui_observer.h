// This is core/vgui/vgui_observer.h
#ifndef vgui_observer_h_
#define vgui_observer_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  Base class for classes that receive messages from observables.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   11 Sep 99
//
//  Contains class vgui_observer
//
// \verbatim
//  Modifications
//   11-Sep-1999 P.Pritchett - Initial version.
//   14-Aug-2002 K.Y.McGaul - Converted to Doxygen style comments.
// \endverbatim

class vgui_message;
class vgui_observable;

//: Base class for classes that receive messages from observables.
//
//  Observers attach them selves to a vgui_observable object.  The observable
//  can broadcast a vgui_message or an update to all the vgui_observer's
//  attached to themselves.
class vgui_observer
{
 public:
  //: Constructor - create a default observable.
  vgui_observer() {}

  //: Destructor.
  virtual ~vgui_observer() {}

  //: Called by the observable when some event occurs.
  virtual void update();

  //: Called by the observable with a message.
  virtual void update(vgui_message const &);

  //: Called by the observable with a message.
  virtual void update(vgui_observable const *);
};

#endif // vgui_observer_h_
