// This is core/vgui/vgui_style.h
#ifndef vgui_style_h_
#define vgui_style_h_
//:
// \file
// \brief  Style (colour, line width, point radius) of a geometric object.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   18 Oct 99
//
//  Contains classes vgui_style, vgui_style_equal
//
// \verbatim
//  Modifications
//   18-OCT-1999 P.Pritchett - Initial version.
//   07-AUG-2002 K.Y.McGaul - Changed to and added Doxygen style comments.
//   06-APR-2004 M.Leotta - Updated for use with smart pointers and no style factory
//   2004/09/10 Peter Vanroose - Added explicit copy constructor (ref_count !)
//   07-AUG-2006 M.Leotta - Enabled alpha channel for transparency
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include "vgui_style_sptr.h"

//: Style (colour, line width, point radius) of a geometric object.
// vgui_style objects are dynamic only and should be accessed via smart pointer only.
// The static new_style member function is the only way to produce new styles.
// You should call
// \code
//  vgui_style::new_style(...);
// \endcode
// instead of
// \code
//  new vgui_style(...);
// \endcode
class vgui_style : public vbl_ref_count
{
 public:

  //: Create a new style object
  static vgui_style_sptr new_style();

  //: Create a new style object
  static vgui_style_sptr new_style(float r, float g, float b,
                                   float point_size, float line_width,
                                   float alpha = 1.0f);

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
  vgui_style(float r, float g, float b, float point_size, float line_width, float alpha);

  // Copy constructor
  vgui_style(vgui_style const& s)
    : vbl_ref_count(), point_size(s.point_size), line_width(s.line_width)
    { rgba[0]=s.rgba[0]; rgba[1]=s.rgba[1]; rgba[2]=s.rgba[2]; rgba[3]=s.rgba[3]; }

  // Destructor - only the smart pointer should use this
  ~vgui_style();

  friend class vbl_smart_ptr<vgui_style>;
};

//: Finds out whether two vgui_styles are equal.
class vgui_style_equal
{
 public:
  //: Constructor - takes one of the styles to be compared.
  vgui_style_equal(vgui_style_sptr s1);

  //: Returns true if the given style is identical to the stored style.
  bool operator() (vgui_style_sptr s2);

  vgui_style_sptr s_;
};

#endif // vgui_style_h_
