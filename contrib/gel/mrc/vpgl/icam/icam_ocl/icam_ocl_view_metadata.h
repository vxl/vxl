// This is gel/mrc/vpgl/icam/icam_ocl/icam_ocl_view_metadata.h
#ifndef icam_ocl_view_metadata_h_
#define icam_ocl_view_metadata_h_
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

#include <vil/vil_image_view.h>
#include <icam/icam_depth_transform.h>
#include "icam_ocl_minimizer.h"
#include <icam/icam_view_metadata.h>
#include <vcl_iostream.h>

//: A class to hold image information of a viewpoint
class icam_ocl_view_metadata : public icam_view_metadata
{
 public:

   //: default constructor
   icam_ocl_view_metadata(){}

   icam_ocl_view_metadata(vcl_string const& exp_img,
                          vcl_string const& dt_path);

  // ~icam_ocl_view_metadata() { if (minimizer_) delete minimizer_; }

   virtual void register_image(vil_image_view<float> const& source_img,
                               vpgl_camera_double_sptr camera,
                               icam_minimizer_params const& params);

   virtual void refine_camera(vil_image_view<float> const& source_img,
                              vpgl_camera_double_sptr camera,
                              icam_minimizer_params const& params);

   void print(vcl_ostream& os) const { os << "icam_ocl_view_metadata:" << vcl_endl; }

   void b_read(vsl_b_istream& is);

   void b_write(vsl_b_ostream& os) const ;

 private:
   //: solver for the registration
  // icam_minimizer* minimizer_;
   vgl_vector_3d<double> min_trans_;
   vgl_rotation_3d<double> min_rot_;
   double cost_;
   unsigned final_level_;
   void create_minimizer(vil_image_view<float>*& exp_img, vil_image_view<double>*& depth_img,
                         vpgl_camera_double_sptr camera, icam_minimizer_params const& params,
                         icam_minimizer*& minimizer);
};

vcl_ostream& operator<<(vcl_ostream& os, icam_ocl_view_metadata const& p);
void vsl_b_read(vsl_b_istream& is, icam_ocl_view_metadata& p);

void vsl_b_write(vsl_b_ostream& os, icam_ocl_view_metadata const& p);

#endif // icam_ocl_view_metadata_h_
