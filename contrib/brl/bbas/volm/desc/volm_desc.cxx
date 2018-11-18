#include "volm_desc.h"
//:
// \file
#include <bsta/vis/bsta_svg_tools.h>
#include <vsl/vsl_vector_io.h>

float volm_desc::similarity(volm_desc_sptr  /*other*/)
{
  return 0.0f;
}

unsigned int volm_desc::get_area()
{
  unsigned area = 0;
  for (unsigned int i=0; i<nbins_; i++)
  { area = area + h_[i]; }
  return area;
}

void volm_desc::visualize(const std::string& outfile, unsigned char const& y_max) const
{
  // use bsvg_plot to visualize the bin
  // define the width and height from descriptor
  float margin = 40.0f;
  int font_size_label = 10;
  float stroke_width = 2.0f;
  float width = (float)nbins_ + 2*margin;
  float height = 255.0f;
  bsvg_plot pl(width, height);
  pl.set_font_size(font_size_label);

  std::stringstream msg1;
  msg1 << "nbins : " << nbins_;

  // add text msg
  int font_size_text = 15;
  bsvg_text* tmm1 = new bsvg_text(msg1.str());
  tmm1->set_font_size(font_size_text);
  tmm1->set_location(margin+(float)font_size_text, margin*0.5f);
  pl.add_element(tmm1);

  // add axes
  pl.add_axes(0.0f, (float)nbins_, 0.0f, (float)y_max, false, stroke_width);

  std::vector<float> ps;
  std::vector<float> x_labels;
  for (unsigned i = 0; i < nbins_; i++) {
    ps.push_back(float(h_[i]));
    x_labels.push_back(float(i));
  }
  pl.add_bars(ps, "red");

  bxml_write(outfile, pl);
}


void volm_desc::print() const
{
  std::cout << "descriptor name: " << name_ << '\n';
  std::cout << "number of total bins:" << nbins_ << '\n'
           << "counts: ";
  for (unsigned i = 0; i < nbins_; i++)
    std::cout << (int)h_[i] << ' ';
  std::cout << std::endl;
}

void volm_desc::b_write(vsl_b_ostream& os)
{
  unsigned ver = this->version();
  vsl_b_write(os, ver);
  vsl_b_write(os, name_);
  vsl_b_write(os, nbins_);
  vsl_b_write(os, h_);
}

void volm_desc::b_read(vsl_b_istream& is)
{
  unsigned ver;
  vsl_b_read(is, ver);
  if (ver == 1) {
    vsl_b_read(is, name_);
    vsl_b_read(is, nbins_);
    h_.resize(nbins_);
    vsl_b_read(is, h_);
  }
  else {
    std::cout << "volm_descriptor -- unknown binary io version " << ver << '\n';
    return;
  }
}

void volm_desc::get_char_array(std::vector<unsigned char>& values) const
{
  values.resize(nbins_);
  values = h_;
}
