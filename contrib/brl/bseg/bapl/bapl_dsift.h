//this is /brl/bseg/bapl/bapl_dsift.h
#ifndef BAPL_DSIFT_H_
#define BAPL_DSIFT_H_
//:
// \file
// \brief Extract SIFT features at specific image locations.
// This is different from bapl_dense_sift in that it ignores scale space
// optimization, the descriptor is computed at the scale of the image.
// Additionally, the dominant gradient direction is not computed and an
// orientation can be specified before a descriptor is extracted.
//
// \author Brandon Mayer (b.mayer1@gmail.com)
// \date March 7, 2011
//
// \verbatim
//  Modifications
// \endverbatim

#include <iostream>
#include <cmath>
#include <vector>
#include <vil/vil_image_view.h>
#include <vil/io/vil_io_image_view.h>

#include <vnl/vnl_vector.h>

#include <vbl/vbl_ref_count.h>

#include <vsl/vsl_binary_io.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:
// Ignores Scale and Orientation computations and produces a sift descriptor at the scale of the image,
// specified orientation and location.
class bapl_dsift: public vbl_ref_count
{
 public:
  bapl_dsift() : grad_valid_(false) {}
  bapl_dsift( vil_image_view<float> const& img );
  bapl_dsift( vil_image_view<vxl_byte> const& img);
   ~bapl_dsift() override = default;

  bool set_img( vil_image_view<float> const& img );
  bool set_img( vil_image_view<vxl_byte> const& img );

  std::vector<float> dsift( unsigned const& key_x, unsigned const& key_y, float const& key_orient = 0.0f );

  vnl_vector<double> vnl_dsift( unsigned const& key_x, unsigned const& key_y, float const& key_orient = 0.0f );

  void b_write(vsl_b_ostream& os) const;

  void b_read(vsl_b_istream& is);

 protected:
  vil_image_view<float> grad_mag_;
  vil_image_view<float> grad_orient_;
  bool grad_valid_;

  inline static float gaussian( float const& x, float const& y ){return std::exp(-(x*x+y*y)/128.0f);}
};

#endif //BAPL_DSIFT_H_
