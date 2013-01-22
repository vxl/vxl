//This is brl/bbas/volm/volm_spherical_region_query.h
#ifndef volm_camera_space_h_
#define volm_camera_space_h_
//:
// \file
// \brief A class to represent the space of camera hypotheses
//
// \author J.L. Mundy
// \date January 20, 2012
// \verbatim
//  Modifications
// None
// \endverbatim

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vpgl/vpgl_perspective_camera.h>
// a struct to hold cam angles
class cam_angles{
 public:
 cam_angles(double heading, double tilt, double roll, double top_fov):
  heading_(heading), tilt_(tilt), roll_(roll), top_fov_(top_fov){}
  void print() const{
    vcl_cout << "(h: " << heading_ << " t: " << tilt_
             << " r: " << roll_ << " f: " << top_fov_ << ")\n";}
  double  heading_;
  double  tilt_;
  double  roll_;
  double  top_fov_;
};
//: defines a space of camera hypotheses
// heading x tilt x roll x focal length
class camera_space_iterator;
class volm_camera_space
{
 public:
  // angle units in degrees
  volm_camera_space(vcl_vector<double> const& top_fov, double altitude,
                    unsigned ni, unsigned nj,
                    double head_mid=0.0,  double head_radius=180.0, double head_inc=2.0,
                    double tilt_mid=90.0, double tilt_radius=20.0,  double tilt_inc=2.0,
                    double roll_mid=0.0,  double roll_radius=3.0,   double roll_inc=2.0);
  //: accessors
  double altitude() const {return altitude_;}
  //: image dimensions
  unsigned ni() const {return ni_;}
  unsigned nj() const {return nj_;}

  double head_mid() const {return head_mid_;}
  double head_radius() const {return head_radius_;}
  double head_inc() const {return head_inc_;}

  double tilt_mid() const {return tilt_mid_;}
  double tilt_radius() const {return tilt_radius_;}
  double tilt_inc() const {return tilt_inc_;}

  double roll_mid() const {return roll_mid_;}
  double roll_radius() const {return roll_radius_;}
  double roll_inc() const {return roll_inc_;}

  //: focal length space
  vcl_vector<double> top_fovs() const {return top_fovs_;}
  
  //: camera at current state of iterator
  vpgl_perspective_camera<double> camera() const;
  
  //: camera angles at current state of iterator
  cam_angles camera_angles() const;

  //: the iterator at the start of camera space
  camera_space_iterator begin();

  //: the iterator at the start of camera space, freeze roll to specified value
  camera_space_iterator begin(double roll);

  //: the iterator at the end of camera space
  camera_space_iterator end();
  //: support for the iterator
  void init();
  bool next_cam();

 private:
  //: camera center high above terrain
  double altitude_;
  //: image dimensions
  unsigned ni_, nj_;
  //: heading space
  double head_mid_;
  double head_radius_;
  double head_inc_;
  //: tilt space
  double tilt_mid_;
  double tilt_radius_;
  double tilt_inc_;
  //: roll space
  double roll_mid_;
  double roll_radius_;
  double roll_inc_;
  //: focal length space
  vcl_vector<double> top_fovs_;
  //: current camera state
  bool freeze_roll_;
  double heading_;
  double tilt_;
  double roll_;
  unsigned fov_idx_;
};

class camera_space_iterator
{
 public:
 camera_space_iterator(volm_camera_space* cam_space = 0)
   : end_(false), cam_space_(cam_space) {}

  ~camera_space_iterator() {}

  //: returns a reference to the scene to enable access to scene methods
  volm_camera_space& operator*() {
    return *cam_space_;
  }

  //: returns a pointer to the scene to enable access to scene methods
  volm_camera_space* operator->() {
    return cam_space_;
  }
  //: increments the camera space iterator
  camera_space_iterator& operator++() {
    if (!cam_space_) // if cam_space_ is null, there is no effect
      return *this;
    if (!cam_space_->next_cam()) end_ = true;
    return *this;
  }

  //: increments the camera space iterator n_inc times
  camera_space_iterator& operator+=(unsigned n_inc) {
    if (!cam_space_)
      return *this;

    for (unsigned k =0; k<n_inc; ++k) {
      if (!cam_space_->next_cam()) { end_ = true; break; }
    }
    return *this;
  }

  //: Only considers the state of end_ in determining equality.
  //  enables the test for the end of depth region arrangements
  bool operator==(const camera_space_iterator& it) {
    return end_ == it.end();
  }
  bool operator!=(const camera_space_iterator& it) {
    return !(end_ == it.end());
  }

  //: defines the state of completing the camera space
  void set_end(){end_ = true;}
  bool end() const{return end_;}
 private:
  bool end_;
  volm_camera_space* cam_space_;
};

#endif // volm_volm_camera_space_h_
