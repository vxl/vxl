// This is core/vgui/vgui_tableau.h
#ifndef vgui_tableau_h_
#define vgui_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   11 Sep 99
// \brief  Base class for all tableaux in vgui.
//
//  Contains classes: vgui_tableau.
//
// \verbatim
//  Modifications
//   21-SEP-1999 fsm various changes.
//   05-OCT-1999 fsm
//   11-OCT-1999 fsm. removed old build_chain code.
//   12-OCT-1999 fsm. added type_name() method. various cleanup.
//   13-OCT-1999 fsm. deprecated draw_impl() and draw_overlay_impl().
//   16-OCT-1999 fsm. deprecated draw(), draw_overlay() + two minor
//                    methods. added get_popup().
//   11-NOV-1999 fsm. added add_popup() now that get_popup() has a
//                    different meaning. Added exists().
//   07-AUG-2002 K.Y.McGaul - Changed to and added Doxygen style comments.
//   08-OCT-2002 K.Y.McGaul - Removed unused adopt and disown functions.
// \endverbatim

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iosfwd.h>

#include <vgui/vgui_event.h>
#include <vgui/internals/vgui_parent_child_link_data.h>
#include "dll.h"

class vgui_menu;
class vgui_popup_params;
struct vgui_tableau_sptr;

//: Base class for all tableaux in vgui.
//
//  WHAT'S A TABLEAU?
//  A tableau is a rectangular area of the screen on which OpenGL may be
//  used to draw and from which events (mouse, key presses etc) are received.
//
//  WHAT SORT OF THINGS CAN TABLEAUX DO?
//  Currently VGUI provides several example tableaux derived from this
//  class, sufficient to construct complex user interfaces combining 2D and
//  3D with manipulators and zoom. So for example an `image' tableau could
//  display an image, a `zoomer' tableau could provide a function to zoom
//  an image in and out. We could also make a `deck' tableau to hold a deck
//  of images and display one image at a time (this would be useful for
//  playing a series of images as a movie).
//
//  WHAT ARE PARENT AND CHILD TABLEAUX?
//  One tableau can be included as a child of another (parent) tableau. This
//  creates a new tableau with the functionality of both tableaux making it
//  up. So the new tableau could, for example, display a deck of images and
//  provide zooming.
//
//  The order in which the inclusion is done will affect the functionality,
//  so if the `zoomer' tableau contains the `deck' tableau then all images
//  will change size together. If however the inclusion is done the other way
//  around and each child tableau of the `deck' contains its own `zoomer'
//  tableau then each image will change size independently.
//
//  WHAT'S ALL THIS SPTR BUSINESS THEN?
//  When you make a new tableau it is recommended that you don't create
//  it from the constructor for that tableau (eg, vgui_wibble_tableau()),
//  but instead call vgui_wibble_tableau_new().  This returns a smart
//  pointer to your tableau (vgui_wibble_tableau_sptr) and means that
//  you don't need to worry about deleting your tableau once you are
//  finished using it.
//
//  SO HOW DO I USE A TABLEAU?
//  Once-off adaptor code (see vgui_adaptor) is provided that plugs a
//  tableau into any supported GUI toolkit.  So to use a tableau you make
//  a window, add an adaptor and set the tableau in that adaptor to be
//  your top level (highest parent) tableau. See the examples on the VXL
//  webpage to see this done.
class vgui_tableau : public vgui_parent_child_link_data
{
 public:
  //: Constructor - in general you should not use this, use vgui_tableau_new.
  vgui_tableau();

  //: Return the name of the tableau.
  virtual vcl_string name() const { return file_name(); }

  //: Return the name of a file associated with some tableau below (if meaningful).
  virtual vcl_string file_name() const { return "(none)"; }

  //: Used to provide an informative name for printouts, debugging etc.
  //  Often it's type_name() + some representation of the essential state.
  virtual vcl_string pretty_name() const { return type_name(); }

  //: Return name of most derived class (for RTTI purposes).
  virtual vcl_string type_name() const;

  //: Get the parent tableaux for this tableau.
  void get_parents (vcl_vector<vgui_tableau_sptr> *out) const;

  //: Get the child tableaux for this tableau.
  void get_children(vcl_vector<vgui_tableau_sptr> *out) const;

  //: Get the ith child or return 0.
  vgui_tableau_sptr get_child(unsigned i) const;

  //: Add the given tableau to the list of child tableaux.
  //  Virtual overridden by consenting parents.
  virtual bool add_child(vgui_tableau_sptr const &);

  //: Remove the given child from the list of child tableaux.
  virtual bool remove_child(vgui_tableau_sptr const &);

  //: Push all tableaux onto the given vector.
  static void get_all(vcl_vector<vgui_tableau_sptr> *out);

  //: Returns true if the given address points to a valid tableau.
  static bool exists(vgui_tableau_sptr const &);

  //: Called whenever a child of this tableau is about to be forcibly replaced
  virtual bool notify_replaced_child(vgui_tableau_sptr const & old_child,
                                     vgui_tableau_sptr const & new_child);

  //: Add the given menu to the popup menu for the tableau.
  virtual void add_popup(vgui_menu &);

  //: Get the default popup menu for the tableau.
  virtual void get_popup(vgui_popup_params const &, vgui_menu &);

  //: Post a message event.
  //  The fact that this is virtual does not imply that you should
  //  go and override it.
  virtual void post_message(char const *, void const *);

  //: Post a draw event.
  //  The fact that this is virtual does not imply that you should
  //  go and override it.
  virtual void post_redraw();

  //: Post a overlay-redraw event.
  //  The fact that this is virtual does not imply that you should
  //  go and override it.
  virtual void post_overlay_redraw();

  //: Post an idle request event.
  //  The fact that this is virtual does not imply that you should
  //  go and override it.
  //
  //  Posting an idle event request means that your tableau has some
  //  idle processing that it'd like to do. This means that your
  //  tableau will continue to receive vgui_IDLE events until the
  //  event handler returns false (i.e. all idle processing is
  //  complete). The idle event handler should return false when it
  //  has no idle processing, or has completed its idle processing. It
  //  may return true if has only partially completed its idle
  //  processing; in this case, it will receive more idle event to
  //  allow it to complete processing.
  //
  virtual void post_idle_request();

  //: Handle all events sent to this tableau.
  //  Override in subclass to give the tableau some appearance and behaviour.
  virtual bool handle(vgui_event const &);

  //: Get the bounding box of this tableau.
  //  If infinite in extent, or nothing is drawn, or you can't be bothered to
  //  implement it, return false.
  //  const. if you need to cache, cast away const.
  virtual bool get_bounding_box(float low[3], float high[3]) const;

  //: Called by default handle when it receives a draw event.
  virtual bool draw();

  //: Called by default handle when it receives a mouse down event.
  virtual bool mouse_down(int x, int y, vgui_button, vgui_modifier);

  //: Called by default handle when it receives a mouse up event.
  virtual bool mouse_up(int x, int y, vgui_button, vgui_modifier);

  //: Called by handle when it receives a mouse motion event.
  virtual bool motion(int x, int y);

  //: Called by default handle when it receives a key-press event.
  virtual bool key_press(int x, int y, vgui_key, vgui_modifier);

  //: Called by default handle when it receives a '?' pressed event.
  virtual bool help(); // this is called if '?' is pressed

  //: Called when the application is otherwise idle.
  // Override if you want to do idle processing.
  // Return false once your idle processing is complete,
  // or if you have no need for more idle processing.
  // Return true if you need more idle processing time.
  virtual bool idle();

  //: Increase the reference count by one (for smart pointers).
  //  "const" is for convenience, it is cast away internally.
  void ref() const;

  //: Decrease the reference count by one (for smart pointers).
  //  "const" is for convenience, it is cast away internally.
  //  If the reference count reaches zero then delete the object.
  void unref() const;

 protected:
  //: Destructor - called by vgui_tableau_sptr.
  virtual ~vgui_tableau();

 private:
  friend struct vgui_parent_child_link;
  friend struct vgui_parent_child_link_impl;
  friend class vgui_adaptor;

  //: Reference count - starts at 0.
  int references;
};

//: Print some indication of what the tableau is.
vcl_ostream &operator<<(vcl_ostream &os, vgui_tableau_sptr const &t);

#include "vgui_tableau_sptr.h"

#endif // vgui_tableau_h_
