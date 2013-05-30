#include "volm_desc.h"
//:
// \file
#include <bsta/vis/bsta_svg_tools.h>

float volm_desc::similarity(volm_desc_sptr other)
{
  return 0.0f;
}

void volm_desc::visualize(vcl_string outfile, unsigned char const& y_max) const
{
  // use bsvg_plot to visualize the bin
  // define the width and height from descriptor
  float margin = 40.0f;
  int font_size_label = 10;
  float stroke_width = 2.0f;
  float width = (float)nbins_ + 2*margin;
  float height = (float)h_.max();
  bsvg_plot pl(width, height);
  pl.set_font_size(font_size_label);

  vcl_stringstream msg1;
  msg1 << "nbins : " << nbins_;

  // add text msg
  int font_size_text = 15;
  bsvg_text* tmm1 = new bsvg_text(msg1.str());
  tmm1->set_font_size(font_size_text);
  tmm1->set_location(margin+(float)font_size_text, margin*0.5f);
  pl.add_element(tmm1);

  // add axes
  pl.add_axes(0.0f, (float)nbins_, 0.0f, (float)y_max, false, stroke_width);

  vcl_vector<float> ps;
  vcl_vector<float> x_labels;
  for (unsigned i = 0; i < nbins_; i++) {
    ps.push_back(float(h_.counts(i)));
    x_labels.push_back(float(i));
  }
  pl.add_bars(ps, "red");

  bxml_write(outfile, pl);
}

