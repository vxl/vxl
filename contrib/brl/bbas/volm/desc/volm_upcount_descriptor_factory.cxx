#include "volm_upcount_descriptor_factory.h"
//:
// \file

void volm_upcount_descriptor_factory::set_count(unsigned const& bin, unsigned char const& magnitude)
{
  if (bin<nbins_) {
    unsigned char temp = h_.counts(bin);
    h_.set_count(bin, temp+magnitude);
  }
}

void volm_upcount_descriptor_factory::set_count(double const& dist, double const& height,
                                                depth_map_region::orientation const& orient, unsigned const& land,
                                                unsigned char const& mag)
{
  unsigned bin_idx = this->bin_index(dist, height, orient, land);
  this->set_count(bin_idx, mag);
}

void volm_upcount_descriptor_factory::set_count(unsigned const& dist_idx, unsigned const& height_idx,
                                                unsigned const& orient_idx, unsigned const& land_idx,
                                                unsigned char const& mag)
{
  unsigned bin_idx = this->bin_index(dist_idx, height_idx, orient_idx, land_idx);
  this->set_count(bin_idx, mag);
}