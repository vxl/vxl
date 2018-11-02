#include <iostream>
#include <cstdio>
#include <sstream>
#include "bwm_site_mgr.h"
//:
// \file
#include <bwm/io/bwm_site_sptr.h>
#include <bwm/io/bwm_site.h>
#include "bwm_observer_mgr.h"
#include "bwm_tableau_mgr.h"
#include "bwm_tableau_video.h"

#include "bwm_observable_mesh.h"
#include "bwm_world.h"
#include "bwm_def.h"
#include "algo/bwm_utils.h"
#include "video/bwm_video_site_io.h"
#include "video/bwm_video_corr.h"
#include "video/bwm_video_cam_istream.h"
#include <vpgl/vpgl_perspective_camera.h>
#include <bpgl/algo/bpgl_project.h>
#include <depth_map/depth_map_scene.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_quaternion.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vul/vul_file.h>
#include <vul/vul_string.h>
#include <vgui/vgui_tableau_sptr.h>

#ifdef WIN32
 #define _LIB
#endif
#include <expatpp.h> // for accessing methods in parent class of bwm_io_config_parser

bwm_site_mgr* bwm_site_mgr::instance_ = nullptr;

bwm_site_mgr* bwm_site_mgr::instance()
{
  if (!instance_) {
    instance_ = new bwm_site_mgr();
  }
  return bwm_site_mgr::instance_;
}

bwm_site_mgr::bwm_site_mgr(): site_name_(""), site_dir_(""), pyr_exe_(""),
                              camera_path_(""), video_path_("")
{
#ifdef HAS_MFC
  site_create_process_ = new bwm_site_process();
#endif
  object_types_.resize(OBJ_UNDEF);
  object_types_[MESH_FEATURE] = BWM_MESH_FEATURE_STR;
  object_types_[MESH_IMAGE_PROCESSING] = BWM_MESH_IMAGE_PROCESSING_STR;
  object_types_[MESH_TERRAIN] = BWM_MESH_TERRAIN_STR;
  object_types_[VSOL] = BWM_OBJ_VSOL_STR;
}

bwm_site_mgr::~bwm_site_mgr()
{
#ifdef HAS_MFC
  delete site_create_process_;
#endif
}

void bwm_site_mgr::init_site()
{
  // delete the active tableaux
  for (unsigned i=0; i<active_tableaus_.size(); i++)
    delete active_tableaus_[i];
  active_tableaus_.clear();

  // delete the inactive tableaux
  for (unsigned i=0; i<inactive_tableaus_.size(); i++)
    delete inactive_tableaus_[i];
  inactive_tableaus_.clear();

  // clear the objects
  site_objs_.clear();
#ifdef HAS_MFC
  if (site_create_process_) {
    delete site_create_process_;
    this->site_create_process_ = new bwm_site_process();
  }
#endif
  site_name_="";
  site_dir_="";
  pyr_exe_="";

  bwm_observer_mgr::instance()->clear();
  bwm_world::instance()->clear();

  timer_.mark();
}

void bwm_site_mgr::create_site_dialog(vgui_dialog_extensions &site_dialog,
                                      std::string &site_name,
                                      std::string &site_dir,
                                      std::string &pyr_exe_dir,
                                      std::vector<std::string> &files,
                                      bool* pyr_v, bool* act_v,
                                      std::vector<std::string> &pyr_levels,
                                      std::vector<std::string> &objs,
                                      int* choices,
                                      double &lat, double &lon, double &elev)
{
  std::string ext;
  site_dialog.field("SITE NAME:", site_name);
  site_dialog.line_break();
  site_dialog.dir("DIRECTORY:", ext, site_dir);
  site_dialog.line_break();
  site_dialog.file("Pyramid exe path:", ext, pyr_exe_dir);
  site_dialog.line_break();
  //site_dialog.line_break();
  site_dialog.set_modal(true);

  // add a bunch of images
  site_dialog.message("Please Choose the Images for this site:");
  site_dialog.line_break();
  for (unsigned i=0; i<files.size(); i++) {
    site_dialog.file("Image Path:", ext, files[i]);
    site_dialog.checkbox("Do Pyramid?", pyr_v[i]);
    site_dialog.field("Levels:", pyr_levels[i]);
    site_dialog.checkbox("Is Active?", act_v[i]);
    site_dialog.line_break();
  }

  site_dialog.message("Please Choose the objects to add to the site:");
  site_dialog.line_break();

  std::vector<std::string> obj_types(4);
  for (unsigned i=0; i<objs.size(); i++) {
    site_dialog.file("Enter input object:", ext, objs[i]);
    site_dialog.choice("Type:", object_types_, choices[i]);
    site_dialog.line_break();
  }

  //site_dialog.line_break();
  site_dialog.message("Enter the LVCS origin for this site:");
  site_dialog.line_break();
  site_dialog.field("Lat:", lat);
  site_dialog.field("Lon:", lon);
  site_dialog.field("Elev:", elev);
  site_dialog.line_break();
  site_dialog.set_ok_button("CREATE");
}


//: create a dialog box to create site to add images, objects, etc..
void bwm_site_mgr::create_site()
{
  vgui_dialog_extensions site_dialog("CrossCut Site Creation");

  std::string site_name, site_dir, pyr_exe_path;

  int num_images = 3;
  std::vector<std::string> files(num_images);
  bool pyr[3] = {false, false, false};
  bool act[3] = {true, true, true};
  std::vector<std::string> levels(num_images, "7");

  int num_objs = 3;
  std::vector<std::string> objs(num_objs);
  int choices[3] = {0, 0, 0};
  double lat=0.0, lon=0.0, elev=0.0;

  create_site_dialog(site_dialog, site_name, site_dir, pyr_exe_path, files, pyr,
                     act, levels, objs, choices, lat, lon, elev);

  if (!site_dialog.ask()) {
    return;
  }
  else
  {
    // collect the parameters
    std::cout << "name:" << site_name << '\n'
             << "dir:" << site_dir << std::endl;

    // make sure site name is filled
    while (site_name.size() == 0) {
      vgui_dialog error ("Error");
      error.message ("Please enter a valid SITE NAME!            " );
      error.ask();
      if (! site_dialog.ask())
        return;
    }

    // make sure site directory is filled and valid
    while ((site_dir.size() == 0) || !vul_file::is_directory(site_dir)) {
      vgui_dialog error ("Error");
      error.message ("Please enter a valid SITE DIRECTORY!            " );
      error.ask();
      if (! site_dialog.ask())
        return;
    }

    // check if the levels are numbers
    bool not_int = true;
    while (not_int) {
      not_int = false;
      for (unsigned i=0; i<levels.size(); i++) {
        int l = vul_string_atoi(levels[i].c_str());
        if (l == 0) {
          not_int = true;
          break;
        }
      }
      if (not_int) {
        vgui_dialog error ("Error");
        error.message ("Please enter an integer level value!            " );
        error.ask();
        if (! site_dialog.ask())
          return;
      }
      else {
        not_int = false;
      }
    }
  }

  std::vector<bool> pyramid;
  std::vector<bool> active;
  for (unsigned j=0; j<files.size(); j++) {
    pyramid.push_back(pyr[j]);
    active.push_back(act[j]);
  }

  std::vector<std::pair<std::string, std::string> > objects;
  for (unsigned obj=0; obj<objs.size(); obj++) {
    std::pair<std::string, std::string> pair(objs[obj], object_types_[choices[obj]]);
    objects.push_back(pair);
  }

  bwm_site_sptr site = new bwm_site(site_name, site_dir, files, pyramid, active,
                                    levels, objects, new vsol_point_3d(lat, lon, elev));

  // temporarily setting the exe path, will find a better solution later - Gamze
  site->pyr_exe_path_ = pyr_exe_path;
#ifdef HAS_MFC
  site_create_process_->set_site(site);
  site_create_process_->StartBackgroundTask();
#else
  vgui_dialog error ("Error");
  error.message ("Site is not created! MFC (Windows platforms) is needed for this task!" );
  error.ask();
  if (! site_dialog.ask())
    return;
  std::cout << "Site is not created MFC (Windows) is needed for this task!" << std::endl;
#endif
}

//: create a dialog box to create site to add images, objects, etc..
void bwm_site_mgr::edit_site()
{
  bwm_io_config_parser* parser = nullptr;
  parser = parse_config();
  if (parser == nullptr) {
    std::cerr << "Site File is not a valid XML site!\n";
    return;
  }

  vgui_dialog_extensions site_edit_dialog("Edit World Model Site");
  bwm_site_sptr site = parser->site();

  // new creation vars
  int num_images = 3;
  std::vector<std::string> files(num_images);
  bool pyr[3] = {false, false, false};
  bool act[3] = {true, true, true};
  std::vector<std::string> levels(num_images, "7");

  int num_objs = 3;
  std::vector<std::string> objs(num_objs);
  int choices[3] = {0, 0, 0};
  double lat=0.0, lon=0.0, elev=0.0;

  // previously created obj vars
  bool act_old[30];
  bool tab_remove[30];
  std::vector<std::string> cam;
  std::string ext;

  site_edit_dialog.file("Pyramid Exe Path:", ext, site->pyr_exe_path_);
  site_edit_dialog.line_break();

  // first place the existing tableaux on the dialog
  std::vector<bwm_io_tab_config* > tableaus;
  site->tableaus(tableaus);
  cam.resize(tableaus.size());
  if (tableaus.size() > 0)
  {
    site_edit_dialog.message("EXISTING IMAGES:");
    site_edit_dialog.line_break();

    for (unsigned i=0; i<tableaus.size(); i++)
    {
      bwm_io_tab_config* t = tableaus[i];
      if (t->type_name.compare(IMAGE_TABLEAU_TAG) == 0)
      {
        bwm_io_tab_config_img* img_tab = static_cast<bwm_io_tab_config_img* > (t);
        bool active = img_tab->status;
        std::string name = img_tab->name;
        std::string path = img_tab->img_path;
        site_edit_dialog.message((" -- "+path).c_str());
        tab_remove[i] = false;
        site_edit_dialog.checkbox("Remove", tab_remove[i]);
        act_old[i] = active;
        site_edit_dialog.checkbox("Active", act_old[i]);
#if 0
        std::string ext = "*.RPG";
        site_edit_dialog.file("Add Camera:", ext, cam[i]);
#endif // 0
        site_edit_dialog.line_break();
      }
      else if (t->type_name.compare(CAMERA_TABLEAU_TAG) == 0)
      {
        bwm_io_tab_config_cam* cam_tab = static_cast<bwm_io_tab_config_cam* > (t);
        bool active = cam_tab->status;
        site_edit_dialog.message((" -- "+cam_tab->img_path).c_str());
        tab_remove[i] = false;
        site_edit_dialog.checkbox("Remove ", tab_remove[i]);
        act_old[i] = active;
        site_edit_dialog.checkbox("Active ", act_old[i]);
        site_edit_dialog.line_break();
      }
    }
  }

  // add a bunch of images
  site_edit_dialog.message("ADD NEW:");
  site_edit_dialog.line_break();
  for (unsigned i=0; i<files.size(); i++) {
    std::string ext;
    site_edit_dialog.file("Image Path:", ext, files[i]);
    site_edit_dialog.checkbox("Do Pyramid?", pyr[i]);
    site_edit_dialog.field("Levels:", levels[i]);
    site_edit_dialog.checkbox("Is Active?", act[i]);
    site_edit_dialog.line_break();
  }

  site_edit_dialog.line_break();

  // put the existing objects
  std::vector<std::pair<std::string, std::string> > object_paths;
  bool obj_remove[30];
  parser->site()->objects(object_paths);

  if (object_paths.size() > 0) {
    site_edit_dialog.message("EXISTING OBJECTS:");
    site_edit_dialog.line_break();

    for (unsigned i=0; i<object_paths.size(); i++) {
      site_edit_dialog.message((" -- " + object_paths[i].first).c_str());
      std::string object_type = object_paths[i].second;
      site_edit_dialog.message(("Type: " + object_type).c_str());
      obj_remove[i] = false;
      site_edit_dialog.checkbox("Remove ", obj_remove[i]);
      site_edit_dialog.line_break();
    }
  }
  // create new objects
  site_edit_dialog.message("ADD OBJECTS:");
  site_edit_dialog.line_break();
  for (unsigned i=0; i<objs.size(); i++) {
    site_edit_dialog.file("Object:", ext, objs[i]);
    site_edit_dialog.choice("Type:", object_types_, choices[i]);
    site_edit_dialog.line_break();
  }

  // lvcs
  site_edit_dialog.line_break();
  site_edit_dialog.message("LVCS origin for this site:");
  site_edit_dialog.line_break();

  lat = lon = elev = 0.0;
  if (parser->site()->lvcs_) {
    lat = parser->site()->lvcs_->x();
    lon = parser->site()->lvcs_->y();
    elev = parser->site()->lvcs_->z();
  }
  site_edit_dialog.field("Lat:", lat);
  site_edit_dialog.field("Lon:", lon);
  site_edit_dialog.field("Elev:", elev);

  site_edit_dialog.line_break();
  site_edit_dialog.set_ok_button("EDIT");
  if (!site_edit_dialog.ask()) {
    return;
  }
  else
  {
    // check if the level values are integer

    // create a removal list for the tableaux that are marked for removal
    std::vector<unsigned> removal;
    for (unsigned t=0; t<tableaus.size(); t++) {
      tableaus[t]->status = act_old[t];
      if (tab_remove[t])
        removal.push_back(t);
    }
    site->remove_ = removal;

    // delete objects that are marked for removal
    std::vector<std::pair<std::string, std::string> > undeleted_objs;
    bool deleted = false;
    for (unsigned i=0; i<object_paths.size(); i++) {
      if (!obj_remove[i]) {
        undeleted_objs.push_back(object_paths[i]);
      }
      else
        deleted = true;
    }
    if (deleted) {
      site->objects_.clear();
      site->objects_ = undeleted_objs;
    }

    // create an updated site
    std::vector<bool> pyramid;
    std::vector<bool> active;
    for (unsigned j=0; j<files.size(); j++) {
      pyramid.push_back(pyr[j]);
      active.push_back(act[j]);
    }

    std::vector<std::pair<std::string, std::string> > objects;
    for (unsigned obj=0; obj<objs.size(); obj++) {
      std::pair<std::string, std::string> pair(objs[obj], object_types_[choices[obj]]);
      objects.push_back(pair);
    }
#if 0
    std::vector<std::vector<std::pair<std::string, vsol_point_2d> > > corr = parser->correspondences();
    for (unsigned i=0; i<corr.size(); i++) {
      bwm_corr_sptr c = new bwm_corr();
      if (parser->corresp_mode().compare("IMAGE_TO_IMAGE") == 0)
        c->set_mode(true);
      else {
        c->set_mode(false);
        c->set_world_point(parser->corresp_world_pts()[i]);
      }
    }
    site->corresp_mode = parser->corresp_mode();
    site->corr_type_ = parser->corresp_type();
    // std::vector<vsol_point_3d> corresp_world_pts() {return corresp_world_pts_; }
#endif // 0

    site->add(files, pyramid, active, levels, objects, new vsol_point_3d(lat, lon, elev));
#ifdef HAS_MFC
    site_create_process_->set_site(site);
    site_create_process_->StartBackgroundTask();
#endif
  }
}


void bwm_site_mgr::load_site()
{
  bwm_io_config_parser* parser = parse_config();

  if (parser)
  {
    init_site();
    bwm_site_sptr site = parser->site();
    std::vector<bwm_io_tab_config* > tableaus;
    site->tableaus(tableaus);

    site_name_ = site->name_;
    site_dir_ = site->path_;
    pyr_exe_ = site->pyr_exe_path_;

    // get the lvcs
    vsol_point_3d_sptr lvcs = site->lvcs_;
    // if LVCS is not set, do not use it
    if (!lvcs||*lvcs == vsol_point_3d(0, 0, 0))
      lvcs = nullptr;
    else {
      double lat = lvcs->x();
      double lon = lvcs->y();
      double elev = lvcs->z();
      bwm_world::instance()->set_lvcs(lat, lon, elev);
    }

    // create the active tableaux
    for (unsigned i=0; i<tableaus.size(); i++)
    {
      bwm_io_tab_config* t = tableaus[i];
      if (t->status) {
        // create an active tableau
        bwm_tableau_img* tab = tableau_factory_.create_tableau(t);
        bwm_tableau_mgr::instance()->add_tableau(tab, t->name);
        active_tableaus_.push_back(t->clone());
      }
      else // inactive tableau
        inactive_tableaus_.push_back(t->clone());
    }

    // create the correspondences
    std::vector<std::vector<std::pair<std::string, vsol_point_2d> > > corresp;
    corresp = site->corresp_;
    if (corresp.size() > 0) {
      std::string mode = site->corr_mode_;
      std::string type = site->corr_type_;

      if (type.compare("MULTIPLE") == 0)
        bwm_observer_mgr::instance()->set_n_corrs(bwm_observer_mgr::MULTIPLE_CORRS);
      else if (type.compare("SINGLE") == 0)
        bwm_observer_mgr::instance()->set_n_corrs(bwm_observer_mgr::SINGLE_PT_CORR);
      else
        std::cerr << "ERROR: Undefined Correspondence type=" << type << '\n';

      if (mode == "WORLD_TO_IMAGE") {
        if (corresp.size() > 0) {
          if (site->corresp_world_pts_.size() > 0)
            bwm_world::instance()->set_world_pt(site->corresp_world_pts_[0].get_p());
          else
            std::cerr << "There is something wrong, the more is W-to-I but there is no world point\n";
        }
        bwm_observer_mgr::instance()->set_corr_mode(bwm_observer_mgr::WORLD_TO_IMAGE);
      }
      else if (mode == "IMAGE_TO_IMAGE") {
        bwm_observer_mgr::instance()->set_corr_mode(bwm_observer_mgr::IMAGE_TO_IMAGE);
      }

      for (unsigned i=0; i<corresp.size(); i++)
      {
        bwm_corr_sptr corr = new bwm_corr();
        std::vector<std::pair<std::string, vsol_point_2d> > elm = corresp[i];

        if (mode == "WORLD_TO_IMAGE") {
          corr->set_mode(false);
          corr->set_world_pt(site->corresp_world_pts_[i].get_p());
          //sets the same pt each time FIXME -JLM
          //bwm_world::instance()->set_world_pt(corr->world_pt());
        }
        else if (mode == "IMAGE_TO_IMAGE") {
          corr->set_mode(true);
        }

        std::string tab_name;
        double X, Y;
        for (unsigned j=0; j<elm.size(); j++) {
          tab_name = elm[j].first;
          X = elm[j].second.x();
          Y = elm[j].second.y();
          bwm_tableau_mgr::instance()->add_corresp(tab_name, corr, X, Y);
#if 0 // commented out
          vgui_tableau_sptr tab = this->find_tableau(tab_name);
          if (tab) {
            if ((tab->type_name().compare("bwm_tableau_proj_cam") == 0) ||
                (tab->type_name().compare("bwm_tableau_rat_cam") == 0)) {
              bwm_tableau_cam* tab_cam = static_cast<bwm_tableau_cam*> (tab.as_pointer());
              bwm_observer_cam* obs = tab_cam->observer();
              if (obs) {
                corr->set_match(obs, X, Y);
                obs->add_cross(X, Y, 3);
              }
            }
          }
#endif // 0
        }
        bwm_observer_mgr::instance()->set_corr(corr);
      }
    }
    // create the objects
    std::vector<std::pair<std::string, std::string> > objs;
    site->objects(objs);
    for (unsigned i=0; i<objs.size(); i++)
    {
      std::string path = objs[i].first;
      std::string type = objs[i].second;
      if (path.size() > 0)
      {
        if (!vul_file::exists(path))
          std::cerr << "ERROR: The object file \"" << path << "\" could not be found!\n";
        else {
          if (type.compare(object_types_[VSOL]) == 0) {
            // will be implemented later!!!
          }
          else {
            // comes here if it is a mesh
            bwm_observable_mesh_sptr mesh = new bwm_observable_mesh();
            mesh->load_from(path);
            if (mesh) {
              bwm_observable_mesh_sptr obj = new bwm_observable_mesh();
              obj->set_site(site_name_);
              bwm_observer_mgr::instance()->attach(obj);
              if (type.compare(object_types_[MESH_FEATURE]) == 0)
                obj->set_mesh_type(bwm_observable_mesh::BWM_MESH_FEATURE);
              else if (type.compare(object_types_[MESH_IMAGE_PROCESSING]) == 0)
                obj->set_mesh_type(bwm_observable_mesh::BWM_MESH_IMAGE_PROCESSING);
              else if (type.compare(object_types_[MESH_TERRAIN]) == 0)
                obj->set_mesh_type(bwm_observable_mesh::BWM_MESH_TERRAIN );
              obj->set_object(mesh->get_object()->clone());
              bwm_world::instance()->add(obj);
            }
          }
        }
      }
    }
    delete parser;
    bwm_tableau_mgr::instance()->redraw();
  }
}

//: saves the site to an XML file
void bwm_site_mgr::save_site()
{
  bwm_site_sptr site = new bwm_site();

  if ((this->site_name_.size() > 0) &&
      (this->site_dir_.size() > 0) &&
      (vul_file::exists(this->site_dir_)))
  {
    vgui_dialog_extensions d("Saving the Site");
    d.message(("Saving the site " + site_name_).c_str());
    d.message(("under: " + site_dir_).c_str());
    d.line_break();
    if (!d.ask()) {
      return;
    }
    site->name_ = this->site_name_;
    site->path_ = this->site_dir_;
    site->pyr_exe_path_ = this->pyr_exe_;
  }
  else
  {
    // ask the path for saving the site
    std::string site_name, site_dir, pyr_exe, ext;
    vgui_dialog_extensions d("Save the Site!");
    d.field("Site name:", site_name);
    d.line_break();
    d.dir("Site dir:", ext, site_dir);
    d.line_break();
    //d.file("Pyramid exe path:" , ext, pyr_exe);
    //d.line_break();
    d.line_break();
    if (!d.ask())
      return;

    if (!vul_file::is_directory(site_dir)) {
      std::cerr << "Please enter a directory for the site\n";
      return;
    }

    site->name_ = this->site_name_ = site_name;
    site->path_ = this->site_dir_ = site_dir;
    site->pyr_exe_path_ = this->pyr_exe_;
  }

  long time = timer_.real();
  std::stringstream strm;
  strm << std::fixed << time;
  std::string str(strm.str());
  std::string site_path = site_dir_ + "\\" + site_name_ + "_v" + str + ".xml";
  std::ofstream s(site_path.data());

  // get the tableaux
  for (unsigned i=0; i<active_tableaus_.size(); i++) {
    if (active_tableaus_[i]->type_name.compare("CameraTableau") == 0) {
      std::string new_cam_path = bwm_tableau_mgr::instance()->save_camera(active_tableaus_[i]->name);
      bwm_io_tab_config_cam* t  = static_cast<bwm_io_tab_config_cam*> (active_tableaus_[i]);
      t->cam_path = new_cam_path;
    }
    site->tableaus_.push_back(active_tableaus_[i]->clone());
  }

  // add the inactive tableaux
  for (unsigned i=0; i<inactive_tableaus_.size(); i++) {
    site->tableaus_.push_back(inactive_tableaus_[i]->clone());
  }

  // add the correspondences if any
  bwm_observer_mgr* obs_mgr = bwm_observer_mgr::instance();
  if (obs_mgr->corr_mode() == bwm_observer_mgr::IMAGE_TO_IMAGE)
    site->corr_mode_ = "IMAGE_TO_IMAGE";
  else {
    site->corr_mode_ = "WORLD_TO_IMAGE";
  }

  if (obs_mgr->n_corrs() == bwm_observer_mgr::MULTIPLE_CORRS)
    site->corr_type_ = "MULTIPLE";
  else
    site->corr_type_ = "SINGLE";

  std::vector<bwm_corr_sptr> c_list = bwm_observer_mgr::instance()->correspondences();
  for (unsigned i=0; i<c_list.size(); i++) {
    site->corresp_.push_back(c_list[i]->match_list());
    site->corresp_world_pts_.push_back(c_list[i]->world_pt());
  }

  // add the objects
  // ask one camera tableau to save its objects
  std::vector<bwm_observable_sptr> objs = bwm_world::instance()->objects();
  std::string obj_path = site_dir_ + "\\" + site_name_ + "_objects\\";
  vul_file::make_directory(obj_path);
  for (unsigned i=0; i<objs.size(); i++) {
    if (objs[i]) {
      std::stringstream strm;
      strm << std::fixed << i;
      std::string str(strm.str());
      std::string fname = obj_path + "mesh_" + str + ".ply";
      objs[i]->save(fname.c_str());
      site->objects_.push_back(std::pair<std::string, std::string>(fname, object_types_[MESH_FEATURE]));
    }
  }

  site->x_write(s);
}

void bwm_site_mgr::load_video_site()
{
  std::string site_path = bwm_utils::select_file();
  if (!site_path.size())
  {
    std::cerr << "In bwm_site_mgr::load_video_site() - no site path specified\n";
    return;
  }

  bwm_video_site_io cio;
  if (!cio.open(site_path))
  {
    std::cerr << "In bwm_site_mgr::load_video_site() - load failed in XML parse\n";
    return;
  }
  site_name_ = cio.name();
  std::string frame_glob = cio.video_path();
  std::string camera_glob = cio.camera_path();
  bwm_io_tab_config_video* v = new bwm_io_tab_config_video(site_name_, true, frame_glob, camera_glob);
  active_tableaus_.push_back(v);
  bwm_tableau_img* t = tableau_factory_.create_tableau(v);
  if (!t)
    return;
  bwm_tableau_video* vt = static_cast<bwm_tableau_video*> (t);
  vt->set_corrs(cio.corrs());
  bwm_tableau_mgr::instance()->add_tableau(t, site_name_);

  site_dir_ = cio.site_directory();
  video_path_ = cio.video_path();
  camera_path_ = cio.camera_path();
  std::vector<std::string> obj_types = cio.object_types();
  std::vector<std::string> obj_paths = cio.object_paths();
  unsigned nobj = obj_types.size();
  if (!nobj) return;
  bwm_tableau_mgr::instance()->set_draw_mode_face();
  for (unsigned i = 0; i<nobj; ++i) {
    if (obj_types[i]!="mesh_feature") continue;
    bwm_observable_mesh_sptr mesh = new bwm_observable_mesh();
    mesh->load_from(obj_paths[i]);
    if (mesh) {
      bwm_observable_mesh_sptr obj = new bwm_observable_mesh();
      bwm_observer_mgr::instance()->attach(obj);
      obj->set_path(obj_paths[i]);
      obj->set_mesh_type(bwm_observable_mesh::BWM_MESH_FEATURE);
      obj->set_object(mesh->get_object()->clone());
      bwm_world::instance()->add(obj);
    }
  }
}

void bwm_site_mgr::save_video_site()
{
  //for now - only support one video observer

  //also for now if the site was initialized with just a directory containing
  //mesh objects the saved site file will have a null directory path
  //but with the individual object paths included inside the objects scope.
  //the effect is the same it is just that the file is somewhat more verbose
  // it is not clear where an object directory path should go bwm_world?

  bool found = false;
  std::vector<bwm_observer_cam*> obsvs =
    bwm_observer_mgr::instance()->observers_cam();
  bwm_observer_video* obv = nullptr;

  for (std::vector<bwm_observer_cam*>::iterator oit = obsvs.begin();
       oit != obsvs.end()&&!found; ++oit)
    if ((*oit)->type_name()=="bwm_observer_video")
    {
      obv = static_cast<bwm_observer_video*>(*oit);
      found =true;
    }

  if (!found)
  {
    std::cerr << "In bwm_site_mgr::save_video_site() - no observer of type video\n";
    return;
  }

  bwm_video_site_io vio;

  if ((this->site_name_.size() > 0) &&
      (this->site_dir_.size() > 0) &&
      (vul_file::exists(this->site_dir_)))
  {
    vgui_dialog d("Saving the Video Site");
    d.message(("Saving the site " + site_name_).c_str());
    d.message(("under: " + site_dir_).c_str());
    d.line_break();
    if (!d.ask()) {
      return;
    }
    vio.set_name(this->site_name_);
    vio.set_site_directory(this->site_dir_);
    vio.set_video_path(this->video_path_);
    vio.set_camera_path(this->camera_path_);
  }
  else
  {
    // ask the path for saving the site
    std::string ext = "png";
    vgui_dialog_extensions d("Save the Video Site!"); //Still will NOT work in Linux!
    d.field("Video Site Name:", this->site_name_);
    d.line_break();
    d.dir("Video Site Dir:", ext, this->site_dir_);
    d.line_break();
    d.line_break();
    if (!d.ask())
      return;
    if (!vul_file::is_directory(this->site_dir_)) {
      std::cerr << "Please enter a directory for the video site\n";
      return;
    }

    vio.set_name(this->site_name_);
    vio.set_site_directory(this->site_dir_);
    vio.set_video_path(obv->image_path());
    vio.set_camera_path(obv->camera_path());
  }

  std::vector<bwm_observable_sptr> objs = bwm_world::instance()->objects();
  std::vector<std::string> obj_types;
  std::vector<std::string> obj_paths;
  std::string obj_dir = site_dir_ + "/" + site_name_ + "_objects/";
  vul_file::make_directory(obj_dir);
  unsigned iobj = 0;
  for (std::vector<bwm_observable_sptr>::iterator oit = objs.begin();
       oit != objs.end(); ++oit, ++iobj)
    if ((*oit)->type_name()=="bwm_observable_mesh") {
      std::stringstream strm;
      strm << std::fixed << iobj;
      std::string str(strm.str());
      std::string path = obj_dir + "mesh_" + str + ".ply";
      obj_types.push_back("mesh_feature");
      obj_paths.push_back(path);
      (*oit)->save(path.c_str());
    }
    else
      std::cout << "Can't save object of type " << (*oit)->type_name() << '\n';
  vio.set_object_types(obj_types);
  vio.set_object_paths(obj_paths);

  std::vector<bwm_video_corr_sptr> corrs = obv->corrs();
  vio.set_corrs(corrs);
  long time = timer_.real();
  std::stringstream strm;
  strm << std::fixed << time;
  std::string ver(strm.str());
  std::string site_path = site_dir_ + "/" + site_name_ + "_v" + ver + ".xml";
  vio.x_write(site_path);
}

void bwm_site_mgr::load_img_tableau()
{
  vgui_dialog/*_extensions*/ params ("Image Tableau");
  std::string ext, name, img_file, empty="";
  params.field("Tableau Name", name);
  // params.line_break();
  params.file("Image...", ext, img_file);
  //params.line_break();
  params.set_modal(true);
  if (!params.ask())
    return;

  if (img_file == "") {
    bwm_utils::show_error("Please specify an image file (prefix).");
    return;
  }

  bwm_io_tab_config_img img(name, true, img_file);
  bwm_tableau_img* tab = tableau_factory_.create_tableau(&img);
  bwm_tableau_mgr::instance()->add_tableau(tab, name);
  active_tableaus_.push_back(img.clone());
}

void bwm_site_mgr::load_video_tableau()
{
  vgui_dialog_extensions params ("Video Tableau");
  static std::string video_path = "";
  static std::string camera_glob = "";
  std::string name = "none";
  std::string ext = "";
  params.field("Tableau Name", name);
  params.line_break();
  params.dir("Frame Glob or File", ext, video_path);
  params.line_break();
  params.dir("Camera Glob", ext, camera_glob);
  params.line_break();
  params.set_modal(true);
  if (!params.ask())
    return;

  if (video_path == "") {
    bwm_utils::show_error("Please specify a video path");
    return;
  }

  bwm_io_tab_config_video* video = new bwm_io_tab_config_video(name, true, video_path, camera_glob);
  active_tableaus_.push_back(video);
  bwm_tableau_img*  t = tableau_factory_.create_tableau(video);
  bwm_tableau_mgr::instance()->add_tableau(t, name);
}

void bwm_site_mgr::load_cam_tableau()
{
  vgui_dialog_extensions params ("Camera Tableau");
  std::string ext, name, img_file, cam_file, empty="";
  //std::string ext, name, cam_file, empty="";
  //static std::string img_file = "";
  static int camera_type = 0;
  std::vector<std::string> types;
  types.push_back("generic");
  types.push_back("rational");
  types.push_back("projective");
  types.push_back("perspective");
  types.push_back("identity");
  types.push_back("geo");
  params.field("Tableau Name", name);
  params.line_break();
  //params.dir("Image...", ext, img_file);
  params.file("Image...", ext, img_file);
  params.line_break();
  params.choice("Camera Type",types, camera_type);
  params.line_break();
  params.file("Camera...", ext, cam_file);
  params.line_break();
  if (!params.ask())
    return;

  if ((img_file == "") || (cam_file == "" && camera_type != 4)) {  // for identity camera type, cam_file can be empty ?
    vgui_dialog error ("Error");
    error.message ("Please specify an input file (prefix)." );
    error.ask();
    return;
  }

  std::string cam_str;
  switch (camera_type)
  {
   case 0:
    cam_str = "generic";
    break;
   case 1:
    cam_str = "rational";
    break;
   case 2:
    cam_str = "projective";
    break;
   case 3:
    cam_str = "perspective";
    break;
   case 4:
    cam_str = "identity";
    break;
   case 5:
    cam_str = "geo";
    break;
   default:
    cam_str = "unknown";
  }

  bwm_io_tab_config_cam* cam = new bwm_io_tab_config_cam(name, true, img_file, cam_file, cam_str);
  active_tableaus_.push_back(cam);
  bwm_tableau_img*  t = tableau_factory_.create_tableau(cam);
  bwm_tableau_mgr::instance()->add_tableau(t, name);
}

bwm_io_config_parser* bwm_site_mgr::parse_config()
{
  std::string fname = bwm_utils::select_file();

  if (fname.size() == 0)
    return nullptr;

  bwm_io_config_parser* parser = new bwm_io_config_parser();
  std::FILE* xmlFile = std::fopen(fname.c_str(), "r");
  if (!xmlFile) {
    std::cerr << fname.c_str() << " error on opening\n";
    delete parser;
    return nullptr;
  }
  if (!parser->parseFile(xmlFile)) {
    std::cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             << parser->XML_GetCurrentLineNumber() << '\n';

    delete parser;
    return nullptr;
  }
  std::cout << "finished!" << std::endl;
  return parser;
}

static void write_vrml_header(std::ofstream& str)
{
  str << "#VRML V2.0 utf8\n"
      << "Background {\n"
      << "  skyColor [ 0 0 0 ]\n"
      << "  groundColor [ 0 0 0 ]\n"
      << "}\n";
}

static void write_vrml_points(std::ofstream& str,
                              std::vector<vgl_point_3d<double> > const& pts3d, double rad=2.0)
{
  int n = pts3d.size();
  for (int i =0; i<n; i++)
    str << "Transform {\n"
        << "translation " << pts3d[i].x() << ' ' << pts3d[i].y() << ' '
        << ' ' << pts3d[i].z() << '\n'
        << "children [\n"
        << "Shape {\n"
        << " appearance Appearance{\n"
        << "   material Material\n"
        << "    {\n"
        << "      diffuseColor " << 0 << ' ' << 1.0 << ' ' << 0.0 << '\n'
        << "      transparency " << 0.0 << '\n'
        << "    }\n"
        << "  }\n"
        << " geometry Sphere\n"
        <<   "{\n"
        << "  radius " << rad << '\n'
        <<  "   }\n"
        <<  "  }\n"
        <<  " ]\n"
        << "}\n";
}

static void
write_vrml_cameras(std::ofstream& str,
                   std::vector<vpgl_perspective_camera<double> > const& cams, double rad=2.0)
{
  str << "#VRML V2.0 utf8\n"
      << "Background {\n"
      << "  skyColor [ 0 0 0 ]\n"
      << "  groundColor [ 0 0 0 ]\n"
      << "}\n";
  int n = cams.size();
  for (int i =0; i<n; i++) {
    vgl_point_3d<double> cent =  cams[i].get_camera_center();
    str << "Transform {\n"
        << "translation " << cent.x() << ' ' << cent.y() << ' '
        << ' ' << cent.z() << '\n'
        << "children [\n"
        << "Shape {\n"
        << " appearance Appearance{\n"
        << "   material Material\n"
        << "    {\n"
        << "      diffuseColor " << 1 << ' ' << 1.0 << ' ' << 0.0 << '\n'
        << "      transparency " << 0.0 << '\n'
        << "    }\n"
        << "  }\n"
        << " geometry Sphere\n"
        <<   "{\n"
        << "  radius " << rad << '\n'
        <<  "   }\n"
        <<  "  }\n"
        <<  " ]\n"
        << "}\n";
    vgl_vector_3d<double> r = cams[i].principal_axis();
    std::cout<<"principal axis :" <<r<<std::endl;
    vnl_double_3 yaxis(0.0, 1.0, 0.0), pvec(r.x(), r.y(), r.z());
    vgl_rotation_3d<double> rot(yaxis, pvec);
    vnl_quaternion<double> q = rot.as_quaternion();

    vnl_double_3 axis = q.axis();
    std::cout<<"quaternion "<<axis<< " angle "<<q.angle()<<"\n\n";
    double ang = q.angle();
    str <<  "Transform {\n"
        << " translation " << cent.x()+6*rad*r.x() << ' ' << cent.y()+6*rad*r.y()
        << ' ' << cent.z()+6*rad*r.z() << '\n'
        << " rotation " << axis[0] << ' ' << axis[1] << ' ' << axis[2] << ' ' <<  ang << '\n'
        << "children [\n"
        << " Shape {\n"
        << " appearance Appearance{\n"
        << "  material Material\n"
        << "   {\n"
        << "     diffuseColor 1 0 0\n"
        << "     transparency 0\n"
        << "    }\n"
        << "  }\n"
        << " geometry Cylinder\n"
        << "{\n"
        << " radius "<<rad/3<<'\n'
        << " height " << 12*rad << '\n'
        << " }\n"
        << " }\n"
        << "]\n"
        << "}\n";
  }
}

#ifdef DEBUG
static void
write_vrml_box(std::ofstream& str,vgl_box_3d<double> box)
{
  str << "#VRML V2.0 utf8\n"
      << "Background {\n"
      << "  skyColor [ 0 0 0 ]\n"
      << "  groundColor [ 0 0 0 ]\n"
      << "}\n"
      << "Transform {\n"
      << "translation " << box.centroid_x() << ' ' << box.centroid_y() << ' '
      << ' ' << box.centroid_z() << '\n'
      << "children [\n"
      << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << 1 << ' ' << 1.0 << ' ' << 0.0 << '\n'
      << "      transparency " << 0.0 << '\n'
    //<< "\t\tfilled FALSE\n"
      << "    }\n"
      << "  }\n"
      << " geometry Box\n"
      <<   "{\n"
      << "  size " << box.width() <<' '<<box.depth()<<' '<<box.height()<< '\n'
      <<  "   }\n"
      <<  "  }\n"
      <<  " ]\n"
      << "}\n";
}
#endif

static void save_video_world_points_vrml_impl(std::ofstream& os)
{
  bool found = false;
  std::vector<bwm_observer_cam*> obsvs =
    bwm_observer_mgr::instance()->observers_cam();
  bwm_observer_video* obv = nullptr;

  for (std::vector<bwm_observer_cam*>::iterator oit = obsvs.begin();
       oit != obsvs.end()&&!found; ++oit)
    if ((*oit)->type_name()=="bwm_observer_video")
    {
      obv = static_cast<bwm_observer_video*>(*oit);
      found =true;
    }

  if (!found)
  {
    std::cerr << "In bwm_site_mgr::save_video_world_points_vrml() - no observer of type video\n";
    return;
  }
  std::vector<vgl_point_3d<double> > pts;
  std::vector<bwm_video_corr_sptr> corrs = obv->corrs();
  for (std::vector<bwm_video_corr_sptr>::iterator cit = corrs.begin();
       cit!= corrs.end(); ++cit)
  {
    bwm_video_corr_sptr corr = *cit;
    if (!corr || !corr->world_pt_valid()) continue;
    vgl_point_3d<double> pt = corr->world_pt();
    pts.push_back(pt);
  }
  if (!pts.size())
    return;
  write_vrml_header(os);
  write_vrml_points(os, pts);
}

static void save_video_world_points_vrml_impl(std::ofstream& os, vgl_box_3d<double> & box, double res)
{
  bool found = false;
  std::vector<bwm_observer_cam*> obsvs =
    bwm_observer_mgr::instance()->observers_cam();
  bwm_observer_video* obv = nullptr;

  for (std::vector<bwm_observer_cam*>::iterator oit = obsvs.begin();
       oit != obsvs.end()&&!found; ++oit)
    if ((*oit)->type_name()=="bwm_observer_video")
    {
      obv = static_cast<bwm_observer_video*>(*oit);
      found =true;
    }

  if (!found)
  {
    std::cerr << "In bwm_site_mgr::save_video_world_points_vrml() - no observer of type video\n";
    return;
  }
  std::vector<vgl_point_3d<double> > pts;
  std::vector<bwm_video_corr_sptr> corrs = obv->corrs();
  int cnt=0;
  for (std::vector<bwm_video_corr_sptr>::iterator cit = corrs.begin();
       cit!= corrs.end(); ++cit)
  {
    if ((++cnt)%10==0)
    {
      bwm_video_corr_sptr corr = *cit;
      if (!corr || !corr->world_pt_valid()) continue;
      vgl_point_3d<double> pt = corr->world_pt();
      if (box.contains(pt))
        pts.push_back(pt);
    }
  }
  if (!pts.size())
    return;
  write_vrml_header(os);
  write_vrml_points(os, pts,res);
}

void bwm_site_mgr::save_video_world_points_vrml()
{
  std::string ext = ".wrl";
  std::string path;
  vgui_dialog vrml_dialog("Write World Points");
  vrml_dialog.file("VRML file", ext, path);
  if (!vrml_dialog.ask())
    return;
  std::ofstream os(path.c_str());
  save_video_world_points_vrml_impl(os);
}

static void save_video_cameras_vrml_impl(std::ofstream& os)
{
  bool found = false;
  std::vector<bwm_observer_cam*> obsvs =
    bwm_observer_mgr::instance()->observers_cam();
  bwm_observer_video* obv = nullptr;

  for (std::vector<bwm_observer_cam*>::iterator oit = obsvs.begin();
       oit != obsvs.end()&&!found; ++oit)
    if ((*oit)->type_name()=="bwm_observer_video")
    {
      obv = static_cast<bwm_observer_video*>(*oit);
      found =true;
    }

  if (!found)
  {
    std::cerr << "In bwm_site_mgr::save_video_cameras_vrml() - no observer of type video\n";
    return;
  }
  bwm_video_cam_istream_sptr cam_istr = obv->camera_stream();
  if (!cam_istr||!cam_istr->is_valid()||!cam_istr->is_seekable()) {
    std::cerr << "Invalid or non-seekable camera stream\n";
    return;
  }
  //to return to starting state
  unsigned cam_number = cam_istr->camera_number();
  cam_istr->seek_camera(0);
  std::vector<vpgl_perspective_camera<double> > cams;
  while (true) {
    vpgl_perspective_camera<double>* cam = cam_istr->current_camera();
    cams.push_back(*cam);
    if (!cam_istr->advance())
      break;
  }
  //restore the camera stream to initial position
  cam_istr->seek_camera(cam_number);
  if (!cams.size())
    return;
  write_vrml_cameras(os, cams);
}

static void save_video_cameras_vrml_impl(std::ofstream& os, vgl_box_3d<double> box, double res)
{
  bool found = false;
  std::vector<bwm_observer_cam*> obsvs =
    bwm_observer_mgr::instance()->observers_cam();
  bwm_observer_video* obv = nullptr;

  for (std::vector<bwm_observer_cam*>::iterator oit = obsvs.begin();
       oit != obsvs.end()&&!found; ++oit)
    if ((*oit)->type_name()=="bwm_observer_video")
    {
      obv = static_cast<bwm_observer_video*>(*oit);
      found =true;
    }

  if (!found)
  {
    std::cerr << "In bwm_site_mgr::save_video_cameras_vrml() - no observer of type video\n";
    return;
  }
  bwm_video_cam_istream_sptr cam_istr = obv->camera_stream();
  if (!cam_istr||!cam_istr->is_valid()||!cam_istr->is_seekable()) {
    std::cerr << "Invalid or non-seekable camera stream\n";
    return;
  }
  //to return to starting state
  unsigned cam_number = cam_istr->camera_number();
  cam_istr->seek_camera(0);
  std::vector<vpgl_perspective_camera<double> > cams;
  int cnt=0;
  while (true) {
    vpgl_perspective_camera<double>* cam = cam_istr->current_camera();
    std::cout<<cam->get_camera_center();
    if (cnt++>170 )
      if (box.contains(cam->get_camera_center()))
        cams.push_back(*cam);

    if (!cam_istr->advance())
      break;
  }
  //restore the camera stream to initial position
  cam_istr->seek_camera(cam_number);
  if (!cams.size())
    return;
  write_vrml_cameras(os, cams, res);
}

void bwm_site_mgr::save_video_cameras_vrml()
{
  std::string ext = ".wrl";
  std::string path;
  vgui_dialog vrml_dialog("Write World Points");
  vrml_dialog.file("VRML file", ext, path);
  if (!vrml_dialog.ask())
    return;
  std::ofstream os(path.c_str());
  write_vrml_header(os);
  save_video_cameras_vrml_impl(os);
}

void bwm_site_mgr::save_video_cams_and_world_pts_vrml()
{
  std::string ext = ".wrl";
  std::string path;
  vgui_dialog vrml_dialog("Write World Points");
  vrml_dialog.file("VRML file", ext, path);
  vrml_dialog.checkbox("Default Params for saving ", defaultparam_);
  vrml_dialog.field("X min:", xmin_);
  vrml_dialog.field("X max:", xmax_);
  vrml_dialog.field("Y min:", ymin_);
  vrml_dialog.field("Y max:", ymax_);
  vrml_dialog.field("Z min:", zmin_);
  vrml_dialog.field("Z max:", zmax_);
  vrml_dialog.field("Resolution", res_);

  if (!vrml_dialog.ask())
    return;
  vgl_box_3d<double> box(xmin_,ymin_,zmin_,xmax_,ymax_,zmax_);
  std::ofstream os(path.c_str());
  write_vrml_header(os);
  if (defaultparam_)
  {
    save_video_world_points_vrml_impl(os);
    save_video_cameras_vrml_impl(os);
  }
  else
  {
    save_video_world_points_vrml_impl(os,box,res_);
    save_video_cameras_vrml_impl(os,box,res_*5);
#ifdef DEBUG
    write_vrml_box(os, box);
#endif
  }
}

//: compute 3-d parameters, site bounding box and GSD
void bwm_site_mgr::compute_3d_world_params()
{
  std::string path = "";
  std::string ext = "*.txt";
  vgui_dialog par_dlg("World Params");
  par_dlg.file("Param File", ext, path);
  if (!par_dlg.ask())
    return;
  vgl_box_3d<double> bb;
  double gsd = 0;

  bool found = false;
  std::vector<bwm_observer_cam*> obsvs =
    bwm_observer_mgr::instance()->observers_cam();
  bwm_observer_video* obv = nullptr;

  for (std::vector<bwm_observer_cam*>::iterator oit = obsvs.begin();
       oit != obsvs.end()&&!found; ++oit)
    if ((*oit)->type_name()=="bwm_observer_video")
    {
      obv = static_cast<bwm_observer_video*>(*oit);
      found =true;
    }

  if (!found)
  {
    std::cerr << "In bwm_site_mgr::compute_world_params() - no observer of type video\n";
    return;
  }
  std::vector<bwm_video_corr_sptr> corrs = obv->corrs();
  if (!corrs.size()) {
    std::cerr << "In bwm_site_mgr::compute_world_params() - no correspondences\n";
    return;
  }
  bwm_video_cam_istream_sptr cam_istr = obv->camera_stream();
  if (!cam_istr || !cam_istr->is_valid() || !cam_istr->is_seekable()) {
    std::cerr << "In bwm_site_mgr::compute_world_params() - no correspondences\n";
    return;
  }
  //Add world points to bounding box
  std::vector<bwm_video_corr_sptr>::iterator cit = corrs.begin();
  for (; cit != corrs.end(); ++cit)
  {
    bwm_video_corr_sptr c = *cit;
    if (!c) continue;
    if (c->world_pt_valid())
      bb.add(c->world_pt());
  }
  //to restore cam stream state
  unsigned cam_number = cam_istr->camera_number();
  cam_istr->seek_camera(0);
  vpgl_perspective_camera<double>* cam = cam_istr->current_camera();
  //project the bounding box
  vgl_box_2d<double> bb_2d = bpgl_project::project_bounding_box(*cam, bb);
  //get the number of pixels on the diagonal
  double w = bb_2d.width(), h = bb_2d.height();
  double diag2 = std::sqrt(w*w + h*h);
  //get the length of the 3-d bb diagonal
  double w3 = bb.width(), h3 = bb.height(), d3 = bb.depth();
  double diag3 = std::sqrt(w3*w3 + h3*h3 + d3*d3);
  gsd = diag3/diag2;
  std::ofstream os(path.c_str());
  if (!os.is_open()) {
    std::cerr << "In bwm_site_mgr::compute_world_params() - invalid parameter output path\n";
    return;
  }
  os << "World Bounding Box\n"
     << bb << '\n'
     << "Ground Sample Distance(GSD): " << gsd << '\n'
     << "Bounding box size in GSD units\n"
     << "Xsize:" << static_cast<unsigned>(bb.width()/gsd)
     << " Ysize:" << static_cast<unsigned>(bb.height()/gsd)
     << " Zsize:" << static_cast<unsigned>(bb.depth()/gsd) << '\n';
  os.close();
  //restore camera stream state
  cam_istr->seek_camera(cam_number);
}

void bwm_site_mgr::load_depth_map_scene()
{
  std::string path = bwm_utils::select_file();
  std::string dir = vul_file::dirname(path);
  depth_map_scene scene;
  vsl_b_ifstream is(path.c_str());
  if (!is) {
    std::cout << "invalid binary stream for path " << path << std::endl;
    return;
  }
  scene.b_read(is);
  std::string name = "depth_map";
  std::string ifile = scene.image_path();
  std::string ipath;
  if (!vul_file::exists(ifile)) {
    // loaded depth_map_scene doesn't have image path, use the jpg having same name instead
    std::cerr << "\n WARNING: loaded depth_map_scene does not have image path" << std::endl;
    std::string temp = vul_file::strip_extension(vul_file::strip_directory(path.c_str())) + ".jpg";
    scene.set_image_path(temp);
    ipath = dir + '/' + temp;
  }
  else
    ipath = ifile;
  bwm_io_tab_config* tab = new bwm_io_tab_config_cam(name, true, ipath , "not_needed" , "perspective");
  active_tableaus_.push_back(tab);
  bwm_tableau_img*  t = tableau_factory_.create_tableau(tab);
  bwm_tableau_mgr::instance()->add_tableau(t, name);
  bwm_tableau_cam* tc = reinterpret_cast<bwm_tableau_cam*>(t);
  vpgl_camera<double>* cam = new vpgl_perspective_camera<double>(scene.cam());
  tc->observer()->set_camera(cam, "");
  tc->observer()->set_depth_map_scene(scene);
  tc->observer()->display_depth_map_scene();
}

void bwm_site_mgr::save_depth_map_scene()
{
  std::cerr << "bwm_site_mgr::save_depth_map_scene() not yet implemented!!!\n";
}
