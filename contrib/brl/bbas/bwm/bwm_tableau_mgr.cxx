#include "bwm_tableau_mgr.h"
//:
// \file
#include "bwm_tableau_img.h"
#include "bwm_tableau_cam.h"
#include "bwm_tableau_proj_cam.h"
#include "bwm_tableau_rat_cam.h"
#include "bwm_observable_mesh_sptr.h"
#include "bwm_observable_mesh.h"
#include "bwm_observer_mgr.h"
#include "bwm_observer_img.h"
#include "bwm_observer_proj_cam.h"
#include "bwm_observer_rat_cam.h"
#include "bwm_corr_sptr.h"
#include "bwm_world.h"
#include "bwm_command.h"
#include "bwm_def.h"
#include "bwm_load_commands.h"
#include "bwm_site_sptr.h"
#include "bwm_site.h"
#include "algo/bwm_algo.h"
#include "algo/bwm_rat_proj_camera.h"
#include "algo/bwm_image_processor.h"
#include "algo/bwm_utils.h"

#include <vgui/vgui_poly_tableau.h>

#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_range_map_params.h>

#include <bgui3d/bgui3d.h>
#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_image_utils.h>

#include <vil/vil_load.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vil/vil_property.h>

#include <vcl_cassert.h>
#include <vul/vul_file.h>
#include <vul/vul_string.h>
#include <Inventor/nodes/SoSelection.h>

bwm_tableau_mgr* bwm_tableau_mgr::instance_ = 0;
vcl_map<vcl_string, bwm_command_sptr> bwm_tableau_mgr::tab_types_;

bwm_tableau_mgr* bwm_tableau_mgr::instance()
{
  if (!instance_) {
    instance_ = new bwm_tableau_mgr();
    bgui3d_init();
  }
  return bwm_tableau_mgr::instance_;
}

bwm_tableau_mgr::bwm_tableau_mgr()
{
  grid_ = vgui_grid_tableau_new ();
  grid_->set_frames_selectable(true);
  grid_->set_unique_selected(true);
  grid_->set_grid_size_changeable(true);
  display_image_path_ = false;
  row_added_ = false;

  site_create_process_ = new bwm_site_process();
  object_types_.resize(OBJ_UNDEF);
  object_types_[MESH_FEATURE] = BWM_MESH_FEATURE_STR;
  object_types_[MESH_IMAGE_PROCESSING] = BWM_MESH_IMAGE_PROCESSING_STR;
  object_types_[MESH_TERRAIN] = BWM_MESH_TERRAIN_STR;
  object_types_[VSOL] = BWM_OBJ_VSOL_STR;
}

bwm_tableau_mgr::~bwm_tableau_mgr()
{
  delete site_create_process_;
}

void bwm_tableau_mgr::add_tableau(vgui_tableau_sptr tab, vcl_string name)
{
  add_to_grid(tab);
  tableaus_[name] = tab;
}

void bwm_tableau_mgr::register_tableau(bwm_command_sptr tab_comm)
{
  tab_types_[tab_comm->name()] = tab_comm;
}

bwm_command_sptr bwm_tableau_mgr::load_tableau_by_type(vcl_string tableau_type)
{
  bwm_command_sptr comm = 0;
  vcl_map<vcl_string, bwm_command_sptr>::iterator iter = tab_types_.find(tableau_type);
  if (iter != tab_types_.end())
    comm = iter->second;

  return comm;
}

void bwm_tableau_mgr::create_site_dialog(vgui_dialog_extensions &site_dialog,
                               vcl_string &site_name,
                               vcl_string &site_dir,
                               vcl_string &pyr_exe_dir,
                               vcl_vector<vcl_string> &files,
                               bool* pyr_v, bool* act_v,
                               vcl_vector<vcl_string> &pyr_levels,
                               vcl_vector<vcl_string> &objs,
                               int* choices,
                               double &lat, double &lon, double &elev)
{
  vcl_string ext;
  site_dialog.field("SITE NAME:", site_name);
  site_dialog.line_break();
  site_dialog.dir("DIRECTORY:", ext, site_dir);
  site_dialog.line_break();
  site_dialog.dir("Pyramid exe path:", ext, pyr_exe_dir);
  site_dialog.line_break();
  site_dialog.line_break();
  site_dialog.set_modal(true);

  // add a bunch of images
  site_dialog.message("Please Choose the Images for this site:");
  site_dialog.line_break();
  for (unsigned i=0; i<files.size(); i++) {
    site_dialog.dir("Image Path:", ext, files[i]);
    site_dialog.checkbox("Do Pyramid?", pyr_v[i]);
    site_dialog.field("Levels:", pyr_levels[i]);
    site_dialog.checkbox("Is Active?", act_v[i]);
    site_dialog.line_break();
  }

  //site_dialog.line_break();
  //site_dialog.line_break();
  site_dialog.message("Please Choose the objects to add to the site:");
  site_dialog.line_break();

  vcl_vector<vcl_string> obj_types(4);
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
  site_dialog.set_ok_button("CREATE");
}

//: create a dialog box to create site to add images, objects, etc..
void bwm_tableau_mgr::create_site()
{
  vgui_dialog_extensions site_dialog("World Model Site Creation");

  vcl_string site_name, site_dir, pyr_exe_path;

  int num_images = 5;
  vcl_vector<vcl_string> files(num_images);
  bool pyr[5] = {false, false, false, false, false};
  bool act[5] = {true, true, true, true, true};
  vcl_vector<vcl_string> levels(num_images, "7");

  int num_objs = 3;
  vcl_vector<vcl_string> objs(num_objs);
  int choices[3] = {0, 0, 0};
  double lat=0.0, lon=0.0, elev=0.0;

  create_site_dialog(site_dialog, site_name, site_dir, pyr_exe_path, files, pyr,
    act, levels, objs, choices, lat, lon, elev);

  if (!site_dialog.ask()) {
    return;
  } else {
    // collect the paramaters
    vcl_cout << "name:" << site_name << vcl_endl
             << "dir:" << site_dir << vcl_endl;

    // make sure site name is filled
    while (site_name.size() == 0) {
      vgui_dialog error ("Error");
      error.message ("Please enter a valid SITE NAME!            " );
      error.ask();
      if (site_dialog.ask() == false)
        return;
    }

    // make sure site directory is filled and valid
     while ((site_dir.size() == 0) || !vul_file::is_directory(site_dir)) {
      vgui_dialog error ("Error");
      error.message ("Please enter a valid SITE DIRECTORY!            " );
      error.ask();
      if (site_dialog.ask() == false)
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
        if (site_dialog.ask() == false)
          return;
       } else {
         not_int = false;
       }
     }
  }

  vcl_vector<bool> pyramid;
  vcl_vector<bool> active;
  for (unsigned j=0; j<files.size(); j++) {
     pyramid.push_back(pyr[j]);
     active.push_back(act[j]);
  }

  vcl_vector<vcl_pair<vcl_string, vcl_string> > objects;
  for (unsigned obj=0; obj<objs.size(); obj++) {
    vcl_pair<vcl_string, vcl_string> pair(objs[obj], object_types_[choices[obj]]);
    objects.push_back(pair);
  }

  bwm_site_sptr site = new bwm_site(site_name, site_dir, files, pyramid, active,
                                    levels, objects, new vsol_point_3d(lat, lon, elev));

  // temporarily setting the exe path, will find a better solution later - Gamze
  site->pyr_exe_path_ = pyr_exe_path;

  site_create_process_->set_site(site);
  site_create_process_->StartBackgroundTask();
}

//: create a dialog box to create site to add images, objects, etc..
void bwm_tableau_mgr::edit_site()
{
  bwm_io_config_parser* parser = 0;
  parser = parse_config();
  if (parser == 0) {
    vcl_cerr << "Site File is not a valid XML site!" << vcl_endl;
    return;
  }

  vgui_dialog_extensions site_edit_dialog("Edit World Model Site");
  bwm_site_sptr site = parser->site();

 // vcl_string site_name, site_dir;

  // new creation vars
  int num_images = 3;
  vcl_vector<vcl_string> files(num_images);
  bool pyr[3] = {false, false, false};
  bool act[3] = {true, true, true};
  vcl_vector<vcl_string> levels(num_images, "7");

  int num_objs = 3;
  vcl_vector<vcl_string> objs(num_objs);
  int choices[3] = {0, 0, 0};
  double lat=0.0, lon=0.0, elev=0.0;

  // previosly created obj vars
  bool act_old[30];
  bool tab_remove[30];
  vcl_vector<vcl_string> cam;
  vcl_string ext;

  site_edit_dialog.dir("Pyramid Exe Path:", ext, site->pyr_exe_path_);
  site_edit_dialog.line_break();

  // first place the existing tableaux on the dialog
  vcl_vector<bwm_io_tab_config* > tableaus;
  site->tableaus(tableaus);
  cam.resize(tableaus.size());
  if (tableaus.size() > 0) {
    site_edit_dialog.message("EXISTING IMAGES:");
    site_edit_dialog.line_break();

    for (unsigned i=0; i<tableaus.size(); i++)
    {
      bwm_io_tab_config* t = tableaus[i];
      if (t->type_name.compare(IMAGE_TABLEAU_TAG) == 0)
      {
        bwm_io_tab_config_img* img_tab = static_cast<bwm_io_tab_config_img* > (t);
        bool active = img_tab->status;
        vcl_string name = img_tab->name;
        vcl_string path = img_tab->img_path;
        site_edit_dialog.message((" -- "+path).c_str());
        tab_remove[i] = false;
        site_edit_dialog.checkbox("Remove", tab_remove[i]);
        act_old[i] = active;
        site_edit_dialog.checkbox("Active", act_old[i]);
#if 0
        vcl_string ext = "*.RPG";
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
    vcl_string ext;
    site_edit_dialog.dir("Image Path:", ext, files[i]);
    site_edit_dialog.checkbox("Do Pyramid?", pyr[i]);
    site_edit_dialog.field("Levels:", levels[i]);
    site_edit_dialog.checkbox("Is Active?", act[i]);
    site_edit_dialog.line_break();
  }

  site_edit_dialog.line_break();

  // put the existing objects
  vcl_vector<vcl_pair<vcl_string, vcl_string> > object_paths;
  bool obj_remove[30];
  parser->site()->objects(object_paths);

  if (object_paths.size() > 0) {
    site_edit_dialog.message("EXISTING OBJECTS:");
    site_edit_dialog.line_break();

    for (unsigned i=0; i<object_paths.size(); i++) {
      site_edit_dialog.message((" -- " + object_paths[i].first).c_str());
      vcl_string object_type = object_paths[i].second;
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
  site_edit_dialog.set_ok_button("Edit");
  if (!site_edit_dialog.ask()) {
    return;
  } else {
    // check if the level values are integer

    // create a removal list for the tableaux that are marked for removal
    vcl_vector<unsigned> removal;
    for (unsigned t=0; t<tableaus.size(); t++) {
      tableaus[t]->status = act_old[t];
      if (tab_remove[t])
        removal.push_back(t);
    }
    site->remove_ = removal;

    // delete objects that are marked for removal
    vcl_vector<vcl_pair<vcl_string, vcl_string> > undeleted_objs;
    bool deleted = false;
    for (unsigned i=0; i<object_paths.size(); i++) {
      if (!obj_remove[i]) {
        undeleted_objs.push_back(object_paths[i]);
      } else
        deleted = true;
    }
    if (deleted) {
      site->objects_.clear();
      site->objects_ = undeleted_objs;
    }

    // create an updated site
    vcl_vector<bool> pyramid;
    vcl_vector<bool> active;
    for (unsigned j=0; j<files.size(); j++) {
       pyramid.push_back(pyr[j]);
       active.push_back(act[j]);
    }

    vcl_vector<vcl_pair<vcl_string, vcl_string> > objects;
    for (unsigned obj=0; obj<objs.size(); obj++) {
      vcl_pair<vcl_string, vcl_string> pair(objs[obj], object_types_[choices[obj]]);
      objects.push_back(pair);
    }
    
  /*vcl_vector<vcl_vector<vcl_pair<vcl_string, vsol_point_2d> > > corr = parser->correspondences();
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
    // vcl_vector<vsol_point_3d> corresp_world_pts() {return corresp_world_pts_; }*/

    
    site->add(files, pyramid, active, levels, objects, new vsol_point_3d(lat, lon, elev));
    site_create_process_->set_site(site);
    site_create_process_->StartBackgroundTask();
  }
}


void bwm_tableau_mgr::load_site()
{
  bwm_io_config_parser* parser = parse_config();

  if (parser)
  {
    bwm_site_sptr site = parser->site();
    vcl_vector<bwm_io_tab_config* > tableaus;
    site->tableaus(tableaus);

    site_name_ = site->name_;
    site_dir_ = site->path_;
    pyr_exe_ = site->pyr_exe_path_;

    // get the lvcs
    vsol_point_3d_sptr lvcs = site->lvcs_;
    // if LVCS is not set, do not use it
    if (!lvcs||*lvcs == vsol_point_3d(0, 0, 0))
      lvcs = 0;
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
      if (t->type_name.compare("ImageTableau") == 0) {
        bwm_io_tab_config_img* img_tab = static_cast<bwm_io_tab_config_img* > (t);
        bool active = img_tab->status;
        if (active) {
          vcl_string name = img_tab->name;
          vcl_string path = img_tab->img_path;
          create_img_tableau(name, path);
        } else
          inactive_tableaus_.push_back(img_tab);
      }
      else if (t->type_name.compare("CameraTableau") == 0) {
        bwm_io_tab_config_cam* cam_tab = static_cast<bwm_io_tab_config_cam* > (t);
        bool active = cam_tab->status;
        if (active) {
          BWM_CAMERA_TYPES cam_type;
          if (cam_tab->cam_type.compare("projective") == 0)
            cam_type = PROJECTIVE;
          else if (cam_tab->cam_type.compare("rational") == 0)
            cam_type = RATIONAL;
          else {
            vcl_cerr << "Unknown camera type " << cam_tab->cam_type << "coming from parser!\n";
            continue;
          }
          this->create_cam_tableau(cam_tab->name, cam_tab->img_path, cam_tab->cam_path, cam_type, lvcs);
        } else
          inactive_tableaus_.push_back(cam_tab);
      }
    }

    // create the correspondences
    vcl_vector<vcl_vector<vcl_pair<vcl_string, vsol_point_2d> > > corresp;
    corresp = site->corresp_;
    if (corresp.size() > 0) {
      vcl_string mode = site->corr_mode_;
      vcl_string type = site->corr_type_;
#if 0
    if (mode == "WORLD_TO_IMAGE") {
      // the vector of 3D points should be of equal size to correspondence point sets
     // assert (parser->corresp_world_pts().size() == corresp.size());
    }
#endif // 0

    if (type.compare("MULTIPLE") == 0)
      bwm_observer_mgr::instance()->set_n_corrs(bwm_observer_mgr::MULTIPLE_CORRS);
    else if (type.compare("SINGLE") == 0)
      bwm_observer_mgr::instance()->set_n_corrs(bwm_observer_mgr::SINGLE_PT_CORR);
    else
      vcl_cerr << "ERROR: Undefined Correspondence type=" << type << vcl_endl;

    if (mode == "WORLD_TO_IMAGE") {
      if (corresp.size() > 0) {
        if (site->corresp_world_pts_.size() > 0)
          bwm_world::instance()->set_world_pt(site->corresp_world_pts_[0].get_p());
        else
          vcl_cerr << "There is something wrong, the more is W-to-I but there is no world point" << vcl_endl;
      }
      bwm_observer_mgr::instance()->set_corr_mode(bwm_observer_mgr::WORLD_TO_IMAGE);
    } else if (mode == "IMAGE_TO_IMAGE") {
      bwm_observer_mgr::instance()->set_corr_mode(bwm_observer_mgr::IMAGE_TO_IMAGE);
    }

    for (unsigned i=0; i<corresp.size(); i++)
    {
      bwm_corr_sptr corr = new bwm_corr();
      vcl_vector<vcl_pair<vcl_string, vsol_point_2d> > elm = corresp[i];

      if (mode == "WORLD_TO_IMAGE") {
        corr->set_mode(false);
        corr->set_world_pt(site->corresp_world_pts_[i].get_p());
        //sets the same pt each time FIXME -JLM
        //bwm_world::instance()->set_world_pt(corr->world_pt());
      } else if (mode == "IMAGE_TO_IMAGE") {
        corr->set_mode(true);
      }

      vcl_string tab_name;
      double X, Y;
      for (unsigned j=0; j<elm.size(); j++) {
        tab_name = elm[j].first;
        X = elm[j].second.x();
        Y = elm[j].second.y();
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
      }
      bwm_observer_mgr::instance()->set_corr(corr);
    }
    }
    // create the objects
    vcl_vector<vcl_pair<vcl_string, vcl_string> > objs;
    site->objects(objs);
    for (unsigned i=0; i<objs.size(); i++)
    {
      vcl_string path = objs[i].first;
      vcl_string type = objs[i].second;
      if (path.size() > 0) {
        if (!vul_file::exists(path))
          vcl_cerr << "ERROR: The object file \"" << path << "\" could not be found!" << vcl_endl;
        else {
          if (type.compare(object_types_[VSOL]) == 0) {
            // will be implemented later!!!
          } else {
            // comes here if it is a mesh
            bwm_observable_mesh_sptr mesh = new bwm_observable_mesh();
            mesh->load_from(path);
            if (mesh) {
              bwm_observable_mesh_sptr obj = new bwm_observable_mesh();
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
  }
}

//: saves the site to an XML file
void bwm_tableau_mgr::save_site()
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
    vcl_string site_name, site_dir, pyr_exe, ext;
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
      vcl_cerr << "Please enter a directory for the site" << vcl_endl;
      return;
    }

    site->name_ = this->site_name_ = site_name;
    site->path_ = this->site_dir_ = site_dir;
    site->pyr_exe_path_ = this->pyr_exe_;
  }

  long time = timer_.real();
  vcl_stringstream strm;
  strm << vcl_fixed << time;
  vcl_string str(strm.str());
  vcl_string site_path = site_dir_ + "\\" + site_name_ + "_v" + str + ".xml";
  vcl_ofstream s(site_path.data());

  // get the tableaux
  vcl_map<vcl_string, vgui_tableau_sptr>::iterator it = tableaus_.begin();
  while (it != tableaus_.end())
  {
    vgui_tableau_sptr tab = it->second;
    vcl_string name = it->first;
    if (tab->type_name().compare("bwm_tableau_image") == 0) {
      bwm_tableau_img* img_tab = static_cast<bwm_tableau_img*> (tab.as_pointer());
      vcl_string img_path = img_tab->img_path();
      bwm_io_tab_config_img *t = new bwm_io_tab_config_img(IMAGE_TABLEAU_TAG, name, "active", img_path);
      site->tableaus_.push_back(t);
    } else if (tab->type_name().compare("bwm_tableau_rat_cam") == 0) {
      bwm_tableau_cam* cam_tab = static_cast<bwm_tableau_cam*> (tab.as_pointer());
      vcl_string img_path = cam_tab->img_path();
       vcl_string cam_path = cam_tab->observer()->camera_path();
      bwm_io_tab_config_cam *t = new bwm_io_tab_config_cam(CAMERA_TABLEAU_TAG,
        name, "active", img_path, cam_path, "rational");
      site->tableaus_.push_back(t);
    } else if (tab->type_name().compare("bwm_tableau_proj_cam") == 0) {
      bwm_tableau_cam* cam_tab = static_cast<bwm_tableau_cam*> (tab.as_pointer());
      vcl_string img_path = cam_tab->img_path();
       vcl_string cam_path = cam_tab->observer()->camera_path();
      bwm_io_tab_config_cam *t = new bwm_io_tab_config_cam(CAMERA_TABLEAU_TAG,
        name, "active", img_path, cam_path, "projective");
      site->tableaus_.push_back(t);
    }
    it++;
  }
  // add the inactive tableaux
  for (unsigned i=0; i<inactive_tableaus_.size(); i++) {
    site->tableaus_.push_back(inactive_tableaus_[i]);
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

  vcl_vector<bwm_corr_sptr> c_list = bwm_observer_mgr::instance()->correspondences();
  for (unsigned i=0; i<c_list.size(); i++){
    site->corresp_.push_back(c_list[i]->match_list());
    site->corresp_world_pts_.push_back(c_list[i]->world_pt());
  }

  // add the objects
  // ask one camera tableau to save its objects
  vcl_vector<bwm_observable_sptr> objs = bwm_world::instance()->objects();
  vcl_string obj_path = site_dir_ + "\\" + site_name_ + "_objects\\";
  vul_file::make_directory(obj_path);
  for (unsigned i=0; i<objs.size(); i++) {
    if (objs[i]) {
      vcl_stringstream strm;
      strm << vcl_fixed << i;
      vcl_string str(strm.str());
      vcl_string fname = obj_path + "mesh_" + str + ".ply";
      objs[i]->save(fname.c_str());
      site->objects_.push_back(vcl_pair<vcl_string, vcl_string>(fname, object_types_[MESH_FEATURE]));
    }
  }

  site->x_write(s);
}

void bwm_tableau_mgr::create_img_tableau(vcl_string name,
                                         vcl_string& image_path)
{
  // create only if registered
  bwm_load_img_command comm;
  vcl_map<vcl_string, bwm_command_sptr>::iterator iter = tab_types_.find(comm.name());
  if (iter == tab_types_.end()) {
    vcl_cerr << "Image tableau type is not registered, not creating!\n";
    return;
  }

  bgui_image_tableau_sptr img = bgui_image_tableau_new();
  img->show_image_path(display_image_path_);

  // LOAD IMAGE
  vgui_range_map_params_sptr params;
  vil_image_resource_sptr img_res = load_image(image_path, params);
  if (!img_res) {
    show_error("Image [" + image_path + "] NOT found");
    return;
  }

  img->set_image_resource(img_res, params);
  img->set_file_name(image_path);

  bwm_observer_img* obs = new bwm_observer_img(img);
  bwm_tableau_img* t = new bwm_tableau_img(obs);
  vgui_viewer2D_tableau_sptr viewer = vgui_viewer2D_tableau_new(t);
  obs->set_tab_name(name);
  obs->set_viewer(viewer);
  unsigned row = 0, col = 0;
  add_to_grid(viewer, col, row);
  obs->set_grid_location(col, row);
  tableaus_[name] = t;
}

void bwm_tableau_mgr::create_cam_tableau(vcl_string name,
                                         vcl_string& image_path,
                                         vcl_string& cam_path,
                                         BWM_CAMERA_TYPES camera_type,
                                         vsol_point_3d_sptr lvcs)
{
  // create only if registered
  bwm_load_cam_command comm;
  vcl_map<vcl_string, bwm_command_sptr>::iterator iter = tab_types_.find(comm.name());
  if (iter == tab_types_.end()) {
    show_error("Camera tableau type "+ comm.name() +" is not registered, not creating!");;
    return;
  }

  bgui_image_tableau_sptr img = bgui_image_tableau_new();
  img->show_image_path(display_image_path_);
  // LOAD IMAGE
  vgui_range_map_params_sptr params;
  vil_image_resource_sptr img_res = load_image(image_path, params);

  if (!img_res) {
     show_error("Image [" + image_path + "] is NOT found");
     return;
  }

  img->set_image_resource(img_res, params);
  img->set_file_name(image_path);

  // LOAD CAMERA
  vpgl_proj_camera<double> *camera_proj=0;
  vpgl_rational_camera<double> *camera_rat=0;
  bwm_observer_cam* observer = (bwm_observer_cam*)0;
  bwm_observer_proj_cam* proj_observer = 0;
  bwm_observer_rat_cam* rat_observer = 0;
  bwm_tableau_cam* t = 0;

  // check if the camera path is not empty, if it is NITF, the camera
  // info is in the image, not a seperate file
  if (cam_path.size() == 0)
  {
    show_error("Camera tableaus need a valid camera path!");
    return;
  }
  else
  {
    switch (camera_type)
    {
     case PROJECTIVE:
      camera_proj = (read_projective_camera(cam_path)).clone();
      proj_observer = new bwm_observer_proj_cam(img, camera_proj, cam_path);
      observer = proj_observer;
      t = new bwm_tableau_proj_cam(proj_observer);
      break;
     case RATIONAL:
      camera_rat = read_rational_camera<double>(cam_path);
      if (!camera_rat) {
        show_error("[" + cam_path + "] is not a valid rational camera path");
        return;
      }
      rat_observer = new bwm_observer_rat_cam(img, camera_rat, cam_path);
      observer = rat_observer;
      t = new bwm_tableau_rat_cam(rat_observer);
      break;
     default:
      vcl_cout << "Error: unknown camera type "<<camera_type<< vcl_endl;
      return;
    }
  }

  // add the observer to the observer pool
  bwm_observer_mgr::instance()->add(observer);
  observer->set_tab_name(name);
  vgui_viewer2D_tableau_sptr viewer = vgui_viewer2D_tableau_new(t);

  observer->set_viewer(viewer);
  unsigned row = 0, col = 0;
  add_to_grid(viewer, col, row);
  observer->set_grid_location(col, row);
  tableaus_[name] = t;
}


void bwm_tableau_mgr::load_img_tableau()
{
  vgui_dialog_extensions params ("Image Tableau");
  vcl_string ext, name, img_file, empty="";
  params.field("Tableau Name", name);
  params.line_break();
  params.dir("Image...", ext, img_file);
  params.line_break();
  params.set_modal(true);
  if (!params.ask())
    return;

  if (img_file == "") {
    show_error("Please specify an image file (prefix).");
    return;
  }

  create_img_tableau(name, img_file);
}


void bwm_tableau_mgr::load_cam_tableau()
{
  vgui_dialog_extensions params ("Camera Tableau");
  vcl_string ext, name, img_file, cam_file, empty="";
  int camera_type = 0;
  vpgl_camera<double> *camera = (vpgl_camera<double>*)0;

  params.field("Tableau Name", name);
  params.line_break();
  params.dir("Image...", ext, img_file);
  params.line_break();
  params.choice("Camera Type", "rational", "projective", camera_type);
  params.line_break();
  params.file("Camera...", ext, cam_file);
  params.line_break();
  if (!params.ask())
    return;

  if ((img_file == "") || (cam_file == "")) {
    vgui_dialog error ("Error");
    error.message ("Please specify a input file (prefix)." );
    error.ask();
    return;
  }

  create_cam_tableau(name, img_file, cam_file, (BWM_CAMERA_TYPES) camera_type);
}

bwm_io_config_parser* bwm_tableau_mgr::parse_config()
{
  vcl_string fname = bwm_utils::select_file();

  if (fname.size() == 0)
    return 0;

  bwm_io_config_parser* parser = new bwm_io_config_parser();
  vcl_FILE* xmlFile = vcl_fopen(fname.c_str(), "r");
  if (!xmlFile){
    vcl_cerr << fname.c_str() << " error on opening\n";
    delete parser;
    return 0;
  }
  if (!parser->parseFile(xmlFile)) {
    vcl_cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             << parser->XML_GetCurrentLineNumber() << vcl_endl;

    delete parser;
    return 0;
  }
  vcl_cout << "finished!" << vcl_endl;
  return parser;
}

void bwm_tableau_mgr::remove_tableau()
{
  unsigned int col, row;
  grid_->get_last_selected_position(&col, &row);
  grid_->set_selected(row, col, false);
  vgui_tableau_sptr tab = grid_->get_tableau_at(col, row);

  vcl_map<vcl_string, vgui_tableau_sptr>::iterator iter = tableaus_.begin();
  while (iter != tableaus_.end()) {
    if ((iter->second == tab) || (tab->get_child(0) == iter->second)) {
      tableaus_.erase(iter);
      grid_->remove_at(col, row);
      return;
    }
    iter++;
  }
  grid_->layout_grid2();
}


vil_image_resource_sptr bwm_tableau_mgr::load_image(vcl_string& filename,
                                                    vgui_range_map_params_sptr& rmps)
{
  vil_image_resource_sptr res;

    // if filename is a directory, assume pyramid image
  if (vul_file::is_directory(filename))
  {
    // if filename is a directory, assume pyramid image  if (vul_file::is_directory(filename)) {
    vil_pyramid_image_resource_sptr pyr = vil_load_pyramid_resource(filename.c_str());

    if (pyr) {
      res = pyr.ptr();
    }
    else {
      vcl_cerr << "error loading image pyramid "<< filename << vcl_endl;
      return 0;
    }
  }
  else {
    res = vil_load_image_resource(filename.c_str());
  }
  bgui_image_utils biu(res);
#if 0
  biu.default_range_map(rmps);
#endif
  biu.range_map_from_hist(1.0, false, true, true,rmps);
  return res;
}

//: Calculate the range parameters for the input image
vgui_range_map_params_sptr bwm_tableau_mgr::
range_params(vil_image_resource_sptr const& image)
{
  float gamma = 1.0;
  bool invert = false;
  bool gl_map = false;
  bool cache = true;

  //Check if the image is blocked
  vil_blocked_image_resource_sptr bir = blocked_image_resource(image);
  if (bir){
    gl_map = true;
    //cache = true;
    vcl_cout << "image is blocked.\n";
  }else {
    vcl_cout << "image is not blocked.\n";
  }

  //Check if the image is a pyramid
  bool pyr = image->get_property(vil_property_pyramid, 0);
  if (pyr){
    gl_map = true;
    //cache = true;
    vcl_cout << "image is a pyramid.\n";
  }else {
    vcl_cout << "image is not a pyramid\n.";
  }
  //Get max min parameters

  double min=0, max=0;
  unsigned n_components = image->nplanes();
  vgui_range_map_params_sptr rmps;
  if (n_components == 1)
  {
    bgui_image_utils iu(image);
    iu.range(min, max);
    rmps= new vgui_range_map_params(min, max, gamma, invert,
                                    gl_map, cache);
  }
  else if (n_components == 3)
  {
    min = 0; max = 255;//for now - ultimately need to compute color histogram
    rmps = new vgui_range_map_params(min, max, min, max, min, max,
                                     gamma, gamma, gamma, invert,
                                     gl_map, cache);
  }
  return rmps;
}

vpgl_proj_camera<double>
bwm_tableau_mgr::read_projective_camera(vcl_string cam_path)
{
  vcl_ifstream cam_stream(cam_path.data());
  vpgl_proj_camera<double> cam;
  cam_stream >> cam;
  //vcl_cout << cam << vcl_endl;

  return cam;
}

//: manages creating new tableaux on the grid.
// Decides on the layout of the grid based on the number of current tableaux.
void bwm_tableau_mgr::add_to_grid(vgui_tableau_sptr tab, unsigned& col,
                                  unsigned& row)
{
   if (tableaus_.size() == 0)
  {
    grid_->add_next(tab, col, row);
    return;
   }

  // it alternatively adds rows and colums, to equally divide the grid
  if ((tableaus_.size()%2 == 0) && (grid_->rows()*grid_->cols() == tableaus_.size())) {
    if (row_added_) {
      grid_->add_column();
      row_added_ = false;
    } else {
      grid_->add_row();
      row_added_ = true;
    }
  }
  grid_->add_next(tab, col, row);
}

void bwm_tableau_mgr::add_to_grid(vgui_tableau_sptr tab)
{
  unsigned row = 0, col=0;
  this->add_to_grid(tab, col, row);
}

vgui_tableau_sptr bwm_tableau_mgr::find_tableau(vcl_string name)
{
  vcl_map<vcl_string, vgui_tableau_sptr>::iterator iter = tableaus_.find(name);
  if (iter != tableaus_.end()) {
    return iter->second;
  }
  return 0;
}

void bwm_tableau_mgr::show_error(vcl_string msg)
{
  vgui_dialog err("ERROR occured");
  err.message(msg.c_str());
  err.ask();
}
