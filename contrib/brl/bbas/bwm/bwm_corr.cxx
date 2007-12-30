#include "bwm_corr.h"
//:
// \file
#include "io/bwm_io_structs.h"
#include <vgl/vgl_distance.h>
#include <vcl_iostream.h>
#include <vsl/vsl_basic_xml_element.h>
#include <vsol/vsol_point_2d.h>

bool bwm_corr::match(bwm_observer_cam* obs, vgl_point_2d<double> &pt)
{
  vcl_map<bwm_observer_cam*, vgl_point_2d<double> >::iterator iter = matches_.begin();

  iter = matches_.find(obs);
  if (iter != matches_.end()) {
    pt = iter->second;
    return true;
  }

  vcl_cerr << "Correspondent point is not found for this observer\n";
  return false;
}
//need to use a tolerance to check equality of points
static bool
point_equal(vgl_point_2d<double> const & a, vgl_point_2d<double> const & b)
{
  double tol = 0.001;
  double d = vgl_distance<double>(a, b);
  return d<tol;
}

bool bwm_corr::update_match(bwm_observer_cam* obs, vgl_point_2d<double> old_pt, vgl_point_2d<double> new_pt)
{
  vcl_map<bwm_observer_cam*, vgl_point_2d<double> >::iterator
    iter = matches_.find(obs);

  if (iter != matches_.end()) {
    vgl_point_2d<double> pt(iter->second);
    if (point_equal(pt,old_pt)) {
      iter->second = vgl_point_2d<double> (new_pt);
      return true;
    }
    iter++;
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
  s << "Number of Cameras: " << c.matches_.size() << '\n';
  int i=0;
  vcl_map<bwm_observer_cam*, vgl_point_2d<double> >::const_iterator
    iter = c.matches_.begin();

  // first write down the camera info
  while (iter != c.matches_.end()) {
    s << "Camera " << i++ << " :" << iter->first->camera_path() << vcl_endl;
  }

  iter = c.matches_.begin();
  i=0;
  if (c.mode()) {
    while (iter != c.matches_.end()) {
      s <<  "Camera [" << i++ << "]: [X: " << iter->second.x() << " Y: " << iter->second.y() << "]\n";
      iter++;
    }
    s << "-----------------------------------------------------\n";
  }
  else
    s << "WORLD POINT: " << c.world_pt_ << '\n';
  return s;
}

void bwm_corr::x_write(vcl_ostream &os)
{
   vsl_basic_xml_element corr(CORRESPONDENCES_TAG);
   corr.x_write_open(os);

   if (mode() == false) {
    vsl_basic_xml_element xml_element(CORRESP_PT_TAG);
    xml_element.add_attribute("X", world_pt().x());
    xml_element.add_attribute("Y", world_pt().y());
    xml_element.add_attribute("Z", world_pt().z());
    xml_element.x_write(os);
   }

   vcl_map<bwm_observer_cam*, vgl_point_2d<double> >::const_iterator
   iter = matches_.begin();
   int i=0;
   while (iter != matches_.end()) {
     vsl_basic_xml_element corr_elm(CORRESP_ELM_TAG);
     corr_elm.x_write_open(os);

     vsl_basic_xml_element corr_tab(CORR_CAMERA_TAG);
     corr_tab.append_cdata(iter->first->tab_name());
     corr_tab.x_write(os);

     vsl_basic_xml_element xml_element(CORRESP_PT_TAG);
     xml_element.add_attribute("X", iter->second.x());
     xml_element.add_attribute("Y", iter->second.y());
     xml_element.x_write(os);
     corr_elm.x_write_close(os);
     iter++;
   }
   corr.x_write_close(os);
}

vcl_vector<vcl_pair<vcl_string, vsol_point_2d> > bwm_corr::match_list()
{
  vcl_vector<vcl_pair<vcl_string, vsol_point_2d> > list;
  vcl_map<bwm_observer_cam*, vgl_point_2d<double> >::iterator iter = matches_.begin();
  int i=0;
  while (iter != matches_.end()) {
   vcl_pair<vcl_string, vsol_point_2d> pair;
   pair.first = iter->first->tab_name();
   pair.second = vsol_point_2d(iter->second.x(), iter->second.y());
   list.push_back(pair);
   iter++;
 }
  return list;
}
