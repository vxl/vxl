#ifndef vgui_tableau_h_
#define vgui_tableau_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
//  \file
//
// \author
//              Philip C. Pritchett, 11 Sep 99
//              Robotics Research Group, University of Oxford
//
// \verbatim
// Modifications:
//  21 September 1999  fsm@robots various changes.
//   5 October   1999  fsm@robots
//  11 October   1999  fsm@robots. removed old build_chain code.
//  12 October   1999  fsm@robots. added type_name() method. various cleanup.
//  13 October   1999  fsm@robots. deprecated draw_impl() and draw_overlay_impl().
//  16 October   1999  fsm@robots. deprecated draw(), draw_overlay() + two minor
//                     methods. added get_popup().
//  11 November  1999  fsm@robots. added add_popup() now that get_popup() has a
//                     different meaning. Added exists().
// \endverbatim

#include <vcl_string.h>
#include <vcl_vector.h>

#include <vgui/vgui_event.h>
#include <vgui/internals/vgui_slot_data.h>
#include "dll.h"

class vgui_menu;
class vgui_popup_params;
struct vgui_tableau_sptr;

class vgui_tableau : public vgui_slot_data {
public:
  vgui_tableau();

  // name methods.
  virtual vcl_string name() const;
  //: get filename from descendant holding a file (if meaningful).
  virtual vcl_string file_name() const;
  //: returns name suitable for debugging purposes.
  virtual vcl_string pretty_name() const;
  //: return name of most derived class (for RTTI purposes).
  virtual vcl_string type_name() const;

  //: methods to get/set children and parents.
  void get_parents (vcl_vector<vgui_tableau_sptr> *out) const;
  void get_children(vcl_vector<vgui_tableau_sptr> *out) const;
  vgui_tableau_sptr get_child(unsigned i) const;
  virtual bool add_child(vgui_tableau_sptr const &);
  virtual bool remove_child(vgui_tableau_sptr const &);
  static void get_all(vcl_vector<vgui_tableau_sptr> *out);
  static bool exists(vgui_tableau_sptr const &);

  // Called whenever a child of this tableau is about to be forcibly replaced
  virtual bool notify_replaced_child(vgui_tableau_sptr const & old_child,
                                     vgui_tableau_sptr const & new_child);

  // methods related to popup menus.
  virtual void add_popup(vgui_menu &);
  virtual void get_popup(vgui_popup_params const &, vgui_menu &);

  // redraw and other post() methods. the fact that these are virtual
  // does not imply that you should go and override them.
  virtual void post_message(char const *, void const *);
  virtual void post_redraw();
  virtual void post_overlay_redraw();

  // The handle method.
  virtual bool handle(vgui_event const &);

  // const. if you need to cache, cast away const.
  virtual bool get_bounding_box(float low[3], float high[3]) const;

  // Convenience handle methods. These are called by the default handle() method.
  virtual bool draw();
  virtual bool mouse_down(int x, int y, vgui_button, vgui_modifier);
  virtual bool mouse_up(int x, int y, vgui_button, vgui_modifier);
  virtual bool motion(int x, int y);
  virtual bool key_press(int x, int y, vgui_key, vgui_modifier);
  virtual bool help(); // this is called if '?' is pressed

  // "const" is for convenience. it is cast away internally.
  void ref() const;
  void unref() const;

  //
  void adopt (vgui_tableau_sptr const &) const;
  void disown(vgui_tableau_sptr const &) const;

protected:
  virtual ~vgui_tableau();

private:
  friend struct vgui_slot;
  friend struct vgui_slot_impl;
  friend class vgui_adaptor;

  int references; // reference count. starts at 0.
};

//: print some indication of what the tableau is.
vcl_ostream &operator<<(vcl_ostream &os, vgui_tableau_sptr const &t);

#include <vgui/vgui_tableau_sptr.h>

#endif // vgui_tableau_h_
