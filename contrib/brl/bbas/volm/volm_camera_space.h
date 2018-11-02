//This is brl/bbas/volm/volm_camera_space.h
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

#include <vector>
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vsl/vsl_binary_io.h>

// a struct to hold cam angles (in degrees )
class cam_angles
{
 public:
 cam_angles(double roll, double top_fov, double heading, double tilt):
  roll_(roll), top_fov_(top_fov), heading_(heading), tilt_(tilt) {}
  void print() const{
    std::cout << "(r: " << roll_ << " f: " << top_fov_
             << " h: " << heading_ << " t: " << tilt_ << ")\n";}
  std::string get_string() const;
  double dif(cam_angles& b);


  double  roll_;
  double  top_fov_;
  double  heading_;
  double  tilt_;
};
//angles are in degrees
double distance(cam_angles const& a, cam_angles const& b);

class camera_space_iterator;

//: defines a space of camera hypotheses
// heading x tilt x roll x focal length
/// ANGLES ARE IN DEGREES!!
class volm_camera_space : public vbl_ref_count
{
 public:
  //default constructor
  volm_camera_space():altitude_(0.0),ni_(0), nj_(0), head_mid_(0.0),
    head_radius_(0.0), head_inc_(0.0),tilt_mid_(0.0), tilt_radius_(0.0),
    tilt_inc_(0.0), roll_mid_(0.0),roll_radius_(0.0), roll_inc_(0.0) {}
  // angle units in degrees
  volm_camera_space(std::vector<double>  top_fov, double altitude,
                    unsigned ni, unsigned nj,
                    double head_mid=0.0,  double head_radius=180.0, double head_inc=2.0,
                    double tilt_mid=90.0, double tilt_radius=20.0,  double tilt_inc=2.0,
                    double roll_mid=0.0,  double roll_radius=3.0,   double roll_inc=2.0);

  volm_camera_space(double top_fov_mid, double top_fov_rad, double top_fov_inc,
                    double altitude,
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
  unsigned n_head() const {return n_head_;}

  double tilt_mid() const {return tilt_mid_;}
  double tilt_radius() const {return tilt_radius_;}
  double tilt_inc() const {return tilt_inc_;}
  unsigned n_tilt() const {return n_tilt_;}

  double roll_mid() const {return roll_mid_;}
  double roll_radius() const {return roll_radius_;}
  double roll_inc() const {return roll_inc_;}
  unsigned n_roll() const {return n_roll_;}

  double roll(unsigned roll_index) const
  { return (-roll_radius_ + roll_inc_*roll_index);}

  double head(unsigned head_index) const
  { return (-head_radius_ + head_inc_*head_index);}

  double tilt(unsigned tilt_index) const
  { return (-tilt_radius_ + tilt_inc_*tilt_index);}

  double top_fov(unsigned fov_index) const
  { return top_fovs_[fov_index];}

  //: focal length space
  std::vector<double> top_fovs() const {return top_fovs_;}
  unsigned n_fovs() const {return n_fovs_;}

  //: camera at current state of iterator
  vpgl_perspective_camera<double> camera() const{
    return this->camera(index_);}

  //: camera angles at current state of iterator
  cam_angles camera_angles() const;

  //: 1-d camera index at current state of iterator
  unsigned cam_index() const;

  //: individual camera angle indices at current state of iterator
  void cam_indices(unsigned & roll_index, unsigned& fov_index,
                   unsigned & head_index, unsigned& tilt_index) const
  { roll_index = roll_index_; fov_index = fov_index_; head_index = head_index_;
    tilt_index = tilt_index_;}

  //: transform 1-d index to indices
  void cam_indices(unsigned cam_index,
                   unsigned & roll_index, unsigned& fov_index,
                   unsigned & head_index, unsigned& tilt_index) const;

  //: transform indices to 1-d index
  unsigned cam_index(unsigned roll_index, unsigned fov_index,
                     unsigned head_index, unsigned tilt_index) const;

  //: return the index of the camera with params closest to the input angles, only the cameras in the valid array are searched
  std::pair<unsigned, cam_angles> cam_index_nearest_in_valid_array(cam_angles a);

  //: camera at specified index
  vpgl_perspective_camera<double> camera(unsigned cam_index) const;

  //: camera angles at specified index
  cam_angles camera_angles(unsigned cam_index) const;
  std::string get_string(unsigned cam_index) const;

  //: find 1-d index closest to specified camera angles, if -1 cangs were invalid
  int closest_index(cam_angles const& cangs);

  //: generate the full set of camera indices
  void generate_full_camera_index_space();

  //: add the given index to valid camera indices array
  void add_camera_index(unsigned cam_index) { valid_camera_indices_.push_back(cam_index); }

  //: remove a camera from the index space
  bool remove_camera_index(unsigned cam_index);

  //: access valid camera indices
  const std::vector<unsigned>& valid_indices() const{
    return valid_camera_indices_;}

  void print_valid_cams() const;

  //: the iterator at the start of camera space
  camera_space_iterator begin();

  //: the iterator at the start of camera space, freeze roll to specified value
  camera_space_iterator begin(double roll);

  //: the iterator at the end of camera space
  camera_space_iterator end();
  //: support for the iterator
  void init();
  bool next_cam();

  //: iterator for valid camera indices
  std::vector<unsigned>::iterator valid_begin() {return valid_camera_indices_.begin();}

  std::vector<unsigned>::iterator valid_end() {return valid_camera_indices_.end();}

  // ===========  binary I/O ================

  //: version
  unsigned version() const {return 1;}

  //: binary IO write
  void b_write(vsl_b_ostream& os);

  //: binary IO read
  void b_read(vsl_b_istream& is);

 private:
  //: make angle ranges consistent
  void adjust_limits();
  //: camera center height above terrain
  double altitude_;
  //: image dimensions
  unsigned ni_, nj_;
  //: heading space
  double head_mid_;
  double head_radius_;
  double head_inc_;
  unsigned n_head_;
  //: tilt space
  double tilt_mid_;
  double tilt_radius_;
  double tilt_inc_;
  unsigned n_tilt_;
  //: roll space
  double roll_mid_;
  double roll_radius_;
  double roll_inc_;
  unsigned n_roll_;
  //: focal length space
  std::vector<double> top_fovs_;
  unsigned n_fovs_;
  //: current camera state for iterator
  bool freeze_roll_;
  double heading_;
  unsigned head_index_;
  double tilt_;
  unsigned tilt_index_;
  double roll_;
  unsigned roll_index_;
  unsigned fov_index_;
  unsigned index_;
  std::vector<unsigned> valid_camera_indices_;
};

class camera_space_iterator
{
 public:
 camera_space_iterator(volm_camera_space* cam_space = nullptr)
   : end_(false), cam_space_(cam_space) {}

  ~camera_space_iterator() = default;

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
  void set_end() {end_ = true;}
  bool end() const{return end_;}
 private:
  bool end_;
  volm_camera_space* cam_space_;
};

#include "volm_camera_space_sptr.h"
void vsl_b_write(vsl_b_ostream& os, const volm_camera_space* rptr);

void vsl_b_read(vsl_b_istream &is, volm_camera_space*& rptr);

void vsl_b_write(vsl_b_ostream& os, const volm_camera_space_sptr& rptr);

void vsl_b_read(vsl_b_istream &is, volm_camera_space_sptr& rptr);
#endif // volm_camera_space_h_
