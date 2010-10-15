// This is gel/mrc/vpgl/icam/icam_view_metadata.h
#ifndef icam_view_metadata_h_
#define icam_view_metadata_h_
//:
// \file
// \brief  A class keeping the metadata associated with view point video cameras, like images
// \author Gamze Tunali
// \date Sept 6, 2010
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <vnl/vnl_vector.h>
#include <vnl/vnl_least_squares_function.h>
#include <vil/vil_image_view.h>
#include "icam_depth_transform.h"
#include "icam_minimizer.h"
#include <vcl_iostream.h>

//: A class to hold image information of a viewpoint
class icam_view_metadata
{
 public:

   //: default constructor
   icam_view_metadata(){}

   icam_view_metadata(vil_image_view<float> const& exp_img,
                      vil_image_view<double> const& depth_img,
                      icam_depth_transform const& dt)
                      : exp_img_(exp_img), depth_img_(depth_img)  { minimizer_=new icam_minimizer(exp_img, dt); }

   ~icam_view_metadata() { if (minimizer_) delete minimizer_; }

   void register_image(vil_image_view<float> const& source_img);

   void compute_camera();

   double error() { return error_; }

   void print(vcl_ostream& os) const { os << "icam_view_metadata:" << vcl_endl; }

   void b_read(vsl_b_istream& is);

   void b_write(vsl_b_ostream& os) const ;

   inline short version() const { return 1; }

 private:
   //: expected image pyramid
   vil_image_view<float> exp_img_;

   //: depth image pyramid
   vil_image_view<double> depth_img_;

   // destination image that is being registered
   vil_image_view<float> source_img_;

   //: solver for the registration
   icam_minimizer* minimizer_;

   double error_;
};


vcl_ostream& operator<<(vcl_ostream& os, icam_view_metadata const& p);
void vsl_b_read(vsl_b_istream& is, icam_view_metadata& p);
void vsl_b_write(vsl_b_ostream& os, icam_view_metadata const& p);

#endif // icam_view_metadata_h_

