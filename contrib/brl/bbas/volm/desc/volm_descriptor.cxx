// This is brl/bbas/volm/desc/volm_descriptor.cxx
#include "volm_descriptor.h"
#include <vcl_algorithm.h>
#include <vsl/vsl_vector_io.h>
#include <bsta/vis/bsta_svg_tools.h>

unsigned locate_idx(double const& target, vcl_vector<double> const& arr)
{
  // simple binary search to locate the dist_id and height_id given dist value or height values
  assert(target >= 0 && "given object dists/height is smaller than 0");
  if (arr.back() < target)
    return (unsigned)arr.size();
  unsigned low = 0, high = (unsigned)arr.size();
  while ( low != high) {
    unsigned mid = (low+high)/2;
    if ( arr[mid] <= target )
      low = mid+1;
    else
      high = mid;
  }
  return low;
}

volm_descriptor::volm_descriptor(vcl_vector<double> const& radius,
                                 vcl_vector<double> const& height,
                                 unsigned const&   norients,
                                 unsigned const&     nlands,
                                 unsigned char const& initial_mag)
  : norients_(norients), nlands_(nlands)
{
  if (radius.empty())
    radius_.push_back(1.0);
  else
    radius_ = radius;
  if (height.empty())
    height_.push_back(1.0);
  else
    height_ = height;
  // sort the radius and height vector to ensure the bin order
  vcl_sort(radius_.begin(), radius_.end());
  vcl_sort(height_.begin(), height_.end());

  ndepths_ = (unsigned)radius_.size() + 1;
  nheights_ = (unsigned)height_.size() + 1;
  // create a zero delta(interval) histogram
  nbins_ = ndepths_ * nheights_ * norients_ * nlands_;
  unsigned char range;
  if (nbins_ > 255)
    range = 255;
  else 
    range = (unsigned char) nbins_;
  h_ = bsta_histogram<unsigned char>(range, nbins_);
  this->initialize_bin(initial_mag);
}

void volm_descriptor::initialize_bin(unsigned char const& mag)
{
  for (unsigned bin_id = 0; bin_id < nbins_; bin_id++)
    h_.set_count(bin_id, mag);
}

unsigned volm_descriptor::bin_index(unsigned const& dist_idx, unsigned const& height_idx,
                                    unsigned const& orient_idx, unsigned const& land_idx)
{
  // loop order
  //  distance
  //   height
  //    orientation
  //     land
  
  // transfer orientation index, (orientation index is defined by the depth_map_region::orientation)
  unsigned o_idx;
  o_idx = orient_idx;
  if (orient_idx == depth_map_region::SLANTED_LEFT || orient_idx == depth_map_region::SLANTED_RIGHT)
    o_idx = depth_map_region::FRONT_PARALLEL;
  if (orient_idx == depth_map_region::POROUS || orient_idx == depth_map_region::INFINT || orient_idx == depth_map_region::NON_PLANAR)
    o_idx = (unsigned char)(norients_-1);
  assert(o_idx < norients_ && "orientation type is not recognized in volm_descriptor");
  assert(land_idx < nlands_ && "land type is beyond defined land type in volm_descriptor");
  unsigned bin_idx = land_idx + nlands_*o_idx;
  bin_idx += (nlands_*norients_)*height_idx + (nlands_*norients_*nheights_)*dist_idx;
  return bin_idx;
}

unsigned volm_descriptor::bin_index(double const& dist, double const& height,
                                    depth_map_region::orientation const& orient, unsigned const& land)
{
  assert(land < nlands_ && "land type is beyond defined land type in volm_descriptor");
  unsigned dist_idx = locate_idx(dist, radius_);
  unsigned height_idx = locate_idx(height, height_);
  return this->bin_index(dist_idx, height_idx, (unsigned)orient, land);
}

void volm_descriptor::set_count(unsigned const& dist_idx, unsigned const& height_idx,
                                unsigned const& orient_idx, unsigned const& land_idx,
                                unsigned char const& count)
{
  unsigned bin_idx = this->bin_index(dist_idx, height_idx, orient_idx, land_idx);
  if (bin_idx < nbins_)
    h_.set_count(bin_idx, count);
  this->set_count(bin_idx, count);
}

void volm_descriptor::set_count(double const& dist, double const& height,
                                depth_map_region::orientation const& orient, unsigned const& land,
                                unsigned char const& count)
{
  unsigned bin_idx = this->bin_index(dist, height, orient, land);
  this->set_count(bin_idx, count);
}

void volm_descriptor::upcount(unsigned const& dist_idx, unsigned const& height_idx,
                              unsigned const& orient_idx, unsigned const& land_idx,
                              unsigned char const& mag)
{
  unsigned bin_idx = this->bin_index(dist_idx, height_idx, orient_idx, land_idx);
  this->upcount(bin_idx, mag);
}

void volm_descriptor::upcount(double const& dist, double const& height,
                              depth_map_region::orientation const& orient, unsigned const& land,
                              unsigned char const& mag)
{
  unsigned bin_idx = this->bin_index(dist, height, orient, land);
  this->upcount(bin_idx, mag);
}

void volm_descriptor::b_write(vsl_b_ostream& os)
{
  unsigned ver = this->version();
  vsl_b_write(os, ver);
  vsl_b_write(os, nbins_);
  vsl_b_write(os, ndepths_);
  vsl_b_write(os, nheights_);
  vsl_b_write(os, norients_);
  vsl_b_write(os, nlands_);
  vsl_b_write(os, radius_);
  vsl_b_write(os, height_);
  for(unsigned bin = 0; bin < nbins_; bin++) {
    unsigned char tmp = h_.counts(bin);
    vsl_b_write(os, h_.counts(bin));
  }
}

void volm_descriptor::b_read(vsl_b_istream& is)
{
  unsigned ver;
  vsl_b_read(is, ver);
  if (ver == 1) {
    vsl_b_read(is, nbins_);
    vsl_b_read(is, ndepths_);
    vsl_b_read(is, nheights_);
    vsl_b_read(is, norients_);
    vsl_b_read(is, nlands_);
    vsl_b_read(is, radius_);
    vsl_b_read(is, height_);
    unsigned char range;
    if (nbins_ > 255) range = 255;
    else range = (unsigned char)nbins_;
    h_ = bsta_histogram<unsigned char>(range, nbins_);
    for (unsigned bin = 0; bin < nbins_; bin++) {
      unsigned char count;
      vsl_b_read(is, count);
      h_.set_count(bin, count);
    }
  }
  else {
    vcl_cout << "volm_descriptor -- unknown binary io version " << ver << '\n';
    return;
  }
}

void vsl_b_write(vsl_b_ostream& os, const volm_descriptor* desc_ptr)
{
  if (desc_ptr == 0) {
    vsl_b_write(os,false);
    return;
  }
  else {
    vsl_b_write(os, true);
    volm_descriptor* ptr_non_const = const_cast<volm_descriptor*>(desc_ptr);
    ptr_non_const->b_write(os);
  }
}

void vsl_b_read(vsl_b_istream &is, volm_descriptor*& desc_ptr)
{
  bool valid_ptr = false;
  vsl_b_read(is, valid_ptr);
  if (valid_ptr) {
    desc_ptr = new volm_descriptor();
    desc_ptr->b_read(is);
    return;
  }
}

void vsl_b_write(vsl_b_ostream& os, const volm_descriptor_sptr& desc_sptr)
{
  volm_descriptor* dp = desc_sptr.ptr();
  vsl_b_write(os,dp);
}

void vsl_b_read(vsl_b_istream &is, volm_descriptor_sptr& desc_sptr)
{
  volm_descriptor* dp = 0;
  vsl_b_read(is, dp);
  desc_sptr = dp;
}

void volm_descriptor::print() const
{
  vcl_cout << "number of depth bins: " << ndepths_ << '\n'
     << "radius interval: ";
  for (unsigned ridx = 0; ridx < radius_.size(); ridx++)
    vcl_cout << radius_[ridx] << ' ';
  vcl_cout << '\n'
     << "number of height bins: " << nheights_ << '\n'
     << "height interval: ";
  for (unsigned hidx = 0; hidx < height_.size(); hidx++)
    vcl_cout << height_[hidx] << ' ';
  vcl_cout << '\n'
     << "number of orientation bins: " << norients_ << '\n'
     << "number of land bins: " << nlands_ << '\n'
     << "histogram info:\n";
  vcl_cout << "number of total bins:" << h_.nbins() << '\n'
           << "min: " << (int)h_.min() << '\n'
           << "max: " << (int)h_.max() << '\n'
           << "delta: " << (int)h_.delta() << '\n'
           << "counts: ";
  for (unsigned i = 0; i < h_.nbins(); i++)
    vcl_cout << (int)h_.counts(i) << ' ';
  vcl_cout << vcl_endl;
}

void volm_descriptor::visualize(vcl_string outfile, unsigned char const& y_max) const
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
  msg1 << "nbins : " << nbins_ << " ( " << ndepths_ << " depths, "
       << nheights_ << " heights, " << norients_ << " orientations, "
       << nlands_ << " land type)";
  vcl_stringstream msg2;
  msg2 << "radius = [";
  for (vcl_vector<double>::const_iterator vit = radius_.begin();
       vit != radius_.end(); ++vit)
    msg2 << *vit << ',';
  msg2 << "],  heights = [";
  for (vcl_vector<double>::const_iterator vit = height_.begin();
       vit != height_.end(); ++vit)
    msg2 << *vit << ',';
  msg2 << ']';

  // add text msg
  int font_size_text = 15;
  bsvg_text* tmm1 = new bsvg_text(msg1.str());
  bsvg_text* tmm2 = new bsvg_text(msg2.str());
  tmm1->set_font_size(font_size_text);
  tmm2->set_font_size(font_size_text);
  tmm1->set_location(margin+(float)font_size_text, margin*0.5f);
  tmm2->set_location(margin+(float)font_size_text, margin*0.5f+(float)font_size_text);
  pl.add_element(tmm1);
  pl.add_element(tmm2);

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
