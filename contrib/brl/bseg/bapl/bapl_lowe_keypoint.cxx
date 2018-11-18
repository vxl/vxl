// This is brl/bseg/bapl/bapl_lowe_keypoint.cxx
#include <iostream>
#include <cstdlib>
#include "bapl_lowe_keypoint.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bapl/bapl_lowe_pyramid_set.h>

//: Constructor
bapl_lowe_keypoint::bapl_lowe_keypoint()
{
  // make a random keypoint
  for ( int i=0; i<128; ++i) {
    descriptor_[i] = std::rand();
  }
  this->normalize();
}

//: Constructor
bapl_lowe_keypoint::bapl_lowe_keypoint( const bapl_lowe_pyramid_set_sptr& pyramid_set,
                                        double i, double j, double s, double o )
 : pyramid_set_(pyramid_set), location_i_(i),
   location_j_(j), scale_(s), orientation_(o)
{
  pyramid_set_->make_descriptor(this);
  this->normalize();
}

//: Constructor - do not extract descriptor from the pyramid but use the one that is passed, assumes the passed descriptor is normalized
bapl_lowe_keypoint::bapl_lowe_keypoint(const bapl_lowe_pyramid_set_sptr& pyramid_set,
                                       double i , double j, double s, double o, vnl_vector_fixed<double, 128>& desc)
 : descriptor_(desc), pyramid_set_(pyramid_set), location_i_(i),
   location_j_(j), scale_(s), orientation_(o)
{
}

//: Destructor
bapl_lowe_keypoint::~bapl_lowe_keypoint()
{
  // This destructor does absolutely nothing
}

//: Accessor for the descriptor vector
const vnl_vector_fixed<double,128>&
bapl_lowe_keypoint::descriptor() const
{
  return descriptor_;
}


//: Mutator for the descriptor vector
void
bapl_lowe_keypoint::set_descriptor(const vnl_vector_fixed<double,128>& descriptor)
{
  descriptor_ = descriptor;
  this->normalize();
}


//: Normalize, threshold, and renormalize
void
bapl_lowe_keypoint::normalize()
{
  descriptor_.normalize();
  bool changed = false;
  for ( int i=0; i<128; ++i) {
    if ( descriptor_[i] > 0.2 ) {
      descriptor_[i] = 0.2;
      changed = true;
    }
  }

  if ( changed )
    descriptor_.normalize();
}


//: Print a summary of the keypoint data to a stream
void
bapl_lowe_keypoint::print_summary(std::ostream& os) const
{
  os << "Lowe keypoint: ("<<location_i_<<','<<location_j_<<") scale="<<scale_<<" orientation="<<orientation_<<std::endl;
  //os << "               desc="<< descriptor_ << std::endl;
}

//: translate the coordinate frame with respect to image width and height if necessary, otherwise pass 0, 0
//  Warning: does not read/write the bapl_lowe_pyramid_set, only the location, orientation and descriptor of the keypoint
bapl_lowe_keypoint_sptr read_from_file(std::istream& ifs, int len, int img_width, int img_height)
{
  float loc_x, loc_y, scale, orientation;
  ifs >> loc_x >> loc_y >> scale >> orientation;
  if (img_height > 0 && img_width > 0) {
    loc_y = img_height - loc_y - 1.0f;
    loc_x -= 0.5f*img_width;
    loc_y -= 0.5f*img_height;
  }
  if (len != 128) {  // not possible to use this method for any other sized descriptor
    //std::cout << "In bapl_lowe_keypoint_sptr read_from_file() -- this method assumes 128 length descriptor vector!\n";
    std::cerr << "WARNING descriptor size is not 128, this method will work if len < 128 and fill the rest with zeros, otherwise it will break!\n";
    //return 0;
  }
  vnl_vector_fixed<double, 128> desc;
  desc.fill(0.0);
  for (int j = 0; j < len; ++j) {
    ifs >> desc[j];
  }
  bapl_lowe_pyramid_set_sptr py;
  bapl_lowe_keypoint_sptr kp = new bapl_lowe_keypoint(py, loc_y, loc_x, scale, orientation, desc);
  return kp;
}
