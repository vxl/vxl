// This is brl/bbas/bxml/bsvg/tests/test_svg.cxx
//:
// \file
// \brief Various tests for SVG library
// \author Ozge C. Ozcanli (Brown)
// \date   April 21, 2009
//
#include <testlib/testlib_test.h>
#include <bxml/bsvg/bsvg_document.h>
#include <bxml/bsvg/bsvg_element.h>
#include <bxml/bsvg/bsvg_plot.h>
#include <bxml/bxml_find.h>
#include <bxml/bxml_write.h>

bsvg_document make_simple_doc()
{
  bsvg_document doc(200, 200);
  doc.add_description("test SVG");

  bsvg_text* t = new bsvg_text("some message");
  t->set_font_size(15);
  t->set_location(10, 10);
  t->set_rotation(90);

  doc.add_element(t);

  auto* g = new bsvg_group();
  g->set_fill_color("red");
  g->set_stroke_color("black");
  //g->set_rotation(90);

  auto* e1 = new bsvg_ellipse(25, 10);
  e1->set_location(50, 20);
  e1->set_rotation(45);
  auto* e2 = new bsvg_ellipse(25, 10);
  e2->set_location(100, 20);
  e2->set_rotation(90);

  auto* e3 = new bsvg_ellipse(25, 25);
  e3->set_location(150, 20);

  g->add_element(e1);
  g->add_element(e2);
  g->add_element(e3);

  doc.add_element(g);

  auto* l = new bsvg_line(90, 115, 125, 115);
  l->set_stroke_color("black");
  l->set_stroke_width(10);

  doc.add_element(l);

  auto* r = new bsvg_rectangle(100, 100, 15, 30);
  r->set_fill_color("blue");
  r->set_stroke_color("black");
  r->set_fill_opacity(0.5f);

  doc.add_element(r);

  return doc;
}

bsvg_plot make_simple_plot()
{
  bsvg_plot p(1200, 600);
  p.set_margin(40);
  p.set_font_size(30);
  //bsvg_plot p(600, 300, 0, 0, 300, 150);
  p.add_axes(0, 1, 0, 1);
  p.add_title("ROC Plot");
  //p.add_x_increments(0.1f);
  p.add_y_increments(0.1f);

  float fpr[] = {0.1f, 0.2f, 0.4f, 0.8f};
  float tpr[] = {0.4f, 0.8f, 0.9f, 0.99f};

  std::vector<float> xs, ys;
  for (unsigned i = 0; i < 4; i++) {
    xs.push_back(fpr[i]); ys.push_back(tpr[i]);
  }

  p.add_line(xs, ys, "red");
  return p;
}

bsvg_plot make_simple_histogram()
{
  bsvg_plot p(1200, 600);
  p.set_margin(40);
  p.set_font_size(30);
  //bsvg_plot p(600, 300, 0, 0, 300, 150);
  p.add_axes(0, 1, 0, 1);
  p.add_title("Bar Plot");
  p.add_y_increments(0.1f);

  float heights[] = {0.1f, 0.2f, 0.0f, 0.8f, 0.9f, 0.0f, 0.3f};

  std::vector<float> hs; std::vector<float> labels;
  for (unsigned i = 0; i < 7; i++) {
    hs.push_back(heights[i]);
    labels.push_back(float(i));
  }
  p.add_bars(hs, labels, true, "red");
  return p;
}

bsvg_plot make_simple_pie_chart()
{
  bsvg_plot p(500, 500);
  p.set_margin(40);
  p.set_font_size(30);
  double pi = 3.14;
  p.add_splice(200.0f, 200.0f, 180.0f, 0.0f, (float)(pi*(80.0/180.0)), "red");
  p.add_splice(200.0f, 200.0f, 180.0f, (float)(pi*(100.0/180.0)), (float)(pi*(150.0/180.0)), "blue");
  p.add_splice(200.0f, 200.0f, 180.0f, (float)(pi*(200.0/180.0)), (float)(pi*(270.0/180.0)), 255, 255, 0);
  p.add_splice(200.0f, 200.0f, 90.0f, (float)(pi*(200.0/180.0)), (float)(pi*(270.0/180.0)), 255, 255, 200);
  p.add_splice(200.0f, 200.0f, 90.0f, (float)(pi*(-180.0/180.0)), (float)(pi*(-160.0/180.0)), 255, 100, 200);
  return p;
}

static void test_svg()
{
  bsvg_document doc = make_simple_doc();
  std::string out_file = "./test.svg";
  bxml_write(out_file, doc);

  bsvg_plot p = make_simple_plot();
  out_file = "./test_plot.svg";
  bxml_write(out_file, p);

  bsvg_plot h = make_simple_histogram();
  out_file = "./test_histogram.svg";
  bxml_write(out_file, h);
  TEST("testing number of bars", h.number_of_bars(), 7);

  bsvg_plot pie = make_simple_pie_chart();
  out_file = "./test_pie_chart.svg";
  bxml_write(out_file, pie);
}

TESTMAIN( test_svg );
