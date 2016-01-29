#include "volm_desc_ex_2d.h"
// :
// \file
#include <vsl/vsl_vector_io.h>
#include <vcl_algorithm.h>

unsigned volm_desc_ex_2d::locate_idx(double const& target, vcl_vector<double> const& arr) const
{
  // simple binary search to locate the dist_id and height_id given dist value or height values
  assert(target >= 0 && "given object dists/height is smaller than 0");
  if( arr.back() < target )
    {
    return (unsigned)arr.size();
    }
  unsigned low = 0, high = (unsigned)arr.size();
  while( low != high )
    {
    unsigned mid = (low + high) / 2;
    if( arr[mid] <= target )
      {
      low = mid + 1;
      }
    else
      {
      high = mid;
      }
    }

  return low;
}

volm_desc_ex_2d::volm_desc_ex_2d(vcl_vector<double> const& radius,
                                 double const& h_width, double const& h_inc,
                                 unsigned const& nlands,
                                 unsigned char const& initial_mag)
{
  name_ = "existence 2d descriptor";
  // create a histogram based on given parameter
  nlands_ = nlands;
  if( radius.empty() )
    {
    radius_.push_back(1.0);
    }
  else
    {
    radius_ = radius;
    }
  // sort the radius to ensure the bin order
  vcl_sort(radius_.begin(), radius_.end() );
  ndists_ = (unsigned)radius.size() + 1;
  // set the heading bins
  h_width_ = h_width;
  h_inc_ = h_inc;
  if( h_width_ < h_inc_ ) {h_inc_ = h_width_; }
  if( h_inc_ == 360.0 ) {nheadings_ = 1; }
  else {nheadings_ = vcl_ceil(360.0 / h_inc_); }
  heading_intervals_.clear();
  for( unsigned hidx = 0; hidx < nheadings_; hidx++ )
    {
    double s = hidx * h_inc_;  double e = s + h_width_;
    heading_intervals_.push_back(vcl_pair<double, double>(s, e) );
    }

  // construct the histogram
  nbins_ = ndists_ * nlands_ * nheadings_;
  h_.resize(nbins_);
  this->initialize_bin(nbins_);
}

void volm_desc_ex_2d::initialize_bin(unsigned char const& mag)
{
  for( unsigned bin_id = 0; bin_id < nbins_; bin_id++ )
    {
    h_[bin_id] = (unsigned char)0;
    }
}

unsigned volm_desc_ex_2d::bin_index(unsigned const& dist_idx, unsigned const& land_idx,
                                    unsigned const& heading_idx) const
{
  // loop order
  //  heading
  //    distance
  //      land
  assert(land_idx < nlands_ && "land type index is beyond defined land type in volm_ex_2d_descriptor");
  assert(dist_idx < ndists_ && "distance index is beyond defined maximum distance index in volm_ex_2d_descriptor");
  assert(heading_idx < nheadings_ && "heading index is beyond defined maximum heading index in volm_ex_2d_descriptor");

  unsigned bin_idx = land_idx + nlands_ * dist_idx;
  bin_idx += (nlands_ * ndists_) * heading_idx;
  return bin_idx;
}

vcl_vector<unsigned> volm_desc_ex_2d::bin_index(double const& distance, unsigned const& land_type,
                                                double const& heading) const
{
  // obtain distance index
  unsigned dist_idx = this->locate_idx(distance, radius_);
  // obtain heading index
  double heading_value = heading;

  while( heading_value >= heading_intervals_[heading_intervals_.size() - 1].second )
    {
    heading_value -= 360.0;
    }

  vcl_vector<unsigned> heading_indice;
  for( unsigned hidx = 0; hidx < heading_intervals_.size(); hidx++ )
    {
    if( heading_value >= heading_intervals_[hidx].first && heading_value < heading_intervals_[hidx].second )
      {
      heading_indice.push_back(hidx);
      }
    }
  vcl_vector<unsigned> bin_indice;
  for( vcl_vector<unsigned>::iterator it = heading_indice.begin(); it != heading_indice.end(); ++it )
    {
    bin_indice.push_back(this->bin_index(dist_idx, land_type, *it) );
    }
  return bin_indice;
}

void volm_desc_ex_2d::set_count(unsigned const& bin, unsigned char const& count)
{
  // Note for same
  if( bin < nbins_ ) {h_[bin] += count; }
}

void volm_desc_ex_2d::set_count(unsigned const& dist_idx, unsigned const& land_idx, unsigned const& heading_idx,
                                unsigned char const& count)
{
  unsigned bin = this->bin_index(dist_idx, land_idx, heading_idx);

  this->set_count(bin, count);
}

void volm_desc_ex_2d::set_count(double const& distance, unsigned const& land_id, double const& heading,
                                unsigned char const& count)
{
  vcl_vector<unsigned> bins = this->bin_index(distance, land_id, heading);
  for( vcl_vector<unsigned>::iterator vit = bins.begin();  vit != bins.end();  ++vit )
    {
    this->set_count(*vit, count);
    }
}

float volm_desc_ex_2d::similarity(volm_desc_sptr other)
{
  if( nbins_ != other->nbins() )
    {
    return 0.0f;
    }
  // calculate the intersection
  float intersec = 0.0f;
  for( unsigned idx = 0; idx < nbins_; idx++ )
    {
    intersec += (float)vcl_min(this->count(idx), other->count(idx) );
    }
  // normalize by current histogram area
  return intersec / this->get_area();
}

void volm_desc_ex_2d::print() const
{
  vcl_cout << "descriptor name: " << name_ << '\n';
  vcl_cout << "number of depth bins: " << ndists_ << '\n'
           << "radius interval: ";
  for( unsigned ridx = 0; ridx < radius_.size(); ridx++ )
    {
    vcl_cout << radius_[ridx] << ' ';
    }
  vcl_cout << "\nnumber of heading bins: " << nheadings_ << ", heading width: " << h_width_
           << ", heading incremental: " << h_inc_ << '\n';
  for( unsigned hidx = 0; hidx < heading_intervals_.size(); hidx++ )
    {
    vcl_cout << '[' << heading_intervals_[hidx].first << ',' << heading_intervals_[hidx].second << "] ";
    }
  vcl_cout << '\n'
           << "number of land bins: " << nlands_ << '\n';
  vcl_cout << "number of total bins:" << nbins_ << '\n';
  if( nbins_ < 100 )
    {
    vcl_cout << "counts: ";
    for( unsigned i = 0; i < nbins_; i++ )
      {
      vcl_cout << (int)h_[i] << ' ';
      }
    vcl_cout << vcl_endl;
    }
}

void volm_desc_ex_2d::b_write(vsl_b_ostream& os)
{
  unsigned ver = this->version();

  vsl_b_write(os, ver);
  vsl_b_write(os, name_);
  vsl_b_write(os, nbins_);
  vsl_b_write(os, ndists_);
  vsl_b_write(os, nlands_);
  vsl_b_write(os, nheadings_);
  vsl_b_write(os, h_width_);
  vsl_b_write(os, h_inc_);
  vsl_b_write(os, radius_);
  vsl_b_write(os, h_);
}

void volm_desc_ex_2d::b_read(vsl_b_istream& is)
{
  unsigned ver;

  vsl_b_read(is, ver);
  if( ver == this->version() )
    {
    vsl_b_read(is, name_);
    vsl_b_read(is, nbins_);
    vsl_b_read(is, ndists_);
    vsl_b_read(is, nlands_);
    vsl_b_read(is, nheadings_);
    vsl_b_read(is, h_width_);
    vsl_b_read(is, h_inc_);
    vsl_b_read(is, radius_);
    h_.resize(nbins_);
    vsl_b_read(is, h_);
    heading_intervals_.clear();
    for( unsigned hidx = 0; hidx < nheadings_; hidx++ )
      {
      double s = hidx * h_inc_;  double e = s + h_width_;
      heading_intervals_.push_back(vcl_pair<double, double>(s, e) );
      }
    }
  else
    {
    vcl_cout << "volm_desc_ex_2d descriptor -- unknown binary io version: " << ver << '\n';
    return;
    }
}
