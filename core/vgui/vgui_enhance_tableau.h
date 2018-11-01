// This is core/vgui/vgui_enhance_tableau.h
#ifndef vgui_enhance_tableau_h_
#define vgui_enhance_tableau_h_
//:
// \file
// \brief  Magnify/display another tableau in a region around the mouse pointer.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   16 Nov 99
//
//  Contains classes vgui_enhance_tableau  vgui_enhance_tableau_new
//
// \verbatim
//  Modifications
//   16-NOV-1999 P.Pritchett - Initial version.
//   26-APR-2002 K.Y.McGaul - Converted to and added doxygen style comments.
//   01-OCT-2002 K.Y.McGaul - Moved vgui_enhance to vgui_enhance_tableau.
// \endverbatim

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <vgui/vgui_enhance_tableau_sptr.h>

class vgui_event;

//: Magnify or display another tableau in a region around the mouse pointer.
//  This happens when the user presses the left mouse button inside the
//  rendering area.
//
//  Use '[' and ']' to change size of region around the mouse pointer.
//
//  Use '{' and '}' to change the magnification.
class vgui_enhance_tableau : public vgui_tableau
{
 public:
  //: Constructor - don't use this, use vgui_enhance_tableau_new.
  //  A tableau constructed this way magnifies an area under the mouse pointer.
  vgui_enhance_tableau();

  //: Constructor - don't use this, use vgui_enhance_tableau_new.
  //  A tableau constructed this way magnifies area of the given tableau
  //  under the mouse pointer.
  vgui_enhance_tableau(vgui_tableau_sptr const& t);

  //: Constructor - don't use this, use vgui_enhance_tableau_new.
  //  A tableau constructed this way displays the tableau t2 on top of t1 in
  //  a region around the mouse pointer.
  vgui_enhance_tableau(vgui_tableau_sptr const& t1,
                       vgui_tableau_sptr const& t2);

  //: Returns the file_name of the first child tableau.
  std::string file_name() const;

  //: Returns the type of this tableau ('vgui_enhance_tableau').
  std::string type_name() const;

  //: True to enable key-presses to change size and magnification.
  void set_enable_key_bindings(bool on) { enable_key_bindings = on; }

  //: Set the child in the first slot.
  void set_child(vgui_tableau_sptr const& t);

 protected:
  //: Destructor - called by vgui_enhance_tableau_sptr.
 ~vgui_enhance_tableau();

  //: Handle all events sent to this tableau.
  //  In particular, use left mouse press to enhance/magnify.
  bool handle(const vgui_event&);

  //: First child, this is the tableau displayed, except when the mouse is down.
  vgui_parent_child_link slot1;

  //: Second child, displayed when the mouse is pressed in the rendering area.
  vgui_parent_child_link slot2;

  //: True if enhancing is on.
  bool enhancing_;

  //: Mouse x-position.
  int x;

  //: Mouse y-position.
  int y;

  //: Size of enhance region around the mouse pointer.
  int size;

  //: Amount to zoom if we are magnifying.
  float zoom_factor;

  //: True if key presses can change size and zoom factor.
  bool enable_key_bindings;
};

//: Create a smart-pointer to a vgui_enhance_tableau tableau.
struct vgui_enhance_tableau_new : public vgui_enhance_tableau_sptr
{
  //: Constructor - magnifies an area under the mouse pointer.
  vgui_enhance_tableau_new() :
    vgui_enhance_tableau_sptr(new vgui_enhance_tableau()) { }

  //: Constructor - magnifies area of the given tableau under the mouse pointer.
  vgui_enhance_tableau_new(vgui_tableau_sptr const&t) :
    vgui_enhance_tableau_sptr(new vgui_enhance_tableau(t)) { }

  //: Constructor - displays t2 on top of t1 in a region around mouse pointer.
  vgui_enhance_tableau_new(vgui_tableau_sptr const&t1,
                           vgui_tableau_sptr const&t2) :
    vgui_enhance_tableau_sptr(new vgui_enhance_tableau(t1,t2)) { }
};

#endif // vgui_enhance_tableau_h_
