#ifndef vgui_blackbox_h_
#define vgui_blackbox_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_blackbox - Undocumented class FIXME
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_blackbox.h
// .FILE vgui_blackbox.cxx
//
// .SECTION Description
//
// vgui_blackbox is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author
//              Philip C. Pritchett, 13 Oct 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//
//-----------------------------------------------------------------------------

#include "vgui_blackbox_ref.h"
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_event.h>
#include <vcl_vector.h>

class vgui_blackbox : public vgui_wrapper_tableau
{
public:
  vgui_blackbox(vgui_tableau_ref const&);

  virtual bool handle(const vgui_event& event);
  virtual vcl_string type_name() const;

protected:
 ~vgui_blackbox();
  bool recording;
  bool playback;

  vcl_vector<vgui_event> events;
};

struct vgui_blackbox_new : public vgui_blackbox_ref {
  typedef vgui_blackbox_ref base;
  vgui_blackbox_new(vgui_tableau_ref const& a) : base(new vgui_blackbox(a)) { }
};

#endif // vgui_blackbox_h_
