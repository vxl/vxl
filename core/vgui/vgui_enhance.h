#ifndef vgui_enhance_h_
#define vgui_enhance_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME vgui_enhance - Undocumented class FIXME
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_enhance.h
// .FILE vgui_enhance.cxx
//
// .SECTION Description:
//
// vgui_enhance is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author:
//              Philip C. Pritchett, 16 Nov 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_slot.h>
#include <vgui/vgui_enhance_sptr.h>

class vgui_event;

class vgui_enhance : public vgui_tableau {
public:
  vgui_enhance();
  vgui_enhance(vgui_tableau_sptr const& t);
  vgui_enhance(vgui_tableau_sptr const& t1, vgui_tableau_sptr const& t2);

  vcl_string file_name() const;
  vcl_string type_name() const;

  void set_enable_key_bindings(bool on) { enable_key_bindings = on; }
  void set_child(vgui_tableau_sptr const& t);

protected:
 ~vgui_enhance();
  bool handle(const vgui_event&);

  vgui_slot slot1;
  vgui_slot slot2;

  bool enhancing_;
  int x, y;
  int size;
  float zoom_factor;
  bool enable_key_bindings;
};

struct vgui_enhance_new : public vgui_enhance_sptr {
  vgui_enhance_new() :
    vgui_enhance_sptr(new vgui_enhance()) { }

  vgui_enhance_new(vgui_tableau_sptr const&t) :
    vgui_enhance_sptr(new vgui_enhance(t)) { }

  vgui_enhance_new(vgui_tableau_sptr const&t1, vgui_tableau_sptr const&t2) :
    vgui_enhance_sptr(new vgui_enhance(t1,t2)) { }
};

#endif // vgui_enhance_h_
