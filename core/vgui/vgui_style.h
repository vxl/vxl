// This is oxl/vgui/vgui_style.h
#ifndef vgui_style_h_
#define vgui_style_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author  Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date    18 Oct 99
// \brief   Style (colour, line width, point radius) of a geometric object.
//
//  Contains classes: vgui_style, vgui_style_equal
//
// \verbatim
//  Modifications:
//    18-OCT-1999 P.Pritchett - Initial version.
//    07-AUG-2002 K.Y.McGaul - Changed to and added Doxygen style comments.
// \endverbatim

//: Style (colour, line width, point radius) of a geometric object.
class vgui_style
{
 public:
  //: Constructor - creates a style with default values.
  vgui_style();

  //: Constructor - creates a style identical to the given style.
  vgui_style(const vgui_style& that) { *this = that; }

  //: Destructor.
  ~vgui_style() { }

  //: Style colour.
  float rgba[4];
  //: Style line width.
  float line_width;
  //: Style point radius.
  float point_size;
};

//: Finds out whether two vgui_style's are equal.
class vgui_style_equal
{
 public:
  //: Constructor - takes one of the styles to be compared.
  vgui_style_equal(vgui_style* s1_) : s1(s1_) {}

  //: Returns true if the given style is identical to the stored style.
  bool operator() (vgui_style* s2) {
    if (s1->rgba[0] == s2->rgba[0] &&
        s1->rgba[1] == s2->rgba[1] &&
        s1->rgba[2] == s2->rgba[2] &&
        s1->point_size == s2->point_size &&
        s1->line_width == s2->line_width) {
      return true;
    }
    return false;
  }
 
  vgui_style *s1;
};

#endif // vgui_style_h_
