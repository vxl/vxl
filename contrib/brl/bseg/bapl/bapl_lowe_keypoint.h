// This is brl/bseg/bapl/bapl_lowe_keypoint.h
#ifndef bapl_lowe_keypoint_h_
#define bapl_lowe_keypoint_h_
//:
// \file
// \brief Lowe scale-invariant keypoint
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date Sat Nov 8 2003
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli -- Sep 15, 2010 -- added constructor with a descriptor vector
// \endverbatim

#include <iostream>
#include <iosfwd>
#include <bapl/bapl_keypoint.h>
#include <bapl/bapl_lowe_keypoint_sptr.h>
#include <bapl/bapl_lowe_pyramid_set_sptr.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class bapl_lowe_keypoint : public bapl_keypoint
{
 public:
  //: Constructor
  bapl_lowe_keypoint();
  //: Constructor
  bapl_lowe_keypoint( const bapl_lowe_pyramid_set_sptr& pyramid_set,
                      double i, double j, double s=1, double o=0 );

  //: Constructor - do not extract descriptor from the pyramid but use the one that is passed, assumes the passed descriptor is normalized
  bapl_lowe_keypoint( const bapl_lowe_pyramid_set_sptr& pyramid_set,
                      double i , double j, double s, double o, vnl_vector_fixed<double, 128>& desc);

  //: Destructor
  ~bapl_lowe_keypoint() override;

  //: Accessor for the descriptor vector
  const vnl_vector_fixed<double,128>& descriptor() const override;

  //: Accessor for the i location
  double location_i() const {return location_i_; }
  //: Accessor for the j location
  double location_j() const {return location_j_; }
  //: Accessor for the scale
  double scale() const {return scale_; }
  //: Accessor for the orientation
  double orientation() const {return orientation_; }

  //: Mutator for the i location
  void set_location_i(double i) { location_i_ = i; }
  //: Mutator for the j location
  void set_location_j(double j) { location_j_ = j; }
  //: Mutator for the scale
  void set_scale(double s) { scale_ = s; }
  //: Mutator for the orientation
  void set_orientation(double o) { orientation_ = o; }

  //: Mutator for the descriptor vector
  void set_descriptor(const vnl_vector_fixed<double,128>& descriptor);

  //: Print a summary of the keypoint data to a stream
  void print_summary(std::ostream& os) const override;

 protected:
  //: Normalize, threshold, and renormalize
  void normalize();

 private:
  //: 128-dimensional descriptor vector
  vnl_vector_fixed<double,128> descriptor_;

  //: Smart pointer to the pyramids of images
  bapl_lowe_pyramid_set_sptr pyramid_set_;

  //: keypoint parameters
  double location_i_;
  double location_j_;
  double scale_;
  double orientation_;
};


//: Create a smart-pointer to a bapl_lowe_keypoint.
struct bapl_lowe_keypoint_new : public bapl_lowe_keypoint_sptr
{
  typedef bapl_lowe_keypoint_sptr base;

  //: Constructor - creates a default bapl_lowe_keypoint_sptr.
  bapl_lowe_keypoint_new() : base(new bapl_lowe_keypoint()) { }

  //: Constructor - creates a bapl_lowe_keypoint_sptr with some params.
  bapl_lowe_keypoint_new( bapl_lowe_pyramid_set_sptr p, double i, double j,
                          double s=1, double o=0)
  : base(new bapl_lowe_keypoint(p,i,j,s,o)) { }
};

//:
// warning: does not read/write the bapl_lowe_pyramid_set, only the location, orientation and descriptor of the keypoint.
//  translate the coordinate frame with respect to image width and height if necessary, otherwise pass 0, 0
bapl_lowe_keypoint_sptr read_from_file(std::istream& is, int len, int img_width = 0, int img_height = 0);

#endif // bapl_lowe_keypoint_h_
