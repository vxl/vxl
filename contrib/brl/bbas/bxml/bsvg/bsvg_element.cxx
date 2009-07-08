//:
// \file
// \author Ozge C. Ozcanli (Brown)
// \date   April 21, 2009

#include "bsvg_element.h"

void bsvg_element::set_transformation(float trans_x, float trans_y, float rot)
{
  vcl_stringstream trans;
  trans << "translate(" << trans_x << "," << trans_y << ") rotate(" << rot << ")";
  this->set_attribute("transform", trans.str());
}

void bsvg_element::set_location(float trans_x, float trans_y)
{
  vcl_stringstream trans;
  trans << "translate(" << trans_x << "," << trans_y << ") ";
  this->set_attribute("transform", trans.str());
}

//: adds rotation to an existing translation if any
void bsvg_element::set_rotation(float rot)
{
  vcl_stringstream trans;
  trans << "rotate(" << rot << ")";

  vcl_string val;
  if (this->get_attribute("transform", val))
    val = val + " " + trans.str();
  else
    val = trans.str();

  this->set_attribute("transform", val);
}

void bsvg_element::set_fill_color(const vcl_string& c)
{
  this->set_attribute("fill", c);
}

void bsvg_element::set_stroke_color(const vcl_string& c)
{
  this->set_attribute("stroke", c);
}

void bsvg_element::set_stroke_width(float w)
{
  vcl_stringstream sw; sw << w;
  this->set_attribute("stroke-width", sw.str());
}
//: 0 <= opacity <= 1
void bsvg_element::set_fill_opacity(float o)
{
  vcl_stringstream os; os << o;
  this->set_attribute("fill-opacity", os.str());
}
//: 0 <= opacity <= 1
void bsvg_element::set_stroke_opacity(float o)
{
  vcl_stringstream os; os << o;
  this->set_attribute("stroke-opacity", os.str());
}

void bsvg_text::set_font_size(int s)
{
  vcl_stringstream fs; fs << s;
  this->set_attribute("font-size", fs.str());
}

bsvg_ellipse::bsvg_ellipse(float rx, float ry) : bsvg_element("ellipse") 
{
  vcl_stringstream rxs; rxs << rx; vcl_stringstream rys; rys << ry;
  this->set_attribute("rx", rxs.str());
  this->set_attribute("ry", rys.str());
}

bsvg_rectangle::bsvg_rectangle(float x, float y, float width, float height) : bsvg_element("rect") 
{
  vcl_stringstream xs; xs << x; vcl_stringstream ys; ys << y;
  vcl_stringstream ws; ws << width; vcl_stringstream hs; hs << height;

  this->set_attribute("x", xs.str());
  this->set_attribute("y", ys.str());
  this->set_attribute("width", ws.str());
  this->set_attribute("height", hs.str());
}

bsvg_line::bsvg_line(float x1, float y1, float x2, float y2) : bsvg_element("line")
{
  vcl_stringstream x1s; x1s << x1; vcl_stringstream y1s; y1s << y1;
  vcl_stringstream x2s; x2s << x2; vcl_stringstream y2s; y2s << y2;

  this->set_attribute("x1", x1s.str());
  this->set_attribute("y1", y1s.str());
  this->set_attribute("x2", x2s.str());
  this->set_attribute("y2", y2s.str());
}

bsvg_arrow_head::bsvg_arrow_head(float x, float y, float l) : bsvg_group()
{
  this->set_location(x,y);

  bsvg_line* l1 = new bsvg_line(0, 0, 0, l);
  l1->set_rotation(135);
  this->add_element(l1);

  bsvg_line* l2 = new bsvg_line(0, 0, 0, l);
  l2->set_rotation(45);
  this->add_element(l2);

}

bsvg_polyline::bsvg_polyline(const vcl_vector<float>& xs, const vcl_vector<float>& ys) : bsvg_element("polyline") 
{
  if (xs.size() == ys.size()) {
    vcl_stringstream ss;
    for (unsigned i = 0; i < xs.size(); i++) {
      ss << xs[i] << "," << ys[i] << " ";
    }
    this->set_attribute("points", ss.str());
  }
}

