// This is oxl/vgui/vgui_observer.h
#ifndef vgui_observer_h_
#define vgui_observer_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   11 Sep 99
// \brief  Observer base class
//
//  Contains classes: vgui_observer
//
// \verbatim
//  Modifications:
//    11-Sep-1999 P.Pritchett - Initial version.
//    14-Aug-2002 K.Y.McGaul - Converted to Doxygen style comments.
// \endverbatim

class vgui_message;
class vgui_observable;

//: Observer base class
class vgui_observer
{
 public:
  vgui_observer();
  virtual ~vgui_observer();

  virtual void update();
  virtual void update(vgui_message const &);
  virtual void update(vgui_observable const *);
};

#endif // vgui_observer_h_
