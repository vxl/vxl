#ifndef vgui_tview_h_
#define vgui_tview_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME vgui_tview - Undocumented class FIXME
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_tview.h
// .FILE vgui_tview.cxx
//
// .SECTION Description
//
// vgui_tview is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author
//              Philip C. Pritchett, 12 Oct 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//
//-----------------------------------------------------------------------------

#include "vgui_tview_sptr.h"
#include <vcl_vector.h>
#include <vgui/vgui_wrapper_tableau.h>

class vgui_tview : public vgui_wrapper_tableau
{
public:
  vgui_tview(vgui_tableau_sptr const&);

  bool handle(const vgui_event &);
  vcl_string type_name() const;

  void get_popup(const vgui_popup_params& params, vgui_menu &menu);


  void draw_tableau_icon(float x, float y);
  void draw_tview_icon(float x, float y);


  struct icon {
    vgui_tableau_sptr tableau;
    float x;
    float y;
  };

  void draw_icons(vgui_tableau_sptr const& parent, float x, float y);
  void add_icons(vcl_vector<icon>* icons, vgui_tableau_sptr const& parent, float x, float y);
  vgui_tableau_sptr find_closest_icon(vcl_vector<icon> const& icons, float x, float y);

  float spacing;
  float icon_height;
  float icon_width;

  vgui_tableau_sptr active_icon;
  vgui_tableau_sptr closest_icon;

protected:
 ~vgui_tview();
};

struct vgui_tview_new : public vgui_tview_sptr {
  typedef vgui_tview_sptr base;
  vgui_tview_new(vgui_tableau_sptr const& arg1000) : base(new vgui_tview(arg1000)) { }
};

#endif // vgui_tview_h_
