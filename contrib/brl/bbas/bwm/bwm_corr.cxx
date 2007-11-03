#include "bwm_corr.h"
#include <vcl_iostream.h>
#include <vsl/vsl_basic_xml_element.h>


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

bool bwm_corr::obs_in(bwm_observer_cam* obs)
{
  vcl_map<bwm_observer_cam*, vgl_point_2d<double> >::iterator iter = matches_.begin();
  while (iter != matches_.end()) {
    if (obs == iter->first)
      return true;
    iter++;
  }
  return false;
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

void x_write(vcl_ostream &os, bwm_corr& c)
{
   os << "<correspondence>" << vcl_endl; 
   if (c.mode() == false) {
    vsl_basic_xml_element xml_element("corr_world_point");
    xml_element.add_attribute("X", c.world_pt().x());
    xml_element.add_attribute("Y", c.world_pt().y());
    xml_element.add_attribute("Z", c.world_pt().z());
    xml_element.x_write(os);
   }
   
   vcl_map<bwm_observer_cam*, vgl_point_2d<double> >::const_iterator 
   iter = c.matches_.begin();
   int i=0;
   while (iter != c.matches_.end()) {
     os << "<corr_elm>" << vcl_endl;
     os << "<corr_camera_tab>" << iter->first->tab_name() << "</corr_camera_tab>" << vcl_endl;
     vsl_basic_xml_element xml_element("corr_point");
     xml_element.add_attribute("X", iter->second.x());
     xml_element.add_attribute("Y", iter->second.y());
     xml_element.x_write(os);
     os << "</corr_elm>" << vcl_endl;
     iter++;
   }
   os << "</correspondence>" << vcl_endl;
}