// This is core/vgui/vgui_tview_tableau.h
#ifndef vgui_tview_tableau_h_
#define vgui_tview_tableau_h_
//:
// \file
// \brief  Tableau displaying a tableau tree.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   12 Oct 99
//
//  Contains classes  vgui_tview_tableau  vgui_tview_tableau_new
//
// \verbatim
//  Modifications
//   12-OCT-2002 P.Pritchett - Initial version.
//   07-Aug-2002 K.Y.McGaul - Change to Doxygen style comments.
//   01-OCT-2002 K.Y.McGaul - Moved vgui_tview to vgui_tview_tableau.
// \endverbatim


#include <vector>
#include "vgui_tview_tableau_sptr.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui_wrapper_tableau.h>

//: Tableau displaying a tableau tree.
class vgui_tview_tableau : public vgui_wrapper_tableau
{
 public:
  //: Constructor - don't use this, use vgui_tview_tableau_new.
  vgui_tview_tableau(vgui_tableau_sptr const&);

  bool handle(const vgui_event &);
  std::string type_name() const;

  void get_popup(const vgui_popup_params& params, vgui_menu &menu);


  void draw_tableau_icon(float x, float y);
  void draw_tview_icon(float x, float y);


  struct icon
  {
    vgui_tableau_sptr tableau;
    float x;
    float y;
  };

  void draw_icons(vgui_tableau_sptr const& parent, float x, float y);
  void add_icons(std::vector<icon>* icons, vgui_tableau_sptr const& parent, float x, float y);
  vgui_tableau_sptr find_closest_icon(std::vector<icon> const& icons, float x, float y);

  float spacing;
  float icon_height;
  float icon_width;

  vgui_tableau_sptr active_icon;
  vgui_tableau_sptr closest_icon;

 protected:
 ~vgui_tview_tableau();
};

//: Create a smart-pointer to a vgui_tview_tableau tableau.
struct vgui_tview_tableau_new : public vgui_tview_tableau_sptr
{
  typedef vgui_tview_tableau_sptr base;
  vgui_tview_tableau_new(vgui_tableau_sptr const& arg1000) : base(new vgui_tview_tableau(arg1000)) { }
};

#endif // vgui_tview_tableau_h_
