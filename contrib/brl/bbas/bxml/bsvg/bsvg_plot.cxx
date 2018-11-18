#include <iostream>
#include <sstream>
#include "bsvg_plot.h"
//:
// \file
// \author Ozge C. Ozcanli (Brown)
// \date   April 21, 2009

#include <bxml/bxml_find.h>
#include <bxml/bsvg/bsvg_element.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void bsvg_plot::add_axes(float x_min, float x_max, float y_min, float y_max, float stroke_width)
{
  float height_y = y_max - y_min;
  // find origin so that height of the plot is scaled wrt to the svg document width and height
  h2_y = h_ - 4*margin_;
  scale_factor_ = h2_y/height_y;

  axes_orig_x_ = 2*margin_;
  axes_orig_y_ = 2*margin_+h2_y;

  auto* line_y = new bsvg_line(axes_orig_x_, axes_orig_y_, axes_orig_x_, axes_orig_y_-h2_y-margin_);
  line_y->set_stroke_color("black");
  line_y->set_stroke_width(stroke_width);

  float height_x = x_max - x_min;
  // find origin so that height of the plot is scaled wrt to the svg document width and height
  h2_x = height_x*scale_factor_;

  auto* line_x = new bsvg_line(axes_orig_x_, axes_orig_y_, axes_orig_x_+h2_x+margin_, axes_orig_y_);
  line_x->set_stroke_color("black");
  line_x->set_stroke_width(stroke_width);

  this->add_element(line_y);
  this->add_element(line_x);

  // add text to denote x_min
  std::stringstream ss; ss << x_min;
  bsvg_text* t = new bsvg_text(ss.str());
  t->set_location(axes_orig_x_, axes_orig_y_+margin_);
  t->set_font_size(font_size_);
  this->add_element(t);

  // add text to denote y_min
  std::stringstream ssy; ssy << y_min;
  bsvg_text* ty = new bsvg_text(ssy.str());
  ty->set_location(axes_orig_x_-margin_, axes_orig_y_);
  ty->set_font_size(font_size_);
  this->add_element(ty);

  // add text to denote x_max
  std::stringstream ssx; ssx << x_max;
  bsvg_text* txm = new bsvg_text(ssx.str());
  txm->set_location(axes_orig_x_+h2_x, axes_orig_y_+margin_);
  txm->set_font_size(font_size_);
  this->add_element(txm);

  // add a short line to denote x_max
  auto* line_xm = new bsvg_line(axes_orig_x_ + h2_x, axes_orig_y_, axes_orig_x_ + h2_x, axes_orig_y_+(margin_/4.0f));
  line_xm->set_stroke_color("black");
  line_xm->set_stroke_width(stroke_width);
  this->add_element(line_xm);

  // add text to denote y_max
  std::stringstream ssym; ssym << y_max;
  bsvg_text* tym = new bsvg_text(ssym.str());
  tym->set_location(axes_orig_x_-margin_, axes_orig_y_-h2_y);
  tym->set_font_size(font_size_);
  this->add_element(tym);

  // add a short line to denote y_max
  auto* line_ym = new bsvg_line(axes_orig_x_-(margin_/4.0f), axes_orig_y_-h2_y, axes_orig_x_, axes_orig_y_-h2_y);
  line_ym->set_stroke_color("black");
  line_ym->set_stroke_width(stroke_width);
  this->add_element(line_ym);

  // put an arrow head at the end of x axis
  auto* a1 = new bsvg_arrow_head(axes_orig_x_+h2_x+margin_, axes_orig_y_, 10.0f);
  a1->set_stroke_width(stroke_width);
  a1->set_stroke_color("black");
  this->add_element(a1);

  // put an arrow head at the end of y axis
  auto* a2 = new bsvg_arrow_head(axes_orig_x_, axes_orig_y_-h2_y-margin_, 10.0f);
  a2->set_stroke_width(stroke_width);
  a2->set_rotation(-90);
  a2->set_stroke_color("black");
  this->add_element(a2);
}

void bsvg_plot::add_axes(float x_min, float x_max, float y_min, float y_max, bool is_scale_x, float stroke_width)
{
  if (is_scale_x)
    this->add_axes(x_min, x_max, y_min, y_max, stroke_width);
  else {
    float height_y = y_max - y_min;
    // find origin so that height of the plot is scaled wrt to the svg document width and height
    h2_y = h_ - 4*margin_;
    scale_factor_ = h2_y/height_y;

    axes_orig_x_ = 2*margin_;
    axes_orig_y_ = 2*margin_+h2_y;

    auto* line_y = new bsvg_line(axes_orig_x_, axes_orig_y_, axes_orig_x_, axes_orig_y_-h2_y-margin_);
    line_y->set_stroke_color("black");
    line_y->set_stroke_width(stroke_width);

    float height_x = x_max - x_min;
    // find origin so that height of the plot is scaled wrt to the svg document width and height
    h2_x = height_x;

    auto* line_x = new bsvg_line(axes_orig_x_, axes_orig_y_, axes_orig_x_+h2_x+margin_, axes_orig_y_);
    line_x->set_stroke_color("black");
    line_x->set_stroke_width(stroke_width);

    this->add_element(line_y);
    this->add_element(line_x);

    // add text to denote x_min
    std::stringstream ss; ss << x_min;
    bsvg_text* t = new bsvg_text(ss.str());
    t->set_location(axes_orig_x_, axes_orig_y_+margin_);
    t->set_font_size(font_size_);
    this->add_element(t);

    // add text to denote y_min
    std::stringstream ssy; ssy << y_min;
    bsvg_text* ty = new bsvg_text(ssy.str());
    ty->set_location(axes_orig_x_-margin_, axes_orig_y_);
    ty->set_font_size(font_size_);
    this->add_element(ty);

    // add text to denote x_max
    std::stringstream ssx; ssx << x_max;
    bsvg_text* txm = new bsvg_text(ssx.str());
    txm->set_location(axes_orig_x_+h2_x, axes_orig_y_+margin_);
    txm->set_font_size(font_size_);
    this->add_element(txm);

    // add a short line to denote x_max
    auto* line_xm = new bsvg_line(axes_orig_x_ + h2_x, axes_orig_y_, axes_orig_x_ + h2_x, axes_orig_y_+(margin_/4.0f));
    line_xm->set_stroke_color("black");
    line_xm->set_stroke_width(stroke_width);
    this->add_element(line_xm);

    // add text to denote y_max
    std::stringstream ssym; ssym << y_max;
    bsvg_text* tym = new bsvg_text(ssym.str());
    tym->set_location(axes_orig_x_-margin_, axes_orig_y_-h2_y);
    tym->set_font_size(font_size_);
    this->add_element(tym);

    // add a short line to denote y_max
    auto* line_ym = new bsvg_line(axes_orig_x_-(margin_/4.0f), axes_orig_y_-h2_y, axes_orig_x_, axes_orig_y_-h2_y);
    line_ym->set_stroke_color("black");
    line_ym->set_stroke_width(stroke_width);
    this->add_element(line_ym);

    // put an arrow head at the end of x axis
    auto* a1 = new bsvg_arrow_head(axes_orig_x_+h2_x+margin_, axes_orig_y_, 10.0f);
    a1->set_stroke_width(stroke_width);
    a1->set_stroke_color("black");
    this->add_element(a1);

    // put an arrow head at the end of y axis
    auto* a2 = new bsvg_arrow_head(axes_orig_x_, axes_orig_y_-h2_y-margin_, 10.0f);
    a2->set_stroke_width(stroke_width);
    a2->set_rotation(-90);
    a2->set_stroke_color("black");
    this->add_element(a2);
  }

}

void bsvg_plot::add_title(const std::string& t)
{
  auto* title = new bsvg_text(t);
  auto w = float(font_size_*t.size());
  title->set_location((this->w_-margin_)/2.0f - w/2, margin_);
  //title->set_location((h2_x+3*margin_-w)/2, margin_);
  title->set_font_size(font_size_);
  this->add_element(title);
}

//: assumes add_axes have been called
void bsvg_plot::add_x_increments(float x_inc, float stroke_width)
{
  float x_inc_scaled = scale_factor_*x_inc;
  auto* g = new bsvg_group();
  g->set_stroke_color("black");
  g->set_stroke_width(stroke_width);
  g->set_stroke_opacity(0.5);

  float end = axes_orig_x_ + h2_x;
  for (float x = axes_orig_x_ + x_inc_scaled; x <= end; x += x_inc_scaled) {
    auto* line_x = new bsvg_line(x, axes_orig_y_, x, axes_orig_y_-h2_y);
    g->add_element(line_x);
  }
  this->add_element(g);
}

//: assumes add_axes have been called
void bsvg_plot::add_y_increments(float y_inc, float stroke_width)
{
  float y_inc_scaled = scale_factor_*y_inc;
  auto* g = new bsvg_group();
  g->set_stroke_color("black");
  g->set_stroke_width(stroke_width);
  g->set_stroke_opacity(0.5);

  float end = axes_orig_y_ - h2_y;
  for (float y = axes_orig_y_ - y_inc_scaled; y >= end; y -= y_inc_scaled) {
    auto* line_y = new bsvg_line(axes_orig_x_, y, axes_orig_x_+h2_x, y);
    g->add_element(line_y);
  }
  this->add_element(g);
}

void bsvg_plot::add_line(const std::vector<float>& xs, const std::vector<float>& ys, const std::string& color, float stroke_width)
{
  // scale the points to our plot
  if (xs.size() != ys.size()) {
    std::cout << " Error: bsvg_plot::add_line() - input vectors are not of the same size\n";
    return;
  }
  std::vector<float> xs_copy(xs);
  std::vector<float> ys_copy(ys);
  for (unsigned i = 0; i < xs.size(); i++) {
    xs_copy[i] = xs_copy[i]*scale_factor_ + axes_orig_x_;
    ys_copy[i] = axes_orig_y_ - ys_copy[i]*scale_factor_;
  }

  auto *pl = new bsvg_polyline(xs_copy, ys_copy);
  pl->set_stroke_color(color);
  pl->set_fill_color("none");
  pl->set_stroke_width(stroke_width);

  this->add_element(pl);
}

bsvg_group* bsvg_plot::add_bars_helper(const std::vector<float>& heights, const std::string& color)
{
  auto* g = new bsvg_group();
  g->set_fill_color(color);

  int n = heights.size();
  // we will set bar margins bar_w/3
  float bar_w = h2_x / float(n + float(n + 1)/3.0f);
  float x = axes_orig_x_ + bar_w/3.0f; // left-point of first bar
  for (int i = 0; i < n; i++) {
    float h = heights[i]*scale_factor_;
    auto *r = new bsvg_rectangle(x, axes_orig_y_-h, bar_w, h);
    g->add_element(r);
    x += bar_w/3 + bar_w;  // left-point of next bar, margin + bar width
  }
  return g;
}

bsvg_group* bsvg_plot::add_x_labels_helper(const std::vector<std::string>& x_labels, const std::string& color, bool vertical_labels)
{
  auto* g = new bsvg_group();
  g->set_fill_color(color);

  int n = x_labels.size();
  // we will set bar margins bar_w/3
  float bar_w = h2_x / float(n + float(n + 1)/3.0f);
  float x = axes_orig_x_ + bar_w/2.0f + bar_w/3.0f; // mid-point of first bar
  for (int i = 0; i < n; i++) {
    auto *t = new bsvg_text(x_labels[i]);
    t->set_font_size(font_size_);
    t->set_location(x, axes_orig_y_+margin_);
    if (vertical_labels)
      t->set_rotation(90);
    g->add_element(t);
    x += bar_w/3.0f + bar_w;  // mid-point of next bar, margin + bar width
  }
  return g;
}

//: add equally spaced and equal width bars with the given heights
void bsvg_plot::add_bars(const std::vector<float>& heights, const std::string& color)
{
  bsvg_group* g = add_bars_helper(heights, color);
  this->add_element(g);
}

void bsvg_plot::add_bars(const std::vector<float>& heights, const std::vector<std::string>& x_labels, bool vertical_labels, const std::string& color)
{
  bsvg_group* g = add_bars_helper(heights, color);
  this->add_element(g);
  bsvg_group* tg = add_x_labels_helper(x_labels, color, vertical_labels);
  this->add_element(tg);
}

void bsvg_plot::add_bars(const std::vector<float>& heights, const std::vector<float>& x_labels, bool vertical_labels, const std::string& color)
{
  bsvg_group* g = add_bars_helper(heights, color);
  this->add_element(g);
  std::vector<std::string> x_ls;
  for (float x_label : x_labels) {
    std::stringstream ss; ss << x_label;
    x_ls.push_back(ss.str());
  }
  bsvg_group* tg = add_x_labels_helper(x_ls, color, vertical_labels);
  this->add_element(tg);
}

//: recursive helper to find number of bars
int number_of_bars_helper(const bxml_data_sptr& d)
{
  auto* r_elm = dynamic_cast<bxml_element*>(d.ptr());
  if (!r_elm)
    return 0;

  int cnt = 0;
  for (auto it = r_elm->data_begin(); it != r_elm->data_end(); it++) {
    if ((*it)->type() != bxml_element::ELEMENT)
      continue;
    auto* it_elm = dynamic_cast<bxml_element*>((*it).ptr());
    if (it_elm->name() == "rect")
      cnt++;
    else if (it_elm->name() == "g") {
      cnt += number_of_bars_helper(*it);
    }
  }
  return cnt;
}

int bsvg_plot::number_of_bars()
{
  // get the root
  bxml_element query("svg");
  bxml_data_sptr root = bxml_find_by_name(this->root_element(), query);
  if (!root)
    return -1;
  return number_of_bars_helper(root);
}

//: add bars sequentially with a fixed interval and width.
//  use margin_ as the width of each bar and leave margin_/3 intervals in between
//  the total width of the plot needs to be adjusted during initialization to contain all desired number of bars
int bsvg_plot::add_bar(const float height, const std::string& color)
{
  // first find the next available bar location (count the number of rects in the document)
  int cnt = this->number_of_bars();
  if (cnt < 0) {
    std::cerr << "In bsvg_plot::add_bar() -- problems with the plot document!\n";
    return -1;
  }
  float x = axes_orig_x_ + margin_/3.0f; // left-point of first bar
  x += cnt*(margin_/3.0f + margin_);  // left-point of next bar, margin + bar width
  float h = height*scale_factor_;
  auto *r = new bsvg_rectangle(x, axes_orig_y_-h, margin_, h);
  r->set_fill_color(color);
  this->add_element(r);
  return cnt;
}

int bsvg_plot::add_bar(const float height, const std::string& label, bool vertical_label, const std::string& color)
{
  int cnt = add_bar(height, color);
  if (cnt < 0) {
    std::cerr << "In bsvg_plot::add_bar() -- problems with the plot document!\n";
    return -1;
  }
  float x = axes_orig_x_ + margin_/2.0f + margin_/3.0f; // mid-point of first text
  x += cnt*(margin_/3.0f + margin_);
  auto *t = new bsvg_text(label);
  t->set_font_size(font_size_);
  t->set_location(x, axes_orig_y_+margin_);
  if (vertical_label)
    t->set_rotation(90);
  this->add_element(t);
  return cnt;
}

int bsvg_plot::add_bar(const float height, const float x_label, bool vertical_label, const std::string& color)
{
  std::stringstream ss; ss << x_label;
  return add_bar(height, ss.str(), vertical_label, color);
}

//: add splices for a pie chart
void bsvg_plot::add_splice(float center_x, float center_y, float radius, float start_angle, float end_angle, const std::string& color)
{
  auto* splice_g = new bsvg_splice(center_x, center_y, radius, start_angle, end_angle);
  splice_g->set_fill_color(color);
  splice_g->set_stroke_color("black");
  this->add_element(splice_g);
}

void bsvg_plot::add_splice(float center_x, float center_y, float radius, float start_angle, float end_angle, unsigned red, unsigned green, unsigned blue)
{
  auto* splice_g = new bsvg_splice(center_x, center_y, radius, start_angle, end_angle);
  splice_g->set_fill_color(red, green, blue);
  splice_g->set_stroke_color("black");
  this->add_element(splice_g);
}
