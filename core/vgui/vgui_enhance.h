// This is oxl/vgui/vgui_enhance.h
#ifndef vgui_enhance_h_
#define vgui_enhance_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   16 Nov 99
// \brief  Magnify/display another tableau in a region around the mouse pointer.
//
//  Contains classes: vgui_enhance  vgui_enhance_new
//
// \verbatim
//  Modifications:
//    16-NOV-1999 P.Pritchett - Initial version.
//    26-APR-2002 K.Y.McGaul - Converted to and added doxygen style comments.
// \endverbatim

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_slot.h>
#include <vgui/vgui_enhance_sptr.h>

class vgui_event;

//: Magnify or display another tableau in a region around the mouse pointer.
//  This happens when the user presses the left mouse button inside the 
//  rendering area.
//
//  Use '[' and ']' to change size of region around the mouse pointer.
//
//  Use '{' and '}' to change the magnification.
class vgui_enhance : public vgui_tableau
{
 public:
  //: Constructor - don't use this, use vgui_enhance_new.
  //  A tableau constructed this way magnifies an area under the mouse pointer.
  vgui_enhance();

  //: Constructor - don't use this, use vgui_enhance_new.
  //  A tableau constructed this way magnifies area of the given tableau 
  //  under the mouse pointer.
  vgui_enhance(vgui_tableau_sptr const& t);

  //: Constructor - don't use this, use vgui_enhance_new.
  //  A tableau constructed this way displays the tableau t2 on top of t1 in 
  //  a region around the mouse pointer.
  vgui_enhance(vgui_tableau_sptr const& t1, vgui_tableau_sptr const& t2);

  //: Returns the file_name of the first child tableau.
  vcl_string file_name() const;

  //: Returns the type of this tableau ('vgui_enhance').
  vcl_string type_name() const;

  //: True to enable key-presses to change size and magnification.
  void set_enable_key_bindings(bool on) { enable_key_bindings = on; }

  //: Set the child in the first slot.
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

//: Create a smart-pointer to a vgui_enhance tableau.
struct vgui_enhance_new : public vgui_enhance_sptr
{
  //: Constructor - magnifies an area under the mouse pointer.
  vgui_enhance_new() :
    vgui_enhance_sptr(new vgui_enhance()) { }

  //: Constructor - magnifies area of the given tableau under the mouse pointer.
  vgui_enhance_new(vgui_tableau_sptr const&t) :
    vgui_enhance_sptr(new vgui_enhance(t)) { }

  //: Constructor - displays t2 on top of t1 in a region around mouse pointer.
  vgui_enhance_new(vgui_tableau_sptr const&t1, vgui_tableau_sptr const&t2) :
    vgui_enhance_sptr(new vgui_enhance(t1,t2)) { }
};

#endif // vgui_enhance_h_
