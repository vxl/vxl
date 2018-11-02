#include <iostream>
#include <fstream>
#include "bwm_observer_mgr.h"
//:
// \file
#include "algo/bwm_utils.h"
#include "bwm_world.h"
#include "bwm_3d_corr.h"
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vpgl/algo/vpgl_rational_adjust_onept.h>
#include <vsl/vsl_basic_xml_element.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_3d.h>
#include <vgui/vgui_soview.h>
#include <vgui/vgui_dialog.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bwm_observer_cam* bwm_observer_mgr::BWM_MASTER_OBSERVER = nullptr;
bwm_observer_cam* bwm_observer_mgr::BWM_EO_OBSERVER = nullptr;
bwm_observer_cam* bwm_observer_mgr::BWM_OTHER_MODE_OBSERVER = nullptr;

bwm_observer_mgr* bwm_observer_mgr::instance_ = nullptr;

bwm_observer_mgr* bwm_observer_mgr::instance()
{
  if (!instance_)
    instance_ = new bwm_observer_mgr();
  return bwm_observer_mgr::instance_;
}

void bwm_observer_mgr::clear()
{
  corr_mode_ = IMAGE_TO_IMAGE;
  n_corrs_ = SINGLE_PT_CORR;
  corr_type_ = FEATURE_CORR;
  start_corr_ = false;
  corr_list_.clear();
}

std::vector<bwm_observer_cam*> bwm_observer_mgr::observers_cam() const
{
  std::vector<bwm_observer_cam*> v;
  for (unsigned i=0; i< observers_.size(); i++) {
    if (observers_[i]->type_name().compare("bwm_observer_cam") == 0)
      v.push_back(static_cast<bwm_observer_cam*> (observers_[i]));
    if (observers_[i]->type_name().compare("bwm_observer_rat_cam") == 0)
      v.push_back(static_cast<bwm_observer_cam*> (observers_[i]));
    if (observers_[i]->type_name().compare("bwm_observer_proj_cam") == 0)
      v.push_back(static_cast<bwm_observer_cam*> (observers_[i]));
    if (observers_[i]->type_name().compare("bwm_observer_geo_cam") == 0)
      v.push_back(static_cast<bwm_observer_cam*> (observers_[i]));
    if (observers_[i]->type_name().compare("bwm_observer_video") == 0)
      v.push_back(static_cast<bwm_observer_cam*> (observers_[i]));
  }
  return v;
}

std::vector<bwm_observer_rat_cam*> bwm_observer_mgr::observers_rat_cam() const
{
  std::vector<bwm_observer_rat_cam*> v;

  for (unsigned i=0; i< observers_.size(); i++) {
    if (observers_[i]->type_name().compare("bwm_observer_rat_cam") == 0)
      v.push_back(static_cast<bwm_observer_rat_cam*> (observers_[i]));
  }

  return v;
}

void bwm_observer_mgr::add(bwm_observer* o)
{
  observers_.push_back(o);

  // make the connection between this observer and the available observables
  std::vector<bwm_observable_sptr> objects = bwm_world::instance()->objects();
#if 0
  vgui_message msg;
  msg.data = "new";
#endif
  for (unsigned i=0; i<objects.size(); i++) {
    bwm_observable_sptr obj = objects[i];
    if (!obj)
      std::cerr << "ERROR: world has an invalid (NULL) object!\n";
    else {
      obj->attach(o);
      o->add_new_obj(obj);
    }
  }
}

void bwm_observer_mgr::attach(bwm_observable_sptr obs)
{
  for (unsigned i=0; i<observers_.size(); i++)
    obs->attach(observers_[i]);
}

void bwm_observer_mgr::detach(bwm_observable_sptr obs)
{
  for (unsigned i=0; i<observers_.size(); i++)
    obs->detach(observers_[i]);
}

void bwm_observer_mgr::remove(bwm_observer* observer)
{
  for (unsigned i=0; i<observers_.size(); i++)
    if (observers_[i] == observer)
      observers_.erase(observers_.begin()+i);
}

bool bwm_observer_mgr::comp_avg_camera_center(vgl_point_3d<double> &cam_center)
{
  unsigned num = 0;
  double x=0, y=0, z=0;
  vgl_point_3d<double> c;

  // go through the camera observers and compute a camera center
  for (unsigned i=0; i<observers_.size(); i++) {
    if (observers_[i]->type_name().compare("bwm_observer_rat_cam") == 0) {
      bwm_observer_rat_cam* obs = static_cast<bwm_observer_rat_cam*> (observers_[i]);
      obs->camera_center(c);
      vgl_point_3d<double> temp(c);
      x += temp.x();
      y += temp.y();
      z += temp.z();
      num++;
    }
  }

  if (num == 0)
    return false;

  cam_center.set(x/num, y/num, z/num);
  return true;
}

#if 0
//: Set a world point to be used in correspondences from world to image
void bwm_observer_mgr::set_world_pt(vgl_point_3d<double> world_pt)
{
  corr_world_pt_ = world_pt;
  world_point_valid_ = true;
}
#endif

//: Set the correspondence mode which is either image_to_image or world_to_image
// The world_to_image mode is only possible if there is a world point available
// The existence of a valid world point is defined by the flag \a world_point_valid_
void bwm_observer_mgr::set_corr_mode()
{
  vgui_dialog params ("Correspondence Mode");
  std::string empty="";
  std::vector<std::string> modes;
  int mode = bwm_observer_mgr::instance()->corr_mode();
  modes.push_back("Image to Image");
  modes.push_back("World to Image");

  std::vector<std::string> n_corrs;
  int n = bwm_observer_mgr::instance()->n_corrs();
  n_corrs.push_back("Single Correspondence ");
  n_corrs.push_back("Multiple Correspondence ");

  std::vector<std::string> types;
  int t = bwm_observer_mgr::instance()->corr_type();
  types.push_back("Feature Correspondence ");
  types.push_back("Terrain Correspondence ");

  std::string name, type;
  params.choice("Correspondence Mode", modes, mode);
  params.choice("Correspondence Type ", types, t);
  params.choice("Number of Correspondences ", n_corrs, n);

  if (!params.ask())
    return;
  if (mode ==bwm_observer_mgr::WORLD_TO_IMAGE) {
    if (bwm_world::instance()->world_pt_valid()) {
      corr_mode_ =  bwm_observer_mgr::WORLD_TO_IMAGE;
      return;
    }
    else {
      std::cout << "In bwm_observer_mgr::set_corr_mode() -\n"
               << " can't use WORLD_TO_IMAGE mode since the 3-d world"
               << " point is not defined" << std::endl;
    }
  }

  if (t == bwm_observer_mgr::FEATURE_CORR) {
    corr_type_ = bwm_observer_mgr::FEATURE_CORR;
  }
  else if (t == bwm_observer_mgr::TERRAIN_CORR) {
    corr_type_ = bwm_observer_mgr::TERRAIN_CORR;
  }
  else
    std::cout << "In bwm_observer_mgr::set_corr_mode() Undefined TYPE - " << t << std::endl;

  corr_mode_ = bwm_observer_mgr::IMAGE_TO_IMAGE;

  n_corrs_ = (BWM_N_CORRS) n;
}

void bwm_observer_mgr::collect_corr()
{
  bwm_corr_sptr corr = new bwm_corr();
  vgl_point_2d<double> pt;

  // set mode
  if (corr_mode_ == IMAGE_TO_IMAGE)
    corr->set_mode(true);
  else if (corr_mode_ == WORLD_TO_IMAGE) {
    vgl_point_3d<double> wpt;
    if (!bwm_world::instance()->world_pt(wpt))
    {
      std::cerr << " In bwm_observer_mgr::collect_corr() -"
               << " Can't do world to image, world pt invalid\n";
      return;
    }
    corr->set_mode(false);
    corr->set_world_pt(wpt);
  }
  else
    std::cerr << "Unknown correspondence mode!\n";

  bool found = false;
  std::vector<bwm_observer_cam*> obs_cam = this->observers_cam();
  for (unsigned i=0; i< obs_cam.size(); i++) {
    bwm_observer_cam* obs = obs_cam[i];
    if (obs->corr_pt(pt)) {
      corr->set_match(obs, pt.x(), pt.y());
      obs->record_corr_pt();
      found = true;
    }
  }

  if (found)
  {
    if (n_corrs_==MULTIPLE_CORRS) {
      if (corr_type_ == FEATURE_CORR)
        corr_list_.push_back(corr);
      else if (corr_type_ == TERRAIN_CORR)
        terrain_corr_list_.push_back(corr);
      return;
    }
    else if (n_corrs_==SINGLE_PT_CORR) // in this case there can be only one
    {
      if (corr_type_ == FEATURE_CORR) {
        corr_list_.clear();
        corr_list_.push_back(corr);
      }
      else if (corr_type_ == TERRAIN_CORR) {
        terrain_corr_list_.clear();
        terrain_corr_list_.push_back(corr);
      }
      return;
    }
  }
  std::cerr << "No Correspondence SET yet!!\n";
}

void bwm_observer_mgr::set_corr(bwm_corr_sptr corr)
{
  if (corr->num_matches() > 0) {
    if (corr_type_ == FEATURE_CORR) {
      corr_list_.push_back(corr);
    }
    else if (corr_type_ == TERRAIN_CORR) {
      terrain_corr_list_.push_back(corr);
    }
  }
}

void bwm_observer_mgr::update_corr(bwm_observer_cam* obs,
                                   vgl_point_2d<double> old_pt,
                                   vgl_point_2d<double> new_pt)
{
  for (unsigned i=0; i< corr_list_.size(); i++) {
    if (corr_type_ == FEATURE_CORR) {
      bwm_corr_sptr corr = corr_list_[i];
      if (corr->update_match(obs, old_pt, new_pt))
       return;
    }
    else if (corr_type_ == TERRAIN_CORR) {
      bwm_corr_sptr corr = terrain_corr_list_[i];
      if (corr->update_match(obs, old_pt, new_pt))
       return;
    }
  }
}

//: finds out if that observer involved with any of the correspondences
bool bwm_observer_mgr::obs_in_corr(bwm_observer_cam *obs)
{
  for (unsigned i=0; i<corr_list_.size(); i++) {
    vgl_point_2d<double> c;
    if (corr_list_[i]->obs_in(obs, c))
      return true;
  }
  return false;
}

//: returns the list correspondence points of a given observer
std::vector<vgl_point_2d<double> >
bwm_observer_mgr::get_corr_points(bwm_observer_cam *obs)
{
  std::vector<vgl_point_2d<double> > corr_list;
  for (unsigned i=0; i<corr_list_.size(); i++) {
    vgl_point_2d<double> corr;
    if (corr_list_[i]->obs_in(obs, corr)) {
      corr_list.push_back(corr);
    }
  }
  return corr_list;
}

void bwm_observer_mgr::save_corr(std::ostream& s)
{
  if (corr_list_.size() == 0)
    std::cerr << "No correspondences to save yet!\n";
  else
  {
#if 0
    std::string fname = bwm_utils::select_file();
    std::ofstream s(fname.data());

    s << "Cameras:" << std::endl;
#endif
    // first write down the camera info
    std::map<bwm_observer_cam*, unsigned> camera_map;
    for (unsigned i=0; i< observers_.size(); i++)
    {
      if ((observers_[i]->type_name().compare("bwm_observer_rat_cam") == 0) ||
          (observers_[i]->type_name().compare("bwm_observer_cam_proj") == 0))
      {
        bwm_observer_cam* obs = static_cast<bwm_observer_cam *> (observers_[i]);

        // check if that camera is involved with any of the correspondences
        if (obs_in_corr(obs)) {
          s << "CAM_TAB: " << i << '\n'
            << "IMAGE: " << obs->image_tableau()->file_name() << '\n'
            << "CAMERA_TYPE: ";
          if (observers_[i]->type_name().compare("bwm_observer_rat_cam") == 0)
            s << "rational" << std::endl;
          else if (observers_[i]->type_name().compare("bwm_observer_cam_proj") == 0)
            s << "projective" << std::endl;
          s << "CAMERA_PATH: " << obs->camera_path() << '\n' << std::endl;
          camera_map[obs] = i;
        }
      }
    }

    s << "CORRESPONDENCES: " << corr_list_.size() << '\n'
      << "CORR_MODE: ";
    if (corr_mode_ == IMAGE_TO_IMAGE)
      s << "IMAGE_TO_IMAGE" << std::endl;
    else
      s << "WORLD_TO_IMAGE" << std::endl;
    for (unsigned i=0; i< corr_list_.size(); i++)
    {
      bwm_corr_sptr corr = corr_list_[i];
      s << "C: " << corr->num_matches() << std::endl;

      std::vector<bwm_observer_cam*> obs = corr->observers();
#if 0
      s << obs.size() << std::endl;
#endif
      if (! corr->mode()) { // WORLD TO IMAGE
        s << "WORLD_POINT: " << corr->world_pt().x() << ' ' << corr->world_pt().y()
          << ' ' << corr->world_pt().z() << std::endl;
      }
      for (unsigned j=0; j< obs.size(); j++) {
        vgl_point_2d<double> p;
        if (corr->match(obs[j], p))
          s << camera_map[obs[j]] << ' ' << p.x() << ' ' << p.y() << std::endl;
      }
    }
    s << "END" << std::endl;
  }
}

void bwm_observer_mgr::save_corr_XML()
{
  if (corr_list_.size() == 0)
    std::cerr << "No correspondences to save yet!\n";
  else
  {
    std::string fname = bwm_utils::select_file();
    std::ofstream s(fname.data());

    s << "<BWM_CONFIG>" << '\n'
      << "<TABLEAUS>" << std::endl;
    // first write down the camera info
    std::map<bwm_observer_cam*, unsigned> camera_map;
    for (unsigned i=0; i< observers_.size(); i++)
    {
      if ((observers_[i]->type_name().compare("bwm_observer_rat_cam") == 0) ||
          (observers_[i]->type_name().compare("bwm_observer_cam_proj") == 0))
      {
        bwm_observer_cam* obs = static_cast<bwm_observer_cam *> (observers_[i]);

        // check if that camera is involved with any of the correspondences
        if (obs_in_corr(obs))
        {
          vsl_basic_xml_element tab("CameraTableau");
          tab.add_attribute("name", obs->tab_name());
          tab.x_write_open(s);

          vsl_basic_xml_element img_path("imagePath");
          img_path.append_cdata(obs->image_tableau()->file_name());
          img_path.x_write(s);

          std::string type;
          if (observers_[i]->type_name().compare("bwm_observer_rat_cam") == 0)
            type = "rational";
          else if (observers_[i]->type_name().compare("bwm_observer_cam_proj") == 0)
            type = "projective";

          vsl_basic_xml_element cam_path("cameraPath");
          cam_path.add_attribute("type", type);
          cam_path.append_cdata(obs->camera_path());
          cam_path.x_write(s);
          camera_map[obs] = i;
          tab.x_write_close(s);
        }
      }
    }
    s << "</TABLEAUS>" << std::endl;

    // write out the correspondence list
    std::string m = "";
    if (corr_mode_ == IMAGE_TO_IMAGE)
      m = "IMAGE_TO_IMAGE";
    else
      m = "WORLD_TO_IMAGE";
    vsl_basic_xml_element xml_element("correspondences");
    xml_element.add_attribute("mode", m);

    std::string n = "";
    if (n_corrs_==MULTIPLE_CORRS)
      n = "MULTIPLE";
    else
      n = "SINGLE";
    xml_element.add_attribute("type", n);
    xml_element.x_write_open(s);


    for (unsigned i=0; i< corr_list_.size(); i++) {
      bwm_corr_sptr corr = corr_list_[i];
      std::cout << corr->num_matches() << std::endl;
      corr->x_write(s);
    }
    xml_element.x_write_close(s);
    s << "</BWM_CONFIG>" << std::endl;
  }
}

void bwm_observer_mgr::delete_last_corr()
{
  unsigned i = corr_list_.size();
  if (i > 0) {
    // first notify the observer to delete the corr point on the screen
    bwm_corr_sptr corr = corr_list_[i-1];
    std::vector<bwm_observer_cam*> obs = corr->observers();
    for (unsigned i=0; i<obs.size(); i++) {
      obs[i]->remove_corr_pt();
      obs[i]->post_redraw();
    }
    corr_list_.pop_back();  // removes the last element
  }
}

void bwm_observer_mgr::delete_all_corr()
{
  while (corr_list_.size() > 0) {
    delete_last_corr();
  }
}

void bwm_observer_mgr::print_observers()
{
  for (unsigned i=0; i< observers_.size(); i++) {
    std::cout <<  i << " - " << observers_[i]->type_name() << std::endl;
  }
}

void bwm_observer_mgr::move_to_corr()
{
  if (!corr_list_.size())
  {
    std::cerr << "In bwm_observer_mgr::move_to_corr() -"
             << " no correspondences to move to\n";
    return;
  }
  bwm_corr_sptr corr = corr_list_[0];
  std::vector<bwm_observer_cam*> obs = corr->observers();
  for (std::vector<bwm_observer_cam*>::iterator oit = obs.begin();
       oit != obs.end(); ++oit)
  {
    vgl_point_2d<double> p;
    //observer has a match so can zoom
    if (corr->match(*oit, p))
    {
      float x = static_cast<float>(p.x());
      float y = static_cast<float>(p.y());
      (*oit)->move_to_point(x, y);
    }
  }
}

void bwm_observer_mgr::adjust_camera_offsets()
{
  if (!corr_list_.size())
    return;
  bwm_corr_sptr corr = corr_list_[0];
  std::vector<bwm_observer_cam*> obs = corr->observers();
  std::vector<vpgl_rational_camera<double> > rcams;
  std::vector<vgl_point_2d<double> > cpoints;
  for (std::vector<bwm_observer_cam*>::iterator oit = obs.begin();
       oit != obs.end(); ++oit)
  {
    if ((*oit)->type_name() != "bwm_observer_rat_cam")
      continue;
    bwm_observer_rat_cam* obscr =
      static_cast<bwm_observer_rat_cam*>(*oit);
    rcams.push_back(obscr->camera());
    vgl_point_2d<double> p;
    if (corr->match(*oit, p))
      cpoints.push_back(p);
  }
  if (cpoints.size()!=rcams.size())
  {
    std::cerr << "In bwm_observer_mgr::adjust_image_offsets - "
             << " inconsistent number of points and cameras\n";
    return;
  }

  std::cout << "Executing adjust image offsets\n";
  std::vector<vgl_vector_2d<double> > cam_trans;
  vgl_point_3d<double> intersection;
  if (!vpgl_rational_adjust_onept::adjust(rcams, cpoints, cam_trans,
                                                intersection))
  {
    std::cerr << "In bwm_observer_rat_cam::adjust_image_offsets - "
             << " adjustment failed\n";
    return;
  }

  vgl_plane_3d<double> world_plane(0,0,1,-intersection.z());
  std::vector<vgl_vector_2d<double> >::iterator ti = cam_trans.begin();
  std::vector<bwm_observer_cam*>::iterator oit = obs.begin();
  for (; oit != obs.end() && ti != cam_trans.end(); ++oit, ++ti)
  {
    if ((*oit)->type_name() != "bwm_observer_rat_cam")
      continue;
    bwm_observer_rat_cam* obsrc =
      static_cast<bwm_observer_rat_cam*>(*oit);
    std::cout << "Shifting camera[" << obsrc->camera_path() <<  "]:\n("
             << (*ti).x() << ' ' << (*ti).y() << "):\n point_3d("
             << intersection.x() << ' ' << intersection.y()
             << ' ' << intersection.z() << ")\n";
    obsrc->shift_camera((*ti).x(), (*ti).y());
    // here is where we would set the terrain plane and maybe not
    // do anything to the projection plane of each observer.
    // but this approach works for now.
    obsrc->set_proj_plane(world_plane);
    obsrc->update_all();
  }

  // send the objects in the world the fact that they need to redisplay
  std::vector<bwm_observable_sptr> objs = bwm_world::instance()->objects();
  for (std::vector<bwm_observable_sptr>::iterator oit = objs.begin();
       oit != objs.end(); ++oit)
    (*oit)->send_update();

  //
  // now that the 3-d intersection is available, the correspondence mode
  // should be changed to "world_to_image." The mode should only be
  // changed back to "image_to_image" if a new image is added to the
  // site and the intersection point is re-computed
  //
  for (std::vector<bwm_corr_sptr>::iterator cit = corr_list_.begin();
       cit != corr_list_.end(); ++cit)
  {
    (*cit)->set_mode(false); //mode is set to world_to_image
    (*cit)->set_world_pt(intersection);
  }

  bwm_world::instance()->set_world_pt(intersection);
  this->set_corr_mode(bwm_observer_mgr::WORLD_TO_IMAGE);
}

void bwm_observer_mgr::find_terrain_points(std::vector<vgl_point_3d<double> >& points)
{
  if (!terrain_corr_list_.size())
    return;

  for (unsigned i=0; i<terrain_corr_list_.size(); i++) {
    bwm_corr_sptr corr = terrain_corr_list_[i];
    std::vector<bwm_observer_cam*> obs = corr->observers();
    std::vector<vpgl_rational_camera<double> > rcams;
    std::vector<vgl_point_2d<double> > cpoints;
    for (std::vector<bwm_observer_cam*>::iterator oit = obs.begin();
         oit != obs.end(); ++oit)
    {
      if ((*oit)->type_name() != "bwm_observer_rat_cam")
        continue;
      bwm_observer_rat_cam* obscr =
        static_cast<bwm_observer_rat_cam*>(*oit);
      rcams.push_back(obscr->camera());
      vgl_point_2d<double> p;
      if (corr->match(*oit, p))
        cpoints.push_back(p);
    }
    if (cpoints.size()!=rcams.size())
    {
      std::cerr << "In bwm_observer_mgr::adjust_image_offsets - "
               << " inconsistent number of points and cameras\n";
      return;
    }

    std::cout << "Executing adjust image offsets\n";
    std::vector<vgl_vector_2d<double> > cam_trans;
    vgl_point_3d<double> intersection;
    if (!vpgl_rational_adjust_onept::adjust(rcams, cpoints, cam_trans,
                                                  intersection))
    {
      std::cerr << "In bwm_observer_rat_cam::find_terrain_points - "
               << " adjustment failed\n";
      return;
    }
    points.push_back(intersection);
  }
  terrain_corr_list_.clear();
}

//: find all selected polygons across all observers
std::vector<bwm_observable_sptr> bwm_observer_mgr::
all_selected_observables(std::string const& soview_type) const
{
  std::vector<bwm_observable_sptr> sel_obsbls;
  std::vector<bwm_observer_cam*> obs_cam = this->observers_cam();

  for (std::vector<bwm_observer_cam*>::iterator oit = obs_cam.begin();
       oit != obs_cam.end(); ++oit) {
    bwm_observer_cam* obs = *oit;
    if (!obs) {
    std::cout << "null observer in all_selected_observables\n";
    return sel_obsbls;//empty
    }
    std::vector<vgui_soview*> soviews = obs->get_selected_soviews();
    for (std::vector<vgui_soview*>::iterator sit = soviews.begin();
         sit != soviews.end(); ++sit) {
      if ((*sit)->type_name().compare(soview_type)==0) {
        unsigned face_id;
        bwm_observable_sptr obj=(*oit)->find_object(soviews[0]->get_id(),face_id);
        if (obj) sel_obsbls.push_back(obj);
      }
    }
  }


  return sel_obsbls;
}

//: requires exactly two selected vertices each in a unique site
bool bwm_observer_mgr::add_3d_corr_vertex()
{
  // not implemented yet
  return true;
}

//: requires exactly two selected polygons each in a unique site. Corresponds centroids of the polygons
bool bwm_observer_mgr::add_3d_corr_centroid()
{
  std::vector<bwm_observable_sptr> obs =
    this->all_selected_observables("bgui_vsol_soview2D_polygon");
  if (obs.size()!=2) {
    std::cout << "must have exactly two polygons selected\n";
    return false;
  }
  bwm_observable_sptr obs0 = obs[0], obs1 = obs[1];
  if (obs0->site() == obs1->site()) {
    std::cout << "each polygon must be from a different site\n";
    return false;
  }
  std::map<int, vsol_polygon_3d_sptr> polys0 = obs0->extract_faces();
  std::map<int, vsol_polygon_3d_sptr> polys1 = obs1->extract_faces();
  if (polys0.size() !=1 || polys1.size() !=1) {
   std::cout << "must be exactly 1 polygon in each observable\n";
    return false;
  }
  std::map<int, vsol_polygon_3d_sptr>::iterator pit = polys0.begin();
  vsol_polygon_3d_sptr poly0 = (*pit).second;
  pit = polys1.begin();
  vsol_polygon_3d_sptr poly1 = (*pit).second;
  unsigned n0 = poly0->size(), n1 = poly1->size();
  if (!n0||!n1) {
   std::cout << "poly with no vertices in add_3d_corr";
    return false;
  }
  double xc0 = 0.0, yc0 = 0.0, zc0 = 0.0;
  double xc1 = 0.0, yc1 = 0.0, zc1 = 0.0;
  for (unsigned i = 0; i<n0; ++i) {
    vsol_point_3d_sptr vi = poly0->vertex(i);
    xc0 += (*vi).x(); yc0 += (*vi).y(); zc0 += (*vi).z();
  }
  for (unsigned i = 0; i<n1; ++i) {
    vsol_point_3d_sptr vi = poly1->vertex(i);
    xc1 += (*vi).x(); yc1 += (*vi).y(); zc1 += (*vi).z();
  }
  bwm_3d_corr_sptr corr_3d = new bwm_3d_corr();
  corr_3d->set_match(obs0->site(), xc0/n0, yc0/n0, zc0/n0);
  corr_3d->set_match(obs1->site(), xc1/n1, yc1/n1, zc1/n1);
  site_to_site_corr_list_.push_back(corr_3d);
  return true;
}

//: save 3d_corrs
void bwm_observer_mgr::save_3d_corrs() const
{
  std::string path = "";
  std::string ext = "*.cor";
  vgui_dialog corr_dlg("Save 3d Correspondences");
  corr_dlg.file("Corr file", ext, path);
  if (!corr_dlg.ask())
    return;
  bwm_observer_mgr::save_3d_corrs(path, site_to_site_corr_list_);
}

void bwm_observer_mgr::save_3d_corrs(std::string const& path,
                                     std::vector<bwm_3d_corr_sptr> const& corrs)
{
  std::ofstream os(path.c_str());
  if (!os.is_open())
  {
    std::cout << "couldn't open 3d_corr file path\n";
    return ;
  }
  unsigned n = corrs.size();
  if (!n) {
    std::cout << "no 3d_corrs to save\n";
    return ;
  }
  os << "Ncorrs: " << n << '\n';
  for (unsigned i = 0; i<n; ++i)
    os << *corrs[i];
}

//: load 3d_corrs
void bwm_observer_mgr::load_3d_corrs()
{
  std::string path = "";
  std::string ext = "*.cor";
  vgui_dialog corr_dlg("Load 3d Correspondences");
  corr_dlg.file("Corr file", ext, path);
  if (!corr_dlg.ask())
    return;
  bwm_observer_mgr::load_3d_corrs(path, site_to_site_corr_list_);
}

void bwm_observer_mgr::load_3d_corrs(std::string const& path,
                                     std::vector<bwm_3d_corr_sptr>& corrs)
{
  std::ifstream is(path.c_str());
  if (!is.is_open())
  {
    std::cout << "couldn't open 3d_corr file path\n";
    return ;
  }
  //clear out any existing correspondences
  corrs.clear();
  std::string temp, temp1, temp2;
  is >> temp;
  if (temp!="Ncorrs:") {
    std::cout << "error in 3d_corr file 1\n";
    return;
  }
  unsigned n_corrs = 0;
  is >> n_corrs;
  for (unsigned i = 0; i<n_corrs; ++i) {
    is >> temp >> temp1 >> temp2;
    if (temp2!="Sites:") {
      std::cout << "error in 3d_corr file 2\n";
      return;
    }
    unsigned n_sites = 0;
    is >> n_sites;
    double x = 0.0, y = 0.0, z = 0.0;
    std::string site;
    bwm_3d_corr_sptr corr = new bwm_3d_corr();
    for (unsigned s = 0; s<n_sites; ++s) {
      is >> temp >> temp1;
      if (temp != "Site[") {
        std::cout << "error in 3d_corr file 3\n";
        return;
      }
      site = temp1;
      is >> temp >> temp1;
      if (temp1 != "X:") {
        std::cout << "error in 3d_corr file 4\n";
        return;
      }
      is >> x;
      is >> temp;
      if (temp != "Y:") {
        std::cout << "error in 3d_corr file 5\n";
        return;
      }
      is >> y;
      is >> temp;
      if (temp != "Z:") {
        std::cout << "error in 3d_corr file 6\n";
        return;
      }
      is >> z;
      is >> temp;//eat up the trailing ")"
      corr->set_match(site, x, y, z);
    }
    corrs.push_back(corr);
  }
}
