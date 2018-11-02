// This is brl/bbas/bsta/vis/bsta_svg_tools.h
#ifndef bsta_svg_tools_h_
#define bsta_svg_tools_h_
//:
// \file
// \brief Visualization of bsta_histograms using bsvg
// \author Ozge C. Ozcanli
// \date April 22, 2009

#include <iostream>
#include <sstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_joint_histogram.h>
#include <bxml/bsvg/bsvg_plot.h>
#include <bxml/bxml_write.h>
#include <vnl/vnl_math.h>

//: Create SVG document from histogram
// \relatesalso bsta_histogram
template <class T>
void write_svg(const bsta_histogram<T>& h, const std::string& outfile,
               float width = 600.0f, float height = 600.0f, float margin = 40.0f, int font_size = 30)
{
  bsvg_plot pl(width, height);
  if (false) pl.set_margin(margin); // TODO
  if (false) pl.set_font_size(font_size); // TODO
  T min = h.min();
  T max = h.max();
  T delta = h.delta();
  std::cout << "min: " << min << " max: " << max << std::endl;
  T abs_min = T(10);
  T abs_max = T(0);
  for (unsigned a = 0; a < h.nbins(); a++) {
    if (h.p(a) < min) abs_min = h.p(a);
    if (h.p(a) > max) abs_max = h.p(a);
  }
  std::stringstream mss; mss << "abs min: " << abs_min << " max: " << abs_max;
  std::stringstream mss2; mss2 << "ent: " << h.entropy();
  bsvg_text* tmm = new bsvg_text(mss.str());
  tmm->set_location(margin, margin);
  pl.add_element(tmm);
  bsvg_text* tmm2 = new bsvg_text(mss2.str());
  tmm2->set_location(margin, margin+font_size);
  pl.add_element(tmm2);

  float minf = float(min);
  float deltaf = float(delta);

  pl.add_axes(0, 1, 0, 1);
  pl.add_y_increments(0.1f);

  std::vector<float> ps;
  std::vector<float> x_labels;
  for (unsigned i = 0; i < h.nbins(); ++i, minf+=deltaf) {
    ps.push_back(float(h.p(i)));
    x_labels.push_back(minf);
  }

  pl.add_bars(ps, x_labels, true, "red");

  bxml_write(outfile, pl);
}

//: Create a polar style pie chart from the joint histogram, assuming that the first dimension is the angle, and it is in radius
template <class T>
void write_svg_angle_distance(const bsta_joint_histogram<T>& h, const std::string& out_file,
                              float width = 600.0f, float height = 600.0f, float margin = 40.0f, int font_size = 30)
{
  bsvg_plot pl(width, height);
  T mina = h.min_a();
  T maxa = h.max_a();
  T minb = h.min_b();
  T maxb = h.max_b();
  T delta_a = h.delta_a();
  T delta_b = h.delta_b();
  std::cout << "min_a: " << mina << " max_a: " << maxa << " min_b: " << minb << " max_b: " << maxb << std::endl;
  T min = T(10);
  T max = T(0);
  for (unsigned a = 0; a < h.nbins_a(); a++)
    for (unsigned b= 0; b < h.nbins_b(); b++) {
      if (h.p(a,b) < min) min = h.p(a,b);
      if (h.p(a,b) > max) max = h.p(a,b);
    }
  std::stringstream mss; mss << "abs min: " << min << " max: " << max;
  std::stringstream mss2; mss2 << "ent: " << h.entropy();
  bsvg_text* tmm = new bsvg_text(mss.str());
  tmm->set_location(margin, margin);
  pl.add_element(tmm);
  bsvg_text* tmm2 = new bsvg_text(mss2.str());
  tmm2->set_location(margin, margin+font_size);
  pl.add_element(tmm2);


  // scale the distances to the scale of the plot
  float factor = (height - 2*margin) / (2*maxb);

  // now add the splices starting from the outer most bins
  for (float valb = maxb; valb >= minb; valb -= delta_b) {
    float radius = valb*factor; std::stringstream rs; rs << (int)(valb);
    bsvg_text* t = new bsvg_text(rs.str());
    t->set_location(300.0f, 315.0f+radius);
    pl.add_element(t);
    for (float vala = (float)mina; vala < maxa; vala += delta_a) {
      float mag = (h.p(vala+0.005f, valb)/max)*255;
#ifdef DEBUG
      std::cout << ' ' << mag << ' ';
#endif
      unsigned channel = (unsigned)mag;
      pl.add_splice(300.0f, 300.0f, radius, vala, vala+delta_a, 255, 255-channel, 255-channel);
    }
#ifdef DEBUG
    std::cout << '\n';
#endif
  }

  for (float vala = (float)mina; vala < maxa; vala += delta_a) {
    std::stringstream rs; rs << (int)(vala*vnl_math::deg_per_rad);
    bsvg_text* t = new bsvg_text(rs.str());
    float radius = maxb*factor+font_size;
    t->set_location((float)(300.0f+(radius)*std::cos(vala+vnl_math::pi*0.01)), (float)(300.0f+(radius)*-std::sin(vala+vnl_math::pi*0.01)));
    pl.add_element(t);
  }

  if (minb > 0) {
    pl.add_splice(300.0f, 300.0f, factor*minb, 0, float(vnl_math::pi), "gray");
    pl.add_splice(300.0f, 300.0f, factor*minb, -float(vnl_math::pi), 0, "gray");
  }

  bxml_write(out_file, pl);
}

#endif // bsta_svg_tools_h_
