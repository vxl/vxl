// This is core/vgui/vgui_style.h
#ifndef vgui_style_h_
#define vgui_style_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief   Style (colour, line width, point radius) of a geometric object.
// \author  Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date    18 Oct 99
//
//  Contains classes vgui_style, vgui_style_equal
//
// \verbatim
//  Modifications
//   18-OCT-1999 P.Pritchett - Initial version.
//   07-AUG-2002 K.Y.McGaul - Changed to and added Doxygen style comments.
//   06-APR-2004 M.Leotta - Updated for use with smart pointers and no style factory
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vgui/vgui_style_sptr.h>

//: Style (colour, line width, point radius) of a geometric object.
// vgui_style objects are dynamic only and should be accessed via smart pointer only.
// The static new_style member function are only way to produce new styles
// You should call
// \verbatim vgui_style::new_style(...); \endverbatim
// instead of
// \verbatim new vgui_style(...); \endverbatim
class vgui_style : public vbl_ref_count
{
 public:
 
  //: Create a new style object
  static vgui_style_sptr new_style();
  
  //: Create a new style object
  static vgui_style_sptr new_style(float r, float g, float b, float point_size, float line_width);

  //: Create a new style object from an existing one;
  static vgui_style_sptr new_style(const vgui_style_sptr& style);

  //: Sets the GL colour to this style's value
  //
  // Does nothing if the colour is invalid.
  void apply_color() const;

  //: Sets the GL line width to this style's value
  //
  // Does nothing if the width is invalid (e.g. <= 0.0)
  void apply_line_width() const;

  //: Sets the GL point size to this style's value
  //
  // Does nothing if the point size is invalid (e.g. <= 0.0)
  void apply_point_size() const;

  //: Sets all the GL style parameters (colour, size, width).
  void apply_all() const;

  //: Style colour.
  float rgba[4];
  
  //: Style point radius.
  float point_size;
  
  //: Style line width.
  float line_width;

 protected:
  //: Constructor - creates a style with default values.
  vgui_style();
  
  //: Constructor - creates a style and initializes the values
  vgui_style(float r, float g, float b, float point_size, float line_width);

  //: Constructor - creates a style identical to the given style.
  vgui_style(const vgui_style& that) { *this = that; }

  //: Destructor - only the smart pointer should use this
  ~vgui_style();

  friend class vbl_smart_ptr<vgui_style>;
};

//: Finds out whether two vgui_style's are equal.
class vgui_style_equal
{
 public:
  //: Constructor - takes one of the styles to be compared.
  vgui_style_equal(vgui_style_sptr s1_) : s1(s1_) {}

  //: Returns true if the given style is identical to the stored style.
  bool operator() (vgui_style_sptr s2) {
    if (s1->rgba[0] == s2->rgba[0] &&
        s1->rgba[1] == s2->rgba[1] &&
        s1->rgba[2] == s2->rgba[2] &&
        s1->point_size == s2->point_size &&
        s1->line_width == s2->line_width) {
      return true;
    }
    return false;
  }

  vgui_style_sptr s1;
};

#endif // vgui_style_h_
