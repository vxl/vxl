#include <iostream>
#include <algorithm>
#include "volm_desc_ex.h"
//:
// \file
#include <vsl/vsl_vector_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

unsigned locate_idx(double const& target, std::vector<double> const& arr)
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


volm_desc_ex::volm_desc_ex(depth_map_scene_sptr const& dms,
                           std::vector<double> const& radius,
                           unsigned const& norients,
                           unsigned const& nlands,
                           unsigned char const& initial_mag)
{
  name_ = "existence descriptor";
  // create a histogram based on given parameters
  norients_ = norients;
  nlands_ = nlands;
  if (radius.empty())
    radius_.push_back(1.0);
  else
    radius_ = radius;
  // sort the radius to ensure the bin order
  std::sort(radius_.begin(), radius_.end());
  ndists_ = (unsigned)radius.size() + 1;
  nbins_ = ndists_ * norients_ * nlands_;

  h_.resize(nbins_);
#if 0
  unsigned char range;
  if (nbins_ > 255)
    range = 255;
  else
    range = (unsigned char)nbins_;
#endif
  this->initialize_bin(initial_mag);

  // ingest depth_map_scene into histogram
  // sky (orient id = 2, land id = 0, distant = 1E6)
  if (!dms->sky().empty()) {
    std::vector<depth_map_region_sptr> sky = dms->sky();
    for (auto & s_idx : sky)
      this->set_count(s_idx->min_depth(), s_idx->orient_type(), s_idx->land_id(), (unsigned char)1);
  }
  // ground
  if (!dms->ground_plane().empty()) {
    std::vector<depth_map_region_sptr> grd = dms->ground_plane();
    for (auto & g_idx : grd)
      this->set_count(g_idx->min_depth(), g_idx->orient_type(), g_idx->land_id(), (unsigned char)1);
  }
  // other objects
  if (!dms->scene_regions().empty()) {
    std::vector<depth_map_region_sptr> obj = dms->scene_regions();
    for (auto & o_idx : obj)
      this->set_count(o_idx->min_depth(), o_idx->orient_type(), o_idx->land_id(), (unsigned char)1);
  }
}

volm_desc_ex::volm_desc_ex(std::vector<unsigned char> const& index_dst,
                           std::vector<unsigned char> const& index_combined,
                           std::vector<double> depth_interval,
                           std::vector<double> const& radius,
                           unsigned const& norients,
                           unsigned const& nlands,
                           unsigned char const& initial_mag)
{
  name_ = "existence descriptor";
  // create a histogram based on given parameters
  norients_ = norients;
  nlands_ = nlands;
  if (radius.empty())
    radius_.push_back(1.0);
  else
    radius_ = radius;
  // sort the radius to ensure the bin order
  std::sort(radius_.begin(), radius_.end());
  ndists_ = (unsigned)radius.size() + 1;
  nbins_ = ndists_ * norients_ * nlands_;
  h_.resize(nbins_);
  this->initialize_bin(initial_mag);

  // ingest index to histogram
  auto nrays = (unsigned)index_dst.size();
  assert( nrays == index_combined.size() && " in volm_desx_ex, dist/land/orient indice has different number of rays");
  for (unsigned r_idx = 0; r_idx < nrays; r_idx++) {
    if (index_dst[r_idx] == 253 || index_combined[r_idx] == 253)         // invalid
      continue;
    else if (index_dst[r_idx] == 254 && index_combined[r_idx] == 254) {  // sky
      this->set_count(1.0E6, depth_map_region::INFINT, 0, (unsigned char)1);
    }
    else {
      unsigned char orientation_value, label_value;
      volm_io_extract_values(index_combined[r_idx], orientation_value, label_value);
      double dist;
      if (index_dst[r_idx] < depth_interval.size())
        dist = depth_interval[index_dst[r_idx]];
      else
        dist = depth_interval[depth_interval.size()-1];
      if (label_value < nlands_)
        this->set_count(dist, (unsigned)orientation_value, (unsigned)label_value, (unsigned char)1);
    }
  }
}

void volm_desc_ex::initialize_bin(unsigned char const&  /*mag*/)
{
  for (unsigned bin_id = 0; bin_id < nbins_; bin_id++)
    h_[bin_id] = (unsigned char)0;
}


unsigned volm_desc_ex::bin_index(unsigned const& dist_idx, unsigned const& orient_idx, unsigned const& land_idx) const
{
  // loop order
  //  distance
  //   orientation
  //    land

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
  bin_idx += (nlands_*norients_)*dist_idx;
  return bin_idx;
}

unsigned volm_desc_ex::bin_index(double const& dist, depth_map_region::orientation const& orient, unsigned const& land) const
{
  assert(land < nlands_ && "land type is beyond defined land type in volm_descriptor");
  unsigned dist_idx = locate_idx(dist, radius_);
  return this->bin_index(dist_idx, (unsigned)orient, land);
}

unsigned volm_desc_ex::bin_index(double const& dist, unsigned const& orient, unsigned const& land) const
{
  unsigned dist_idx = locate_idx(dist, radius_);
  return this->bin_index(dist_idx, orient, land);
}

void volm_desc_ex::set_count(unsigned const& bin, unsigned char const& count)
{
  if (bin < nbins_) h_[bin] = count;
}

void volm_desc_ex::set_count(unsigned const& dist_idx, unsigned const& orient_idx, unsigned const& land_idx,
                             unsigned char const& count)
{
  unsigned bin = this->bin_index(dist_idx, orient_idx, land_idx);
  this->set_count(bin, count);
}

void volm_desc_ex::set_count(double const& dist, unsigned const& orient, unsigned const& land,
                             unsigned char const& count)
{
  unsigned bin = this->bin_index(dist, orient, land);
  this->set_count(bin, count);
}

void volm_desc_ex::set_count(double const& dist, depth_map_region::orientation const& orient, unsigned const& land,
                             unsigned char const& count)
{
  unsigned bin = this->bin_index(dist, orient, land);
  this->set_count(bin, count);
}

void volm_desc_ex::print() const
{
  std::cout << "descriptor name: " << name_ << '\n';
  std::cout << "number of depth bins: " << ndists_ << '\n'
     << "radius interval: ";
  for (double radiu : radius_)
    std::cout << radiu << ' ';
  std::cout << '\n'
           << "number of orientation bins: " << norients_ << '\n'
           << "number of land bins: " << nlands_ << '\n'
           << "histogram info:\n";
  std::cout << "number of total bins:" << nbins_ << '\n'
           << "counts: ";
  for (unsigned i = 0; i < nbins_; i++)
    std::cout << (int)h_[i] << ' ';
  std::cout << std::endl;
}

float volm_desc_ex::similarity(volm_desc_sptr other)
{
  if (nbins_ != other->nbins())
    return 0.0f;
  // calcualte the intersection
  float intersec = 0.0f;
  for (unsigned idx = 0; idx < nbins_; idx++) {
    intersec += (float)std::min(this->count(idx), other->count(idx));
  }
  // normalize by current histogram area
  return intersec/this->get_area();
}

void volm_desc_ex::b_write(vsl_b_ostream& os)
{
  unsigned ver = this->version();
  vsl_b_write(os, ver);
  vsl_b_write(os, name_);
  vsl_b_write(os, nbins_);
  vsl_b_write(os, ndists_);
  vsl_b_write(os, norients_);
  vsl_b_write(os, nlands_);
  vsl_b_write(os, radius_);
  vsl_b_write(os, h_);
}

void volm_desc_ex::b_read(vsl_b_istream& is)
{
  unsigned ver;
  vsl_b_read(is, ver);
  if (ver == 1) {
    vsl_b_read(is, name_);
    vsl_b_read(is, nbins_);
    vsl_b_read(is, ndists_);
    vsl_b_read(is, norients_);
    vsl_b_read(is, nlands_);
    vsl_b_read(is, radius_);
    h_.resize(nbins_);
    vsl_b_read(is, h_);
  }
  else {
    std::cout << "volm_descriptor -- unknown binary io version " << ver << '\n';
    return;
  }
}
