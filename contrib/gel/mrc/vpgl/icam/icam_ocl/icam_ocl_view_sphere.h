#ifndef icam_ocl_view_sphere_h_
#define icam_ocl_view_sphere_h_
//:
// \file
#include <icam/icam_view_metadata.h>
#include <icam/icam_view_sphere.h>
#include <vsph/vsph_view_point.h>
#include <vpgl/vpgl_camera.h>
#include <vil/vil_image_view.h>
#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>

class icam_ocl_view_sphere : public icam_view_sphere
{
 public:
  //: Constructor, creates a view sphere around the box, centered at box's center with radius
  icam_ocl_view_sphere(vgl_box_3d<double> bb, double radius)
  : icam_view_sphere(bb, radius){}

  //: Destructor
  ~icam_ocl_view_sphere(){}

  //: sets the images and depth images, associated with the view point id
  virtual void set_images(vcl_map<unsigned, vil_image_view<float>*>& images,
                          vcl_map<unsigned,vil_image_view<double>*>& depth_images);
  virtual void register_image(vil_image_view<float> const& source_img)
  { icam_view_sphere::register_image(source_img, icam_minimizer_params()); }
};

typedef vbl_smart_ptr<icam_ocl_view_sphere> icam_ocl_view_sphere_sptr;

#endif
