// This is bbas/bpgl/icam/icam_view_metadata.h
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

#include <iostream>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include "icam_depth_transform.h"
#include "icam_minimizer.h"
#include "icam_minimizer_params.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A class to hold image information of a viewpoint
class icam_view_metadata
{
 public:

   //: default constructor
   icam_view_metadata() = default;

   icam_view_metadata(std::string  exp_img,
                      std::string  dt_path);

   virtual ~icam_view_metadata() = default;

   virtual void register_image(vil_image_view<float> const& source_img,
                               vpgl_camera_double_sptr camera,
                               icam_minimizer_params const& params);

   virtual void refine_camera(vil_image_view<float> const& source_img,
                              vpgl_camera_double_sptr camera,
                              icam_minimizer_params const& params);

   double cost() { return cost_; }

   //: for debug purposes
   void mapped_image(vil_image_view<float> const& source_img,
                     const vpgl_camera_double_sptr& camera,
                     vgl_rotation_3d<double>& rot,
                     vgl_vector_3d<double>& trans, unsigned level,
                     icam_minimizer_params const& params,
                     vil_image_view<float>& act_dest,
                     vil_image_view<float>& mapped_dest);

   void print(std::ostream& os) const { os << "icam_view_metadata:" << std::endl; }

   void b_read(vsl_b_istream& is);

   void b_write(vsl_b_ostream& os) const ;

   inline short version() const { return 1; }

 protected:
   std::string exp_img_path_;
   std::string depth_img_path_;
   vgl_vector_3d<double> min_trans_;
   vgl_rotation_3d<double> min_rot_;
   double cost_;
   unsigned final_level_;

   void create_minimizer(vil_image_view<float>*& exp_img, vil_image_view<double>*& depth_img,
                         const vpgl_camera_double_sptr& camera, icam_minimizer_params const& params,
                         icam_minimizer*& minimizer);
};


std::ostream& operator<<(std::ostream& os, icam_view_metadata const& p);
void vsl_b_read(vsl_b_istream& is, icam_view_metadata& p);
void vsl_b_write(vsl_b_ostream& os, icam_view_metadata const& p);

template <class T>
bool load_image(vil_image_view_base_sptr const& base_img, vil_image_view<T>*& image)
{
  if (base_img->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float>* fimage = static_cast<vil_image_view<float>*> (base_img.ptr());
    image = new vil_image_view<T>(base_img->ni(), base_img->nj());
    vil_convert_cast<float,T>(*fimage, *image);
  }
  else if (base_img->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    vil_image_view<vxl_byte>* byte_image = static_cast<vil_image_view<vxl_byte>*> (base_img.ptr());
    image = new vil_image_view<T>(base_img->ni(), base_img->nj());
    vil_convert_cast<vxl_byte,T>(*byte_image, *image);
  }
  else if (base_img->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    vil_image_view<double>* img = static_cast<vil_image_view<double>*> (base_img.ptr());
    image = new vil_image_view<T>(base_img->ni(), base_img->nj());
    vil_convert_cast<double,T>(*img, *image);
  }
  else {
    std::cout << "icam_register_image_process -- image type " << base_img->pixel_format() << " is not supported!" << std::endl;
    return false;
  }
  return true;
}

#endif // icam_view_metadata_h_
