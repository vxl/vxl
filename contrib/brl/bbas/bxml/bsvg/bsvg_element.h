//:
// \file
// \brief Primitives of an SVG library using bxml - 
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

#if !defined(_bsvg_element_h)
#define _bsvg_element_h

#include <bxml/bxml_document.h>

class bsvg_element : public bxml_element
{
public:
    bsvg_element(const vcl_string& name) : bxml_element(name) {}
    //:  rotation angle is specified in degrees, e.g. 90
    void set_transformation(float trans_x, float trans_y, float rot_angle);
    void set_location(float trans_x, float trans_y);
    //: adds rotation to an existing translation if any
    //  angle is specified in degrees, e.g. 90
    void set_rotation(float rot_angle);
    void set_fill_color(const vcl_string& c);
    void set_stroke_color(const vcl_string& c);
    void set_stroke_width(float w);
    //: 0 <= opacity <= 1
    void set_fill_opacity(float o);
    //: 0 <= opacity <= 1
    void set_stroke_opacity(float o);
};

class bsvg_text : public bsvg_element
{
public:
  bsvg_text(const vcl_string& msg) : bsvg_element("text") { this->append_text(msg); }
  void set_font_size(int s);
};

//: an SVG tag to group elements, e.g. define a group translation and rotation then apply it to all the members of the group
//                                     or define a stroke-color, fill opacity etc. once and apply it to all the group
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
  bsvg_polyline(const vcl_vector<float>& xs, const vcl_vector<float>& ys);
};


#endif  //_bsvg_element_h
