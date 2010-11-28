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
                      icam_depth_transform const& dt);

   ~icam_view_metadata() { if (minimizer_) delete minimizer_; }

   virtual void register_image(vil_image_view<float> const& source_img);

   virtual void refine_camera();

   double cost() { return cost_; }

   //: for debug purposes
   void mapped_image(vil_image_view<float> const& source_img,
                     vgl_rotation_3d<double>& rot,
                     vgl_vector_3d<double>& trans, unsigned level,
                     vil_image_view<float>& act_dest,
                     vil_image_view<float>& mapped_dest);

   void print(vcl_ostream& os) const { os << "icam_view_metadata:" << vcl_endl; }

   void b_read(vsl_b_istream& is);

   void b_write(vsl_b_ostream& os) const ;

   inline short version() const { return 1; }

   icam_minimizer* minimizer() {return minimizer_;}

 private:
   //: solver for the registration
   icam_minimizer* minimizer_;
   vgl_vector_3d<double> min_trans_;
   vgl_rotation_3d<double> min_rot_;
   double cost_;
   unsigned final_level_;
};


vcl_ostream& operator<<(vcl_ostream& os, icam_view_metadata const& p);
void vsl_b_read(vsl_b_istream& is, icam_view_metadata& p);
void vsl_b_write(vsl_b_ostream& os, icam_view_metadata const& p);

#endif // icam_view_metadata_h_

