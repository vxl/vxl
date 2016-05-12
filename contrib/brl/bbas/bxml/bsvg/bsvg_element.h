#ifndef bsvg_element_h_
#define bsvg_element_h_
//:
// \file
// \brief Primitives of an SVG library using bxml
//
//           files created in svg format may be rendered by any web browser.
//
//           So far, added basic shapes: rectangle, line, ellipse, polyline.
//           Other shapes supported by SVG can also be added such as polygon etc. Just subclass from bsvg_element class and write a constructor.
//
// \author Ozge C. Ozcanli (Brown)
// \date   April 21, 2009
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - July 08, 09 - ported to vxl from local repository - minor fixes
// \endverbatim

#include <bxml/bxml_document.h>

class bsvg_element : public bxml_element
{
 public:
  bsvg_element(const std::string& name) : bxml_element(name) {}
  //:  rotation angle is specified in degrees, e.g. 90
  void set_transformation(float trans_x, float trans_y, float rot_angle);
  void set_location(float trans_x, float trans_y);
  //: adds rotation to an existing translation if any angle is specified in degrees, e.g. 90
  void set_rotation(float rot_angle);
  void set_fill_color(const std::string& c);
  //: turns the given red, green, blue values in range [0,255] to #00 00 00 notation (Hex color) for each color
  void set_fill_color(unsigned red, unsigned green, unsigned blue);
  void set_stroke_color(const std::string& c);
  //: turns the given red, green, blue values in range [0,255] to #00 00 00 notation (Hex color) for each color
  void set_stroke_color(unsigned red, unsigned green, unsigned blue);
  void set_stroke_width(float w);
  //: 0 <= opacity <= 1
  void set_fill_opacity(float o);
  //: 0 <= opacity <= 1
  void set_stroke_opacity(float o);
};

class bsvg_text : public bsvg_element
{
 public:
  bsvg_text(const std::string& msg) : bsvg_element("text") { this->append_text(msg); }
  void set_font_size(int s);
};

//: an SVG tag to group elements.
//  e.g. define a group translation and rotation then apply it to all the members of the group
//  or define a stroke-color, fill opacity etc. once and apply it to all the group
class bsvg_group : public bsvg_element
{
 public:
  bsvg_group() : bsvg_element("g") { this->append_text("\n"); }
  bool add_element(const bxml_data_sptr& element) { this->append_data(element); this->append_text("\n"); return true; }
};

class bsvg_ellipse : public bsvg_element
{
 public:
  bsvg_ellipse(float rx, float ry);
};

class bsvg_rectangle : public bsvg_element
{
 public:
  bsvg_rectangle(float x, float y, float width, float height);
};

class bsvg_line : public bsvg_element
{
 public:
  bsvg_line(float x1, float y1, float x2, float y2);
};

//: tip of the arrow will be at x, y and the length will be l
class bsvg_arrow_head : public bsvg_group
{
 public:
  bsvg_arrow_head(float x, float y, float l);
};

class bsvg_polyline : public bsvg_element
{
 public:
  bsvg_polyline(const std::vector<float>& xs, const std::vector<float>& ys);
};

//: draw a splice e.g. for a "pie chart".
//  A splice is an arc of a full circle given by start and end angles and the
//  arc is closed at the ends by lines from and to the center of the circle pass
//  the angles in radians in range [0,2pi]
//  when long_arc = true plots the arc (end_angle, start_angle) (so goes the other way around the circle)
class bsvg_splice : public bsvg_group
{
 public:
  bsvg_splice(float center_x, float center_y, float radius, float start_angle, float end_angle, bool long_arc = false);
};

#endif  // bsvg_element_h_
