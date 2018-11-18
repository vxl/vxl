// This is brl/bseg/bapl/bapl_mi_matcher_params.cxx
#include <iostream>
#include "bapl_mi_matcher_params.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define BAPL_MIN_SCALE 1.0
#define BAPL_MAX_SCALE 10.0
#define BAPL_MIN_TRANS 0.0
#define BAPL_MAX_TRANS 500.0
#define BAPL_MIN_ROT   0.0
#define BAPL_MAX_ROT   180.0
#define BAPL_MIN_SHEAR 0.0
#define BAPL_MAX_SHEAR 89.9


//: Constructor
bapl_mi_matcher_params::bapl_mi_matcher_params(unsigned num_samples,
                                               double max_scale, double max_radius,
                                               double max_rotation, double max_shear)
 : num_samples_(num_samples), max_sx_(max_scale), max_sy_(max_scale), max_tx_(max_radius),
   max_ty_(max_radius), max_rotation_ang_(max_rotation), max_shear_ang_(max_shear)
{}

//: Check the value of each member variable to see if it is in range
bool
bapl_mi_matcher_params::valid_range()
{
  return max_sx_ >= BAPL_MIN_SCALE  &&  max_sx_ <= BAPL_MAX_SCALE  &&
         max_sy_ >= BAPL_MIN_SCALE  &&  max_sy_ <= BAPL_MAX_SCALE  &&
         max_tx_ >= BAPL_MIN_TRANS  &&  max_tx_ <= BAPL_MAX_TRANS  &&
         max_ty_ >= BAPL_MIN_TRANS  &&  max_ty_ <= BAPL_MAX_TRANS  &&
         max_rotation_ang_ >= BAPL_MIN_ROT  &&  max_rotation_ang_ <= BAPL_MAX_ROT  &&
         max_shear_ang_ >= BAPL_MIN_SHEAR  &&  max_shear_ang_ <= BAPL_MAX_SHEAR;
}


//: Fix any parameters that are out of range
void
bapl_mi_matcher_params::correct_range()
{
  if (max_sx_ < BAPL_MIN_SCALE) max_sx_ = BAPL_MIN_SCALE;
  if (max_sx_ > BAPL_MAX_SCALE) max_sx_ = BAPL_MAX_SCALE;
  if (max_tx_ < BAPL_MIN_TRANS) max_tx_ = BAPL_MIN_TRANS;
  if (max_tx_ > BAPL_MAX_TRANS) max_tx_ = BAPL_MAX_TRANS;
  if (max_rotation_ang_ < BAPL_MIN_ROT) max_rotation_ang_ = BAPL_MIN_ROT;
  if (max_rotation_ang_ > BAPL_MAX_ROT) max_rotation_ang_ = BAPL_MAX_ROT;
  if (max_shear_ang_ < BAPL_MIN_SHEAR) max_shear_ang_ = BAPL_MIN_SHEAR;
  if (max_shear_ang_ > BAPL_MAX_SHEAR) max_shear_ang_ = BAPL_MAX_SHEAR;
}


//: Output stream operator for printing the parameter values
std::ostream& operator<<(std::ostream& os, bapl_mi_matcher_params const& p)
{
  os << "bapl_mi_matcher_params:\n[---\n"
     << "max scale x  " << p.max_sx_ << '\n'
     << "max scale y  " << p.max_sy_ << '\n'
     << "max trans x  " << p.max_tx_ << '\n'
     << "max trans y  " << p.max_ty_ << '\n'
     << "max rotation " << p.max_rotation_ang_ << '\n'
     << "max shear    " << p.max_shear_ang_ << '\n'
     << "---]" << std::endl;
  return os;
}
