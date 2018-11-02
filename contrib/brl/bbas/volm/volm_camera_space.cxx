#include <iostream>
#include <algorithm>
#include <limits>
#include <utility>
#include <volm/volm_camera_space.h>
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bpgl/bpgl_camera_utils.h>
#include <vsl/vsl_vector_io.h>
#include <vsph/vsph_utils.h>
  //it is possible for radius and increment to be inconsistent
  //that is, the radius is not a multiple of the increment
  //perhaps the best assumption is that increment rules and
  //the radius adjusted accordingly

  // always use floor in case inc does not evenly divide
void volm_camera_space::adjust_limits()
{
  auto n_inc = static_cast<unsigned>(head_radius_/head_inc_);
  head_radius_ = n_inc * head_inc_;
  n_head_ = 2*n_inc +1;

  //check for duplication of heading i.e. +- 180
  double head_start = head_mid_ - head_radius_;
  double head_end = head_mid_ + head_radius_;
  if ((head_start == -180.0) && (head_end == 180.0))
    n_head_--;

  n_inc = static_cast<unsigned>(tilt_radius_/tilt_inc_);
  tilt_radius_ = n_inc * tilt_inc_;
  n_tilt_ = 2*n_inc +1;
  n_inc = static_cast<unsigned>(roll_radius_/roll_inc_);
  roll_radius_ = n_inc * roll_inc_;
  n_roll_ = 2*n_inc +1;
  n_fovs_ = (unsigned)top_fovs_.size();
}

volm_camera_space::
volm_camera_space(std::vector<double>  top_fovs,double altitude,
                  unsigned ni, unsigned nj,
                  double head_mid,  double head_radius, double head_inc,
                  double tilt_mid, double tilt_radius,  double tilt_inc,
                  double roll_mid,  double roll_radius,   double roll_inc)
: altitude_(altitude), ni_(ni), nj_(nj),
  head_mid_(head_mid), head_radius_(head_radius), head_inc_(head_inc),
  tilt_mid_(tilt_mid), tilt_radius_(tilt_radius),tilt_inc_(tilt_inc),
  roll_mid_(roll_mid),roll_radius_(roll_radius),roll_inc_(roll_inc),
  top_fovs_(std::move(top_fovs)), freeze_roll_(false),
  heading_(0.0), tilt_(0.0), roll_(0.0), fov_index_(0)
{
  this->adjust_limits();
}

volm_camera_space::
volm_camera_space(double top_fov_mid, double top_fov_rad, double top_fov_inc,
                  double altitude,
                  unsigned ni, unsigned nj,
                  double head_mid,  double head_radius, double head_inc,
                  double tilt_mid, double tilt_radius,  double tilt_inc,
                  double roll_mid,  double roll_radius,   double roll_inc):
  altitude_(altitude), ni_(ni), nj_(nj),
  head_mid_(head_mid), head_radius_(head_radius), head_inc_(head_inc),
  tilt_mid_(tilt_mid), tilt_radius_(tilt_radius),tilt_inc_(tilt_inc),
  roll_mid_(roll_mid),roll_radius_(roll_radius),roll_inc_(roll_inc),
  freeze_roll_(false),heading_(0.0), tilt_(0.0), roll_(0.0), fov_index_(0)
{
  top_fovs_.push_back(top_fov_mid);    // top viewing ranges from 1 to 89
  for (double i = top_fov_inc; i <= top_fov_rad; i+=top_fov_inc) {
    double right_fov = top_fov_mid + i, left_fov = top_fov_mid - i;
    if (right_fov > 89)  right_fov = 89;
    if (left_fov  < 1)   left_fov = 1;
    top_fovs_.push_back(right_fov);
    if (left_fov != right_fov)
      top_fovs_.push_back(left_fov);
  }
  this->adjust_limits();
}

void  volm_camera_space::cam_indices(unsigned cam_index,
                                     unsigned & roll_index, unsigned& fov_index,
                                     unsigned & head_index, unsigned& tilt_index) const {
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
                                       unsigned head_index, unsigned tilt_index) const {
  unsigned temp = tilt_index + n_tilt_*head_index;
  temp += (n_head_*n_tilt_)*fov_index + (n_head_*n_tilt_*n_fovs_)*roll_index;
  return temp;
}

cam_angles volm_camera_space::camera_angles(unsigned cam_index) const
{
  unsigned roll_index,  fov_index;
  unsigned head_index,  tilt_index;
  this->cam_indices(cam_index, roll_index, fov_index, head_index, tilt_index);
  double roll = roll_mid_ - roll_radius_ + (roll_index*roll_inc_);
  double top_fov = top_fovs_[fov_index];
  double heading = head_mid_ - head_radius_ + (head_index*head_inc_);
  double tilt = tilt_mid_ - tilt_radius_ + (tilt_index*tilt_inc_);
  return {roll, top_fov, heading, tilt};
}

double cam_angles::dif(cam_angles& b)
{
  return std::sqrt(std::pow(roll_ - b.roll_, 2.0) + std::pow(top_fov_ - b.top_fov_, 2.0) +
    std::pow(heading_ - b.heading_, 2.0) + std::pow(tilt_ - b.tilt_, 2.0) );
}

//: return the index of the camera with params closest to the input angles, only the cameras in the valid array are searched
std::pair<unsigned, cam_angles> volm_camera_space::cam_index_nearest_in_valid_array(cam_angles a)
{
  double min = 4*360.0;
  cam_angles min_b(0,0,0,0);
  unsigned min_id;
  for (unsigned i = 0; i < valid_camera_indices_.size(); i++) {
    cam_angles b = this->camera_angles(valid_camera_indices_[i]);
    double val = a.dif(b);
    if (min > val) {
      min_b = b;
      min_id = i;
      min = val;
    }
  }
  return std::pair<unsigned, cam_angles>(min_id, min_b);
}


void volm_camera_space::generate_full_camera_index_space()
{
  valid_camera_indices_.clear();
  camera_space_iterator cit = this->begin();
  for (; cit != this->end(); ++cit)
    valid_camera_indices_.push_back(this->cam_index());
}

bool volm_camera_space::remove_camera_index(unsigned cam_index)
{
  // find if index exists
  std::vector<unsigned>::iterator it;
  it = std::find(valid_camera_indices_.begin(), valid_camera_indices_.end(),
                cam_index);
  if (it == valid_camera_indices_.end())
    return false;
  valid_camera_indices_.erase(it);
  return true;
}

unsigned volm_camera_space::cam_index() const
{
  unsigned temp = cam_index(roll_index_, fov_index_, head_index_, tilt_index_);
  return temp;
}

cam_angles volm_camera_space::camera_angles() const
{
  return {roll_, top_fovs_[fov_index_], heading_, tilt_};
}

//: the iterator at the start of camera space
camera_space_iterator volm_camera_space::begin()
{
  this->init();
  return camera_space_iterator(this);
}

//: the iterator at the start of camera space, freeze roll to value
camera_space_iterator volm_camera_space::begin(double roll)
{
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
  freeze_roll_ = false;
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
  if (tilt_index_ <  (n_tilt_-1)) {
    tilt_+=tilt_inc_;
    index_++;
    tilt_index_++;
    return true;
  }
  if (head_index_ < (n_head_-1)) {
    heading_+=head_inc_;
    head_index_++;
    index_++;
    tilt_ = tilt_mid_-tilt_radius_;
    tilt_index_ = 0;
    return true;
  }
  if (fov_index_ < (n_fovs_-1)) {
    fov_index_++;
    index_ ++;
    tilt_ = tilt_mid_-tilt_radius_;
    heading_ = head_mid_-head_radius_;
    tilt_index_ = 0;
    head_index_ = 0;
    return true;
  }
  if (!freeze_roll_&& (roll_index_< (n_roll_-1))) {
    tilt_ = tilt_mid_-tilt_radius_;
    heading_ = head_mid_-head_radius_;
    tilt_index_ = 0;
    head_index_ = 0;
    fov_index_ = 0;
    roll_ += roll_inc_;
    roll_index_++;
    index_++;
    return true;
  }
  return false;
}

vpgl_perspective_camera<double> volm_camera_space::camera(unsigned cam_index) const
{
  cam_angles ca = this->camera_angles(cam_index);
  double top_fov = ca.top_fov_;
  double dtor = vnl_math::pi_over_180;
  double tan_top_fov = std::tan(top_fov*dtor);
  double right_fov = std::atan(ni_*tan_top_fov/nj_)/dtor;
  return bpgl_camera_utils::camera_from_kml(ni_, nj_, right_fov, top_fov,
                                            altitude_, ca.heading_,
                                            ca.tilt_, ca.roll_);
}


void vsl_b_write(vsl_b_ostream& os, const volm_camera_space* csp_ptr)
{
  if (csp_ptr ==nullptr) {
    vsl_b_write(os, false);
    return;
  }
  else
    vsl_b_write(os, true);
  auto* dm_non_const = const_cast<volm_camera_space*>(csp_ptr);
  dm_non_const->b_write(os);
}

void vsl_b_read(vsl_b_istream &is, volm_camera_space*& csp_ptr)
{
  bool valid_ptr = false;
  vsl_b_read(is, valid_ptr);
  if (valid_ptr) {
    csp_ptr = new volm_camera_space();
    csp_ptr->b_read(is);
    return;
  }
  csp_ptr = nullptr;
}

void vsl_b_write(vsl_b_ostream& os, const volm_camera_space_sptr& csp_ptr)
{
  volm_camera_space* dm=csp_ptr.ptr();
  vsl_b_write(os, dm);
}

void vsl_b_read(vsl_b_istream &is, volm_camera_space_sptr& csp_ptr)
{
  volm_camera_space* dm=nullptr;
  vsl_b_read(is, dm);
  csp_ptr = dm;
}

//: binary IO write
void volm_camera_space::b_write(vsl_b_ostream& os)
{
  unsigned ver = this->version();
  vsl_b_write(os, ver);
  vsl_b_write(os, altitude_);
  vsl_b_write(os, ni_);
  vsl_b_write(os, nj_);
  vsl_b_write(os, head_mid_);
  vsl_b_write(os, head_radius_);
  vsl_b_write(os, head_inc_);
  vsl_b_write(os, tilt_mid_);
  vsl_b_write(os, tilt_radius_);
  vsl_b_write(os, tilt_inc_);
  vsl_b_write(os, roll_mid_);
  vsl_b_write(os, roll_radius_);
  vsl_b_write(os, roll_inc_);
  vsl_b_write(os, top_fovs_);
  vsl_b_write(os, valid_camera_indices_);
}

//: binary IO read
void volm_camera_space::b_read(vsl_b_istream& is)
{
  unsigned ver;
  vsl_b_read(is, ver);
  if (ver ==1) {
    vsl_b_read(is, altitude_);
    vsl_b_read(is, ni_);
    vsl_b_read(is, nj_);
    vsl_b_read(is, head_mid_);
    vsl_b_read(is, head_radius_);
    vsl_b_read(is, head_inc_);
    vsl_b_read(is, tilt_mid_);
    vsl_b_read(is, tilt_radius_);
    vsl_b_read(is, tilt_inc_);
    vsl_b_read(is, roll_mid_);
    vsl_b_read(is, roll_radius_);
    vsl_b_read(is, roll_inc_);
    vsl_b_read(is, top_fovs_);
    vsl_b_read(is, valid_camera_indices_);
    freeze_roll_=false;
    heading_=0.0;
    tilt_=0.0; roll_=0.0;
    fov_index_ = 0;
    this->adjust_limits();
    this->init();
  }
  else {
    std::cout << "volm_camera_space - unknown binary io version " << ver <<'\n';
    return;
  }
}
int volm_camera_space::closest_index(cam_angles const& cangs){

  camera_space_iterator cit = this->begin();
  double mind = std::numeric_limits<double>::max();
  int mindx = -1;
  for (; cit != this->end(); ++cit){
    cam_angles cit_cangs = this->camera_angles();
    double d = distance(cit_cangs, cangs);
    if(d<mind){
      mind = d;
      mindx = this->cam_index();
    }
  }
  return mindx;
}
double distance(cam_angles const& a, cam_angles const& b){
  //angles are assumed to be in radians.
  double rola = a.roll_, rolb = b.roll_;
  double ha = a.heading_, hb = b.heading_;
  double ta = a.tilt_, tb = b.tilt_;
  double fa = a.top_fov_, fb = b.top_fov_;
  double d = std::numeric_limits<double>::max();
  //don't allow invalid tilt angles
  if(rola<-90.0||rola>90.0||rolb<-90.0||rolb>90.0)
    return d;
  //don't allow invalid focal angles
  if(fa <= 0.0 || fa>90.0 ||fb <= 0.0 || fb>90.0)
    return d;
  //don't allow invalid tilt angles
  if(ta < -90.0  || ta>90.0 ||
     tb < -90.0  || tb>90.0)
    return d;
  d = std::fabs(vsph_utils::azimuth_diff(ha, hb));
  d += std::fabs(rola - rolb);
  d += (std::fabs(ta - tb) + std::fabs(fa - fb));
  return d;
}

std::string cam_angles::get_string() const
{
  std::stringstream str;
  str << "_h_" << heading_ << "_t_" << tilt_ << "_r_" << roll_ << "_top_fov_" << top_fov_;
  return str.str();
}

std::string volm_camera_space::get_string(unsigned cam_index) const
{
  cam_angles angles = this->camera_angles(cam_index);
  return angles.get_string();
}

void volm_camera_space::print_valid_cams() const
{
  for (unsigned i = 0; i < valid_camera_indices_.size(); ++i)
    std::cout << i << ": " << this->get_string(valid_camera_indices_[i]) << std::endl;
}
