// This is gel/mrc/vpgl/icam/icam_view_metadata.h
#ifndef icam_view_metadata_h_
#define icam_view_metadata_h_
//:
// \file
// \brief A cost function for registering video cameras by minimizing square difference in intensities
// \author J.L. Mundy
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

//: A class to hold image information of a viewpoint
class icam_view_metadata 
{
 public:

   //: default constructor
   icam_view_metadata(){}

   icam_view_metadata(vil_image_view<double> const& exp_img, 
                      vil_image_view<double> const& depth_img, 
                      icam_depth_transform const& dt) 
                      : exp_img_(exp_img), depth_img_(depth_img)  { minimizer_=new icam_minimizer(exp_img, dt); }

   ~icam_view_metadata() { if (minimizer_) delete minimizer_; }

   //vil_image_view<double> expected_image() { return exp_img_; }

   //void set_expected_image(vil_image_view<double> img) { exp_image_=img; }

   //vil_pyr_image_view<double> depth_image() { return depth_img_; }
   
   //void set_depth_image(vil_pyramid_image_view<double> img) { depth_image_=img; }

   void register_image(vil_image_view<double> const& source_img);

   //void set_score(double s) { score_; }

   double error() { return min_cost_; }

   void print(vcl_ostream& os) const { os << " icam_view_metadata: " << vcl_endl; }

   void b_read(vsl_b_istream& is);

   void b_write(vsl_b_ostream& os) const ;

   inline short version() const { return 1; }

 private:
   //: expected image pyramid
   vil_image_view<double> exp_img_;

   //: depth image pyramid
   vil_image_view<double> depth_img_;

   // source image that is being registered
   vil_image_view<double> source_img_;

   //: solver for the registration
   icam_minimizer* minimizer_;

   double min_cost_;
};


vcl_ostream& operator<<(vcl_ostream& os, icam_view_metadata const& p);
void vsl_b_read(vsl_b_istream& is, icam_view_metadata& p);
void vsl_b_write(vsl_b_ostream& os, icam_view_metadata const& p);

#endif // icam_view_metadata_h_

