// This is dbsta/vis/bsta_svg_tools.h
#ifndef bsta_svg_tools_h_
#define bsta_svg_tools_h_
//:
// \file
// \brief Visualization of bsta_histograms using bsvg
// \author Ozge C. Ozcanli
// \date April 22, 2009
//
// \verbatim
//
// \endverbatim

#include <vcl_iostream.h>
#include <bsta/bsta_histogram.h>
#include <bxml/bsvg/bsvg_plot.h>
#include <bxml/bxml_write.h>

//: Create SVG document from histogram
// \relates bsta_histogram 
template <class T>
void write_svg(const bsta_histogram<T>& h, const vcl_string& outfile, 
               float width = 600.0f, float height = 600.0f, float margin = 40.0f, int font_size = 30)
{
  bsvg_plot pl(width, height);
  //pl.set_margin(margin);
  //pl.set_font_size(font_size);

  float min = float(h.min());
  float max = float(h.max());
  float delta = float(h.delta());

  //float range_scaled = 

  pl.add_axes(0, 1, 0, 1);
  pl.add_y_increments(0.1f);

  vcl_vector<float> ps;
  vcl_vector<float> x_labels;
  for (unsigned i = 0; i < h.nbins(); i++) {
    ps.push_back(float(h.p(i)));
    x_labels.push_back(min + i*delta);
  }

  pl.add_bars(ps, x_labels, true, "red");

  bxml_write(outfile, pl);
}

#endif // 
