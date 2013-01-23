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
  freeze_roll_(false),heading_(0.0), tilt_(0.0), roll_(0.0), fov_index_(0)
{
  //it is possible for radius and increment to be inconsistent
  //that is, the radius is not a multiple of the increment
  //perhaps the best assumption is that increment rules and 
  //the radius adjusted accordingly

  // always use floor in case inc does not evenly divide

  unsigned n_inc = static_cast<unsigned>(head_radius_/head_inc_);
  head_radius_ = n_inc * head_inc_;
  n_head_ = 2*n_inc +1;
  n_inc = static_cast<unsigned>(tilt_radius_/tilt_inc_);
  tilt_radius_ = n_inc * tilt_inc_;
  n_tilt_ = 2*n_inc +1;
  n_inc = static_cast<unsigned>(roll_radius_/roll_inc_);
  roll_radius_ = n_inc * roll_inc_;
  n_roll_ = 2*n_inc +1;
  n_fovs_ = (unsigned)top_fovs_.size();
}

void  volm_camera_space::cam_indices(unsigned cam_index,
		 unsigned & roll_index, unsigned& fov_index,
		 unsigned & head_index, unsigned& tilt_index) const{
  unsigned prd = n_head_*n_tilt_*n_fovs_;
  roll_index= cam_index/prd;
  unsigned resid = cam_index - (prd*roll_index);
  prd = (n_head_*n_tilt_);
  fov_index = resid/prd;
  resid = resid - (prd*fov_index);
  head_index = resid/n_tilt_;
  tilt_index = resid - (head_index*n_tilt_);
}
  
unsigned  volm_camera_space::cam_index(unsigned roll_index, unsigned fov_index,
		   unsigned head_index, unsigned tilt_index) const{
  unsigned temp = tilt_index + n_tilt_*head_index;
  temp += (n_head_*n_tilt_)*fov_index + (n_head_*n_tilt_*n_fovs_)*roll_index;
  return temp;
}

unsigned volm_camera_space::cam_index() const{
  unsigned temp = cam_index(roll_index_, fov_index_, head_index_, tilt_index_);
  return temp;
}

cam_angles volm_camera_space::camera_angles() const{
  return cam_angles(heading_, tilt_, roll_, top_fovs_[fov_index_]);
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
  roll_index_ = static_cast<unsigned>((roll-roll_mid_+roll_radius_)/roll_inc_);
  index_ = this->cam_index(roll_index_, fov_index_, head_index_, tilt_index_);

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
  fov_index_ = 0;
  tilt_index_ = 0;
  head_index_ = 0;
  fov_index_ = 0;
  roll_index_ = 0;
  index_ = 0;//1-d index
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
    index_++;
    tilt_index_++;
    return true;
  }else if(heading_<(head_mid_+head_radius_)){
    heading_+=head_inc_;
    head_index_++;
    index_ += n_tilt_;
    tilt_ = tilt_mid_-tilt_radius_;
    tilt_index_ = 0;
    return true;
  }else if(fov_index_<(top_fovs_.size()-1)){
    fov_index_++;
    index_ += (n_tilt_*n_head_);
    tilt_ = tilt_mid_-tilt_radius_;
    heading_ = head_mid_-head_radius_;
    head_index_ = 0;
    return true;
  }else if(!freeze_roll_&&(roll_<roll_mid_+roll_radius_)){
    tilt_ = tilt_mid_-tilt_radius_;
    heading_ = head_mid_-head_radius_;
    fov_index_ = 0;
    roll_ += roll_inc_;
    roll_index_++;
    index_ += (n_tilt_*n_head_*n_fovs_);
    return true;
  }else
    return false;
}




