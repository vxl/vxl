#ifndef icam_view_sphere_h_
#define icam_view_sphere_h_
//:
// \file
#include "icam_view_metadata.h"
#include <vsph/vsph_view_sphere.h>
#include <vsph/vsph_view_point.h>
#include <vpgl/vpgl_camera.h>
#include <vil/vil_image_view.h>
#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>

class icam_view_sphere : public vbl_ref_count
{
 public:
  //: Constructor, creates a view sphere around the box, centered at box's center with radius
  icam_view_sphere(vgl_box_3d<double> bb, double radius);

  //: Destructor
  ~icam_view_sphere(){ delete view_sphere_; }

  //: creates uniform view points on the view square and generates expected and depth images at each view point
  void create_view_points(double cap_angle, double view_angle);

  //: returns the cameras of the view points, associated with the view point id
  void cameras(vcl_map<unsigned, vpgl_camera_double_sptr> &cameras);

  //: sets the images and depth images, associated with the view point id
  void set_images(vcl_map<unsigned, vil_image_view<double> > images,
                  vcl_map<unsigned,vil_image_view<double> > depth_images);

  //: computes the camera registration errors for a given image
  void register_image(vil_image_view<double> const& source_img);

  //: computes the camera registration errors local minimas for a given image
  void find_local_minima(vcl_vector<vsph_view_point<icam_view_metadata> >& local_minima);

  inline short version() const { return 1; }

  void b_read(vsl_b_istream &is);

  void b_write(vsl_b_ostream &os) const ;

 private:
  vsph_view_sphere<vsph_view_point<icam_view_metadata> >* view_sphere_;
};

typedef vbl_smart_ptr<icam_view_sphere> icam_view_sphere_sptr;

void vsl_b_read(vsl_b_istream const& is, icam_view_sphere &sp);
void vsl_b_write(vsl_b_ostream &os, icam_view_sphere const& sp);
void vsl_b_read(vsl_b_istream const& is, icam_view_sphere_sptr &sp);
void vsl_b_write(vsl_b_ostream &os, icam_view_sphere_sptr const& sp);


#endif
