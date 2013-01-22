#include <volm/volm_camera_space.h>
volm_camera_space::
volm_camera_space(vcl_vector<double> const& top_fovs,double altitude, 
                  unsigned ni, unsigned nj,
                  double head_mid,  double head_radius, double head_inc,
                  double tilt_mid, double tilt_radius,  double tilt_inc,
                  double roll_mid,  double roll_radius,   double roll_inc):
  top_fovs_(top_fovs), altitude_(altitude), ni_(ni), nj_(nj), 
  head_mid_(head_mid), head_radius_(head_radius), head_inc_(head_inc),
  tilt_mid_(tilt_mid), tilt_radius_(tilt_radius),tilt_inc_(tilt_inc),
  roll_mid_(roll_mid),roll_radius_(roll_radius),roll_inc_(roll_inc),
  freeze_roll_(false),heading_(0.0), tilt_(0.0), roll_(0.0), fov_idx_(0){}

cam_angles volm_camera_space::camera_angles() const{
  return cam_angles(heading_, tilt_, roll_, top_fovs_[fov_idx_]);
}

//: the iterator at the start of camera space
camera_space_iterator volm_camera_space::begin()
{
  this->init();
  return camera_space_iterator(this);
}
//: the iterator at the start of camera space, freeze roll to value
camera_space_iterator volm_camera_space::begin(double roll){
  this->init();
  roll_ = roll;
  freeze_roll_ = true;
  return camera_space_iterator(this);
}

//: the iterator at the end of depth search
camera_space_iterator volm_camera_space::end()
{
  camera_space_iterator temp;
  temp.set_end();
  return temp;
}

//: initialize the camera space iterator
void volm_camera_space::init()
{
  heading_ = head_mid_-head_radius_;
  tilt_ = tilt_mid_-tilt_radius_;
  roll_ = roll_mid_-roll_radius_;
  fov_idx_ = 0;
}
//: move vert regions to next depth configuration. returns false if done
bool volm_camera_space::next_cam()
{
  // loop order
  //  roll
  //   focal length
  //    heading
  //     tilt
  if(tilt_< (tilt_mid_+tilt_radius_)){
    tilt_+=tilt_inc_;
    return true;
  }else if(heading_<(head_mid_+head_radius_)){
    heading_+=head_inc_;
    tilt_ = tilt_mid_-tilt_radius_;
    return true;
  }else if(fov_idx_<(top_fovs_.size()-1)){
    fov_idx_++;
    tilt_ = tilt_mid_-tilt_radius_;
    heading_ = head_mid_-head_radius_;
    return true;
  }else if(!freeze_roll_&&(roll_<roll_mid_+roll_radius_)){
    tilt_ = tilt_mid_-tilt_radius_;
    heading_ = head_mid_-head_radius_;
    fov_idx_ = 0;
    roll_ += roll_inc_;
    return true;
  }else
    return false;
}




