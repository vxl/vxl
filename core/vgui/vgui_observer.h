// This is oxl/vgui/vgui_observer.h
#ifndef vgui_observer_h_
#define vgui_observer_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
// 
// .NAME vgui_observer - observer base class
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_observer.h
// .FILE vgui_observer.cxx
//
// .SECTION Description:
//
// vgui_observer is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author:
//              Philip C. Pritchett, 11 Sep 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

class vgui_message;
class vgui_observable;

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
