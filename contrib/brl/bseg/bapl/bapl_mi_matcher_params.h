// This is brl/bseg/bapl/bapl_mi_matcher_params.h
#ifndef bapl_mi_matcher_params_h_
#define bapl_mi_matcher_params_h_
//:
// \file
// \brief A block of parameters for the bapl_mi_matcher class
// \author Matt Leotta
// \date October 15, 2003
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim

#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:A block of parameters for the bapl_mi_matcher class
// The member variable are kept public here for easy access,
// but the entire block will be a private member of bapl_mi_matcher
// This class is essentially just a data block with a few added functions
// for I/O and range validation.
//

class bapl_mi_matcher_params
{
 public:
  //: Constructor
  bapl_mi_matcher_params( unsigned num_samples=  10,
                          double max_scale=       2.0,
                          double max_radius=     10.0,
                          double max_rotation=   45.0,
                          double max_shear=      10.0 );

  //: Check the value of each member variable to see if it is in range
  bool valid_range();

  //: Fix any parameters that are out of range
  void correct_range();

  //: Number of random samples to generate
  unsigned num_samples_;

  // --- Maxima of parameters for random affine transformations ---

  //: Maximum horizontal scale
  double max_sx_;
  //: Maximum vertical scale
  double max_sy_;
  //: Maximum horizontal translation
  double max_tx_;
  //: Maximum vertical translation
  double max_ty_;
  //: Maximum rotation angle in degrees (0 to 180)
  double max_rotation_ang_;
  //: Maximum shear angle in degrees (0 to 90)
  double max_shear_ang_;
};

//: Output stream operator for printing the parameter values
std::ostream& operator<<(std::ostream& os, const bapl_mi_matcher_params& params);

#endif // bapl_mi_matcher_params_h_
