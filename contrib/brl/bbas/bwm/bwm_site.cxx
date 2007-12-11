#include "bwm_site.h"
//:
// \file
#include "bwm_corr.h"

#include <vcl_utility.h>
#include <vcl_cassert.h>
#include <vsl/vsl_basic_xml_element.h>
#include <vsol/vsol_point_2d.h>

//: destructor
bwm_site::~bwm_site()
{
  // delete the tableau structures
  for (unsigned i=0; i<tableaus_.size(); i++) {
    if (tableaus_[i])
      delete tableaus_[i];
  }
}

void bwm_site::add(vcl_vector<vcl_string> images,
                   vcl_vector<bool> is_pyr, vcl_vector<bool> is_act,
                   vcl_vector<vcl_string> levels,
                   vcl_vector<vcl_pair<vcl_string, vcl_string> > objects,
                   vsol_point_3d_sptr lvcs)
{
  image_paths_ = images;
  pyr_ = is_pyr;
  act_ = is_act;
  pyr_levels_ = levels;

  // insert the new objects to the list
  for (unsigned i=0; i < objects.size(); i++) {
    objects_.push_back(objects[i]);
  }

  lvcs_ = lvcs;
}

void bwm_site::x_write(vcl_ostream& s)
{
  vsl_basic_xml_element site(SITE_TAG);
  site.add_attribute("name", name_);
  site.x_write_open(s);

  vsl_basic_xml_element site_dir(SITE_HOME_TAG);
  site_dir.append_cdata(path_);
  site_dir.x_write(s);

  vsl_basic_xml_element pyr_path(PYRAMID_EXE_TAG);
  pyr_path.append_cdata(pyr_exe_path_);
  pyr_path.x_write(s);

  vsl_basic_xml_element tableaus(TABLEAUS_TAG);
  tableaus.x_write_open(s);
  for (unsigned i=0; i<tableaus_.size(); i++)
  {
    bwm_io_tab_config* t = tableaus_[i];

    if (t->type_name.compare(IMAGE_TABLEAU_TAG) == 0)
    {
      bwm_io_tab_config_img* img_tab = static_cast<bwm_io_tab_config_img* > (t);
      bool active = img_tab->status;
      vsl_basic_xml_element tab(IMAGE_TABLEAU_TAG);
      tab.add_attribute("name", img_tab->name);
      tab.add_attribute("status", active == true ? "active" : "inactive");
      tab.x_write_open(s);

      vsl_basic_xml_element img_path(IMAGE_PATH_TAG);
      img_path.append_cdata(img_tab->img_path);
      img_path.x_write(s);
      tab.x_write_close(s);
    }
    else if (t->type_name.compare(CAMERA_TABLEAU_TAG) == 0)
    {
      bwm_io_tab_config_cam* cam_tab = static_cast<bwm_io_tab_config_cam* > (t);
      bool active = cam_tab->status;
      vsl_basic_xml_element tab(CAMERA_TABLEAU_TAG);
      tab.add_attribute("name", cam_tab->name);
      tab.add_attribute("status", active == true ? "active" : "inactive");
      tab.x_write_open(s);

      vsl_basic_xml_element img_path(IMAGE_PATH_TAG);
      img_path.append_cdata(cam_tab->img_path);
      img_path.x_write(s);

      vsl_basic_xml_element camera(CAMERA_PATH_TAG);
      camera.add_attribute("type", cam_tab->cam_type);
      camera.append_cdata(cam_tab->cam_path);
      camera.x_write(s);
      tab.x_write_close(s);
    }
  }
  tableaus.x_write_close(s);

  // add the LVCS
  if (lvcs_) {
    vsl_basic_xml_element lvcs(LVCS_TAG);
    lvcs.add_attribute("lat", lvcs_->x());
    lvcs.add_attribute("lon", lvcs_->y());
    lvcs.add_attribute("elev", lvcs_->z());
    lvcs.x_write(s);
  }

  // add the object files
  vsl_basic_xml_element objects(OBJECTS_TAG);
  objects.x_write_open(s);
  for (unsigned i=0; i<objects_.size(); i++) {
    if (objects_[i].first.size() > 0) {
      vsl_basic_xml_element obj(OBJECT_TAG);
      obj.add_attribute("type",objects_[i].second);
      obj.append_cdata(objects_[i].first);
      obj.x_write(s);
    }
  }
  objects.x_write_close(s);

  // write out the correspondence list
  if (this->corresp_.size() > 0)
  {
    vsl_basic_xml_element xml_element(CORRESPONDENCES_TAG);
    xml_element.add_attribute("mode", corr_mode_);
    xml_element.add_attribute("type", corr_type_);
    xml_element.x_write_open(s);

    if (corr_mode_.compare("WORLD_TO_IMAGE") == 0)
      assert(corresp_.size() == corresp_world_pts_.size());

    for (unsigned i=0; i< this->corresp_.size(); i++) {
      /*bwm_corr_sptr corr = corresp_[i];
      vcl_cout << corr->num_matches() << vcl_endl;
      corr->x_write(s);*/
      vsl_basic_xml_element corr(CORRESP_TAG);
      corr.x_write_open(s);

     if (corr_mode_.compare("WORLD_TO_IMAGE") == 0) {
      vsl_basic_xml_element xml_element(CORRESP_WORLD_PT_TAG);
      xml_element.add_attribute("X", corresp_world_pts_[i].x());
      xml_element.add_attribute("Y", corresp_world_pts_[i].y());
      xml_element.add_attribute("Z", corresp_world_pts_[i].z());
      xml_element.x_write(s);
     }

     vcl_vector<vcl_pair<vcl_string, vsol_point_2d> > c_list = corresp_[i];
    // int i=0;
     for (unsigned c=0; c<c_list.size(); c++) {
       vsl_basic_xml_element corr_elm(CORRESP_ELM_TAG);
       corr_elm.x_write_open(s);

       vcl_pair<vcl_string, vsol_point_2d> pair = c_list[c];
       vsl_basic_xml_element corr_tab(CORR_CAMERA_TAG);
       corr_tab.append_cdata(pair.first);
       corr_tab.x_write(s);

       vsl_basic_xml_element xml_element(CORRESP_PT_TAG);
       xml_element.add_attribute("X", pair.second.x());
       xml_element.add_attribute("Y", pair.second.y());
       xml_element.x_write(s);
       corr_elm.x_write_close(s);
     }
     corr.x_write_close(s);
    }
    xml_element.x_write_close(s);
  }

  site.x_write_close(s);
}
