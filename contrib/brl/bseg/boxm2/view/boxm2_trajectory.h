// This is brl/bseg/boxm2/view/boxm2_trajectory.h
#ifndef boxm2_trajectory_h_
#define boxm2_trajectory_h_
//:
// \file
// \brief A camera iterator that provides a smooth trajectory around a volume
// \author Andrew Miller
// \date March 2, 2011
//
// Given a start incline, end incline, and a radius, this class will provide
// a camera iterator that provides a smooth trajectory around a volume.
// For example this will be used to render a smooth pass around a volume, and
// turned into a video (for instance with mencoder)
// %% mencoder "mf://*.png" -mf type=png:fps=18 -ovc lavc -o output.avi
//
#include <vgl/vgl_box_3d.h>
#include <vpgl/vpgl_camera.h>
#include <vil/vil_image_view.h>
//smart ptr includes
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>


class boxm2_trajectory : public vbl_ref_count
{
 public:

  //: construct camera vector from incline0, incline1, radius, bounding box and image size (ni,nj)
  boxm2_trajectory(double incline0, double incline1, double radius, vgl_box_3d<double> bb, unsigned ni, unsigned nj) {
    init_cameras(incline0, incline1, radius, bb, ni, nj);
  }

  //: initialize cameras using parametrization in a photo overlay given by a kml file
  boxm2_trajectory(unsigned ni, unsigned nj, double right_fov, double top_fov, double alt, double heading, double tilt, double roll,
    double x_start, double y_start, double x_end, double y_end, double x_increment, double y_increment, double heading_increment);

  //: initialize cameras using parametrization in a photo overlay given by a kml file
  //  use the x,y,z images to set camera centers, height of camera will be z value + alt
  boxm2_trajectory(vgl_box_3d<double> bb, vil_image_view<float>& x_img, vil_image_view<float>& y_img, vil_image_view<float>& z_img,
                   unsigned ni, unsigned nj, double right_fov, double top_fov, double alt, double tilt, double roll,
                   unsigned margin, unsigned i_start, unsigned j_start, unsigned i_inc, unsigned j_inc, double heading_start, double heading_increment);

  virtual ~boxm2_trajectory() {}

  //: number of cameras in the trajectory
  unsigned size() const { return (unsigned)cams_.size(); }

  //: Iterator
  typedef std::vector<vpgl_camera_double_sptr>::iterator iterator;
  iterator begin() { return cams_.begin(); }
  iterator end()   { return cams_.end(); }

  //: next method
  vpgl_camera_double_sptr next() {
    if ( iter_ == cams_.end() ) iter_ = cams_.begin();
    return *iter_++;
  }

  //: direct access to cameras
  vpgl_camera_double_sptr camera(unsigned int i) {i %= cams_.size(); return cams_[i];}

 protected:

  //list of cams
  std::vector<vpgl_camera_double_sptr> cams_;

  //next cam
   std::vector<vpgl_camera_double_sptr>::iterator iter_;

  //: initialize cameras
  void init_cameras(double incline0, double incline1, double radius, vgl_box_3d<double> bb, unsigned ni, unsigned nj);

};

//: Smart_Pointer typedef for boxm2_data_base
typedef vbl_smart_ptr<boxm2_trajectory> boxm2_trajectory_sptr;

//: Binary write boxm_update_bit_scene_manager scene to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_trajectory const& scene);
void vsl_b_write(vsl_b_ostream& os, const boxm2_trajectory* &p);
void vsl_b_write(vsl_b_ostream& os, boxm2_trajectory_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, boxm2_trajectory_sptr const& sptr);

//: Binary load boxm_update_bit_scene_manager scene from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_trajectory &scene);
void vsl_b_read(vsl_b_istream& is, boxm2_trajectory* p);
void vsl_b_read(vsl_b_istream& is, boxm2_trajectory_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, boxm2_trajectory_sptr const& sptr);

#endif // boxm2_trajectory_h_
