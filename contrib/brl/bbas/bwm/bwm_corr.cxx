#include "bwm_corr.h"
#include <vcl_iostream.h>


/*bwm_corr::bwm_corr(const int n_cams, vgl_point_3d<double> const& wp)
{
  n_cams_ = n_cams;
  matches_.resize(n_cams_);
  valid_.resize(n_cams_);
  for(int i = 0; i<n_cams_; i++)
    {
      matches_[i]=vgl_homg_point_2d<double>(-1, -1);
      valid_[i]=false;
    }
  mode_=false;
  world_pt_ = wp;
}*/


bool bwm_corr::match(bwm_observer_cam* obs, vgl_point_2d<double> &pt)
{
  vcl_map<bwm_observer_cam*, vgl_point_2d<double> >::iterator iter = matches_.begin();
  
  iter = matches_.find(obs);
  if (iter != matches_.end()) {
    pt = iter->second;
    return true;
  }

  vcl_cerr << "Correspondent point is not found for this observer" << vcl_endl;
  return false;
}

bool bwm_corr::update_match(bwm_observer_cam* obs, vgl_point_2d<double> old_pt, vgl_point_2d<double> new_pt)
{
  vcl_map<bwm_observer_cam*, vgl_point_2d<double> >::iterator 
    iter = matches_.find(obs);

  if (iter != matches_.end()) {
    vgl_point_2d<double> pt(iter->second);
    if (pt == old_pt) {
      iter->second = vgl_point_2d<double> (new_pt);
      return true;
    }
    iter++;
  }
  return false;
}

//: checks if the given observer, 2D point pair is available 
bool bwm_corr::valid(bwm_observer_cam* obs, double x, double y)
{
  vcl_map<bwm_observer_cam*, vgl_point_2d<double> >::iterator iter = matches_.begin();
  
  iter = matches_.find(obs);
  if (iter != matches_.end()) {
    return true;
  }
  return false;
}

void bwm_corr::set_match(bwm_observer_cam* obs, const double x, const double y)
{
  vgl_point_2d<double> pt(x, y);
  //vcl_pair<bwm_observer_cam*, vgl_point_2d<double> > pair(obs, pt);
  matches_[obs] = pt;
}

//: Deletes the observer's correspondence point
void bwm_corr::erase(bwm_observer_cam* obs)
{
  matches_.erase(obs);
}

vcl_vector<bwm_observer_cam*> bwm_corr::observers()
{
  vcl_vector<bwm_observer_cam*>  obs(0);
  vcl_map<bwm_observer_cam*, vgl_point_2d<double> >::iterator iter = matches_.begin();
  while (iter != matches_.end()) {
    obs.push_back(iter->first);
    iter++;
  }
  return obs;
}

//external functions
vcl_ostream& operator<<(vcl_ostream& s, bwm_corr const& c)
{
  s << "Number of Cameras: " << c.matches_.size() << "\n";
  int i=0;
  vcl_map<bwm_observer_cam*, vgl_point_2d<double> >::const_iterator 
    iter = c.matches_.begin();

  // first write down the camera info
  while (iter != c.matches_.end()) {
    s << "Camera " << i++ << " :" << iter->first->camera_path() << vcl_endl;
  }

  iter = c.matches_.begin();
  i=0;
  if(c.mode()) {
    while (iter != c.matches_.end()) {
      s <<  "Camera [" << i++ << "]:" << "[X: " << iter->second.x() << " Y: " << iter->second.y() << "]\n";
      iter++;
    }
    s << "-----------------------------------------------------\n";
  } 
  else
      s << "WORLD POINT: " << c.world_pt_ << '\n';
  return s;
}
