// This is oxl/vgui/vgui_listmanager2D.h
#ifndef vgui_listmanager2D_h_
#define vgui_listmanager2D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   21 Oct 99
// \brief  Tableau which manages a set of vgui_displaylist2D children
//
//  Contains classes: vgui_listmanager2D  vgui_listmanager2D_new

#include <vgui/vgui_observable.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_slot.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_displaylist2D.h>
#include <vgui/vgui_listmanager2D_sptr.h>

class vgui_event;
class vgui_displaylist2D;
class vgui_soview2D;

//: Tableau which manages a set of vgui_displaylist2D children
//
// It behaves like an acetate, but is more efficient.
class vgui_listmanager2D : public vgui_tableau
{
 public:
  //: Constructor - don't use this, use vgui_listmanager2D_new.
  vgui_listmanager2D();
 ~vgui_listmanager2D();

  vcl_string type_name() const;

  void add(vgui_displaylist2D_sptr const&);
  void remove(vgui_displaylist2D_sptr const&);

  vgui_observable observers;

  // Child activity/visibility
  void set_active(int, bool);
  void set_visible(int, bool);

  bool is_active(int);
  bool is_visible(int);

  // Interaction
  bool handle(const vgui_event&);
  bool help();
  bool key_press(int /*x*/, int /*y*/, vgui_key key, vgui_modifier);
  bool mouse_down(int x, int y, vgui_button button, vgui_modifier modifier);
  bool motion(int x, int y);

 protected:

  // helper
  bool index_ok(int);

  // data
  vcl_vector<vgui_slot> children;
  vcl_vector<bool> active;
  vcl_vector<bool> visible;

  vgui_displaylist2D_sptr highlight_list;
  vgui_soview2D *highlight_so;
  vgui_event saved_event_;

  vgui_displaylist2D_sptr contains_hit(vcl_vector<unsigned> const& names);
  void get_hits(float x, float y, vcl_vector<vcl_vector<unsigned> >* hits);
  void find_closest(float x, float y, vcl_vector<vcl_vector<unsigned> >* hits,
                    vgui_soview2D** closest_so, vgui_displaylist2D_sptr * closest_display);
};

struct vgui_listmanager2D_new : public vgui_listmanager2D_sptr
{
  vgui_listmanager2D_new() :
    vgui_listmanager2D_sptr(new vgui_listmanager2D) { }
};

#endif // vgui_listmanager2D_h_
