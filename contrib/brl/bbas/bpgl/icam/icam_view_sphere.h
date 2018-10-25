#ifndef icam_view_sphere_h_
#define icam_view_sphere_h_
//:
// \file
#include "icam_view_metadata.h"
#include <vsph/vsph_view_sphere.h>
#include <vsph/vsph_view_point.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vil/vil_image_view.h>
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>

class icam_view_sphere : public vbl_ref_count
{
 public:
  //: Constructor, creates a view sphere around the box, centered at box's center with radius
  icam_view_sphere(): view_sphere_(nullptr),ICAM_LOCAL_MIN_THRESH_(0.0){}
  icam_view_sphere(vgl_box_3d<double> bb, double radius);

  //: Destructor
  ~icam_view_sphere() override{ delete view_sphere_; }

  //: creates uniform view points on the view square and generates expected and depth images at each view point
  void create_view_points(double cap_angle, double view_angle, unsigned ni, unsigned nj);

  //: returns the cameras of the view points, associated with the view point id
  void cameras(std::map<unsigned, vpgl_camera_double_sptr> &cameras);

  void set_cameras(std::map<unsigned, vpgl_camera_double_sptr> const &cameras);

  //: sets the images and depth images, associated with the view point id
  virtual void set_images(std::map<unsigned, /*vil_image_view<float>**/std::string>& images,
                          std::map<unsigned, /*vil_image_view<double>**/std::string>& depth_images);

  //: computes the camera registration errors for a given image
  virtual void register_image(vil_image_view<float> const& source_img, icam_minimizer_params const& params);

  //: computes the camera registration errors local minima for a given image
  void find_local_minima(std::vector<vsph_view_point<icam_view_metadata> >& local_minima);

  //: given a camera, find the relative camera rotation and translations for each view point
  void camera_transf(vpgl_perspective_camera<double> const& cam);

  //: for debug purposes
  void set_ground_truth_cam(vpgl_camera_double_sptr const& gt_cam)
    {ground_truth_cam_ = gt_cam;}

  //: the mapped source image and actual destination image at a level
  void mapped_image(unsigned viewpoint_id,
                    vil_image_view<float> const& source_img,
                    vgl_rotation_3d<double>& rot,
                    vgl_vector_3d<double>& trans, unsigned level,
                    vil_image_view<float>& act_dest,
                    vil_image_view<float>& mapped_dest,
                    icam_minimizer_params const& params);


  //: binary I/O
  inline short version() const { return 1; }

  void b_read(vsl_b_istream &is);

  void b_write(vsl_b_ostream &os) const ;

 protected:
  vsph_view_sphere<vsph_view_point<icam_view_metadata> >* view_sphere_;
  //: for debugging - is the camera being searched for
  vpgl_camera_double_sptr ground_truth_cam_;
  const double ICAM_LOCAL_MIN_THRESH_;
};

void vsl_b_read(vsl_b_istream &is, icam_view_sphere &sp);
void vsl_b_write(vsl_b_ostream &os, icam_view_sphere const& sp);
void vsl_b_read(vsl_b_istream &is, icam_view_sphere* sp);
void vsl_b_write(vsl_b_ostream &os, const icam_view_sphere* sp);

#include "icam_view_sphere_sptr.h"

#endif
