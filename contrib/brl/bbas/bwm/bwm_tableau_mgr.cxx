#include "bwm_tableau_mgr.h"
#include "bwm_tableau_img.h"
#include "bwm_tableau_cam.h"
#include "bwm_tableau_proj_cam.h"
#include "bwm_tableau_rat_cam.h"
#include "bwm_tableau_coin3d.h"
#include "bwm_tableau_proj2d.h"
#include "bwm_tableau_lidar.h"
#include "bwm_observer_mgr.h"
#include "bwm_observer_img.h"
#include "bwm_observer_proj_cam.h"
#include "bwm_observer_rat_cam.h"
#include "bwm_observer_coin3d.h"
#include "bwm_observer_proj2d.h"
#include "bwm_observer_lidar.h"
#include "bwm_corr_sptr.h"
#include "bwm_command.h"
#include "bwm_load_commands.h"
#include "algo/bwm_algo.h"
#include "algo/bwm_rat_proj_camera.h"
#include "algo/bwm_image_processor.h"
#include "algo/bwm_utils.h"
//#include "algo/bwm_site.h"
//#include "algo/bwm_site_creation_process.h"

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

#include <vul/vul_file.h>
#include <Inventor/nodes/SoSelection.h>

bwm_tableau_mgr* bwm_tableau_mgr::instance_ = 0;
vcl_map<vcl_string, bwm_command_sptr> bwm_tableau_mgr::tab_types_;

bwm_tableau_mgr* bwm_tableau_mgr::instance() {
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
}

bwm_tableau_mgr::~bwm_tableau_mgr()
{
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
  site_dialog.line_break();
  site_dialog.set_modal(true);

  // add a bunch of images
  site_dialog.message("Please Choose the Images for this site:");
  site_dialog.line_break();
  for (unsigned i=0; i<files.size(); i++) {
    site_dialog.dir("Image Path:", ext, files[i]);
    site_dialog.checkbox("Pyramid:", pyr_v[i]);
    site_dialog.field("Levels:", pyr_levels[i]);
    site_dialog.checkbox("Active:", act_v[i]);
    site_dialog.line_break();
  }

  site_dialog.line_break();
  site_dialog.line_break();
  site_dialog.message("Please Choose the objects to add to the site:");
  site_dialog.line_break();

  for (unsigned i=0; i<objs.size(); i++) {
    site_dialog.file("Enter input object:", ext, objs[i]);
    site_dialog.choice("Type:", "mesh", "vsol", choices[i]);
    site_dialog.line_break();
  }

  site_dialog.line_break();
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

  vcl_string site_name, site_dir;

  int num_images = 5;
  vcl_vector<vcl_string> files(num_images);
  bool pyr[5] = {false, false, false, false, false};
  bool act[5] = {false, false, false, false, false};
  vcl_vector<vcl_string> levels(num_images);

  int num_objs = 3;
  vcl_vector<vcl_string> objs(num_objs);
  int choices[3] = {0, 0, 0};
  double lat=0.0, lon=0.0, elev=0.0;

  create_site_dialog(site_dialog, site_name, site_dir, files, pyr, 
    act, levels, objs, choices, lat, lon, elev);
 
  if (!site_dialog.ask()) {
    return;
  } else {
    // collect the paramaters
    vcl_cout << "name:" << site_name << vcl_endl;
    vcl_cout << "dir:" << site_dir << vcl_endl;

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
  }
      
  vcl_vector<bool> pyramid;
  vcl_vector<bool> active;
  for (unsigned j=0; j<files.size(); j++) {
     pyramid.push_back(pyr[j]);
     active.push_back(act[j]);
  }

  vcl_vector<vcl_pair<vcl_string, vcl_string> > objects;
  for (unsigned obj=0; obj=objs.size(); obj++) {
    vcl_pair<vcl_string, vcl_string> pair(objs[obj], choices[obj] == 0 ? "mesh":"vsol");
    objects.push_back(pair);
  }

//  bwm_site site(site_name, site_dir, files, pyramid, active, 
 //   levels, objects, vsol_point_3d(lat, lon, elev));

  //bwm_site_creation_process(site, NULL);
  // collect the images and check the validity
  
  
  // process the input fields
}

void bwm_tableau_mgr::create_img_tableau(vcl_string name, 
                                         vcl_string& image_path)
{
  // create only if registered
  bwm_load_img_command comm;
  vcl_map<vcl_string, bwm_command_sptr>::iterator iter = tab_types_.find(comm.name());
  if (iter == tab_types_.end()) {
    vcl_cerr << "Image tableau type is not registered, not creating!" << vcl_endl;
    return;
  }

  bgui_image_tableau_sptr img = bgui_image_tableau_new();
  img->show_image_path(display_image_path_);
  // LOAD IMAGE
  vgui_range_map_params_sptr params;
  vil_image_resource_sptr img_res = load_image(image_path, params);
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
                                         unsigned camera_type)
{
  // create only if registered
  bwm_load_cam_command comm;
  vcl_map<vcl_string, bwm_command_sptr>::iterator iter = tab_types_.find(comm.name());
  if (iter == tab_types_.end()) {
    vcl_cerr << "Camera tableau type is not registered, not creating!" << vcl_endl;
    return;
  }
  
  bgui_image_tableau_sptr img = bgui_image_tableau_new();
  img->show_image_path(display_image_path_);
  // LOAD IMAGE
  vgui_range_map_params_sptr params;
  vil_image_resource_sptr img_res = load_image(image_path, params);
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
  if (cam_path.size() == 0) {
    camera_rat = bwm_algo::extract_nitf_camera(img_res);
    if (camera_rat == 0) {
      vcl_cerr << "Camera is not given, the tableau is not created!" << vcl_endl;
      return;
    } else {
      rat_observer = new bwm_observer_rat_cam(img, camera_rat, cam_path);
      observer = rat_observer;
      t = new bwm_tableau_rat_cam(rat_observer);
    }
  } else {
    switch(camera_type) {
    case 0:
      // projective
      camera_proj = (read_projective_camera(cam_path)).clone();
      proj_observer = new bwm_observer_proj_cam(img, camera_proj, cam_path);
      observer = proj_observer;
      t = new bwm_tableau_proj_cam(proj_observer);
      break;
    case 1:
      // rational
      camera_rat = new vpgl_rational_camera<double>(cam_path);
      rat_observer = new bwm_observer_rat_cam(img, camera_rat, cam_path);
      observer = rat_observer;
      t = new bwm_tableau_rat_cam(rat_observer);
      break;
    default:
      vcl_cout << "Error: unknown camera type "<<camera_type<< vcl_endl;
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

void bwm_tableau_mgr::create_coin3d_tableau(vcl_string name, 
                                            vcl_string& cam_path, 
                                            unsigned camera_type)
{
  // create only if registered
  bwm_load_coin3d_command comm;
  vcl_map<vcl_string, bwm_command_sptr>::iterator iter = tab_types_.find(comm.name());
  if (iter == tab_types_.end()) {
    vcl_cerr << "Coin3d tableau type is not registered, not creating!" << vcl_endl;
    return;
  }

  vpgl_proj_camera<double> *camera;
  SoSelection* master_root_ = new SoSelection;
  master_root_->ref();
  bwm_observer_coin3d* observer = 0;

  switch(camera_type) {
  case 0:
    // projective 
    camera = read_projective_camera(cam_path).clone();
    observer = new bwm_observer_coin3d(camera, master_root_);
    break;
  case 1:
    // rational
    vpgl_rational_camera<double> rat_cam;
    rat_cam = vpgl_rational_camera<double>(cam_path);
    //vcl_cout << rat_cam << vcl_endl;
    bwm_rat_proj_camera rat_proj_cam;
    rat_proj_cam.set_rat_cam(rat_cam);
    rat_proj_cam.compute();

    vpgl_perspective_camera<double> pcam;
    pcam = rat_proj_cam.get_persp_cam();

    camera = vpgl_proj_camera<double> (pcam).clone();
    observer = new bwm_observer_coin3d(camera, master_root_);
    observer->set_trans(rat_proj_cam.norm_trans());
    break;
  //default:
  //  vcl_cout << "Error: unknown camera type "<<camera_type<< vcl_endl;
  }

  observer->save_home_position();
  observer->set_camera(*camera);
  bwm_observer_mgr::instance()->add(observer);
  bwm_tableau_coin3d* t = new bwm_tableau_coin3d();
  t->set_observer(observer);
  t->add_child(observer);
  add_to_grid(t);
  tableaus_[name] = t;
}

void bwm_tableau_mgr::create_proj2d_tableau(vcl_string name, 
                                            vcl_string type, 
                                            vcl_string coin3d_name, 
                                            vcl_string image_path, 
                                            vcl_string camera_path, 
                                            unsigned camera_type)
{
  // create only if registered
  bwm_load_proj2d_command comm;
  vcl_map<vcl_string, bwm_command_sptr>::iterator iter = tab_types_.find(comm.name());
  if (iter == tab_types_.end()) {
    vcl_cerr << "Proj2d tableau type is not registered, not creating!" << vcl_endl;
    return;
  }

  bwm_observer_proj2d* observer = 0;
  //bwm_observer_proj2d_simple* proj2d_observer_simple = 0;
  bwm_rat_proj_camera rat_proj_cam;

  // LOAD CAMERA
  vpgl_proj_camera<double> *camera;
  switch(camera_type) {
  case 0:
    // projective
    
    camera = read_projective_camera(camera_path).clone();
    break;
  case 1:
    vpgl_rational_camera<double> rat_cam;
    rat_cam = vpgl_rational_camera<double>(camera_path);
    vcl_cout << rat_cam << vcl_endl;
    rat_proj_cam.set_rat_cam(rat_cam);
    rat_proj_cam.compute();

    vpgl_perspective_camera<double> pcam;
    pcam = rat_proj_cam.get_persp_cam();
    //vcl_cout << pcam1 << vcl_endl;
    camera = vpgl_proj_camera<double> (pcam).clone();
    
    break;
  //default:
  //  vcl_cout << "Error: unknown camera type "<<camera_type<< vcl_endl;
  }

  // to be able to create a simple proj2d tableau, we need a coin3d tableau's
  // scene graph root
  if (type.compare("simple")==0) {
    vgui_tableau_sptr tab = this->find_tableau(coin3d_name);
    if (tab == 0) {
      vgui_dialog error ("Error");
      error.message ("Proj2D Simple Tableau needs a corresponding Coin3D tableau name!" );
      error.ask();
      return;
    } 
   
    if (tab->type_name().compare("bwm_tableau_coin3d") != 0) {
      vgui_dialog error ("Error");
      error.message ("Proj2D Simple Tableau needs a corresponding Coin3D tableau!" );
      error.ask();
      return;
    } 
    bwm_tableau_coin3d* coin3d_tab = static_cast<bwm_tableau_coin3d*> (tab.as_pointer());
    SoNode* root = coin3d_tab->root();
    root->ref();
    observer = new bwm_observer_proj2d(*camera, root);
   // observer->ref();
   
    
    // if (camera_type == 1) { //rational camera
   //   proj2d_observer_simple->set_trans(rat_proj_cam.norm_trans()); 
   // observer = proj2d_observer_simple;
  } else {
   // observer = new bwm_observer_proj2d(camera);
    //if (camera_type == 1) { //rational camera
    //  proj2d_observer->set_trans(rat_proj_cam.norm_trans()); 
    //observer = proj2d_observer;

  }

  /*if (camera_type == 1) { //rational camera
    observer->set_trans(rat_proj_cam.norm_trans()); 
  }*/

  bgui_image_tableau_sptr img = bgui_image_tableau_new();
  img->show_image_path(display_image_path_);
  // LOAD IMAGE
  vgui_range_map_params_sptr params;
  vil_image_resource_sptr img_res = load_image(image_path, params);
  img->set_image_resource(img_res, params);


  //bwm_tableau_proj2d* tab = new bwm_tableau_proj2d(observer);
  bwm_observer_mgr::instance()->add(observer);
  vgui_composite_tableau_new comp(img, observer);
  vgui_viewer2D_tableau_new viewer(comp);
  add_to_grid(viewer);
  tableaus_[name] = comp;
}

void bwm_tableau_mgr::create_lidar_tableau(vcl_string name, 
                                           vcl_string first_ret,
                                           vcl_string second_ret)
{
  // create only if registered
  bwm_load_lidar_command comm;
  vcl_map<vcl_string, bwm_command_sptr>::iterator iter = tab_types_.find(comm.name());
  if (iter == tab_types_.end()) {
    vcl_cerr << "Lidar tableau type is not registered, not creating!" << vcl_endl;
    return;
  }
  float min_val = 20;
  float max_val = 100;
  float gamma = 1.0;
  bool invert = false;
  bool use_glPixelMap = false;
  bool cache_buffer = true;

  vgui_range_map_params_sptr params = new vgui_range_map_params(min_val, max_val, gamma,
                                invert, use_glPixelMap, cache_buffer);

  vil_image_resource_sptr res1 = vil_load_image_resource(first_ret.c_str());

  // second return may not be given, then we set it to NULL
  vil_image_resource_sptr res2 = 0;
  if (second_ret.size() > 0) 
    res2 = vil_load_image_resource(second_ret.c_str());

  bgui_image_tableau_sptr img = bgui_image_tableau_new(res1, params);
  img->show_image_path(display_image_path_);
  bwm_observer_lidar* observer = new bwm_observer_lidar(img, res1, res2);
  bwm_tableau_lidar* tab = new bwm_tableau_lidar(observer);
  tab->add_child(img);
  bwm_observer_mgr::instance()->add(observer);
  bgui_picker_tableau_sptr picker = bgui_picker_tableau_new(tab);
  vgui_viewer2D_tableau_new viewer(picker);
  unsigned col = 0, row = 0;
  add_to_grid(viewer, col, row);
  observer->set_grid_location(col, row);
  tableaus_[name] = tab;
}

void bwm_tableau_mgr::load_tableaus()
{
  bwm_io_config_parser* parser = parse_config();
  if (parser) {
    vcl_vector<bwm_io_tab_config* > tableaus =  parser->tableau_config();

    for (unsigned i=0; i<tableaus.size(); i++) {
      bwm_io_tab_config* t = tableaus[i];
      if (t->type_name.compare("ImageTableau") == 0) {
        bwm_io_tab_config_img* img_tab = static_cast<bwm_io_tab_config_img* > (t);
        bool active = img_tab->status;
        if (active) {
          vcl_string name = img_tab->name;
          vcl_string path = img_tab->img_path;
          create_img_tableau(name, path);
        }

      } else if (t->type_name.compare("CameraTableau") == 0) {
        bwm_io_tab_config_cam* cam_tab = static_cast<bwm_io_tab_config_cam* > (t);
        bool active = cam_tab->status;
        if (active) {
          BWM_CAMERA_TYPES cam_type;
          if (cam_tab->cam_type.compare("projective") == 0) 
            cam_type = PROJECTIVE;
          else if (cam_tab->cam_type.compare("rational") == 0) 
            cam_type = RATIONAL;
          else {
            vcl_cerr << "Unknown camera type " << cam_tab->cam_type << "coming from parser!" << vcl_endl;
            continue;
          }
          this->create_cam_tableau(cam_tab->name, cam_tab->img_path, cam_tab->cam_path, cam_type);
        }
      } else if (t->type_name.compare("Coin3DTableau") == 0) {
        bwm_io_tab_config_coin3d* coin3d_tab = static_cast<bwm_io_tab_config_coin3d* > (t);
        bool active = coin3d_tab->status;
        if (active) {
          BWM_CAMERA_TYPES cam_type;
          if (coin3d_tab->cam_type.compare("projective") ==0) 
            cam_type = PROJECTIVE;
          else if (coin3d_tab->cam_type.compare("rational") ==0) 
            cam_type = RATIONAL;
          else {
            vcl_cerr << "Unknown camera type " << coin3d_tab->cam_type << "coming from parser!" << vcl_endl;
            continue;
          }
          this->create_coin3d_tableau(coin3d_tab->name, coin3d_tab->cam_path, cam_type);
        }
      }
    }

    vcl_vector<vcl_vector<vcl_pair<vcl_string, vsol_point_2d> > > corresp = 
      parser->correspondences();
    vcl_string mode = parser->corresp_mode();

    if (mode == "WORLD_TO_IMAGE") {
      // the vector of 3D points should be of equal size to correspondence point sets
      assert (parser->corresp_world_pts().size() == corresp.size());
    }

    for (unsigned i=0; i<corresp.size(); i++) {
      bwm_corr_sptr corr = new bwm_corr();
      vcl_vector<vcl_pair<vcl_string, vsol_point_2d> > elm = corresp[i];

      if (mode == "WORLD_TO_IMAGE") {
        corr->set_mode(false);
        corr->set_world_pt(parser->corresp_world_pts()[i].get_p());
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
        bwm_observer_mgr::instance()->set_corr(corr);
      }
    }
    delete parser;
  }
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
    vgui_dialog error ("Error");
    error.message ("Please specify an image file (prefix)." );
    error.ask();
    return;
  }

  create_img_tableau(name, img_file);
}


void bwm_tableau_mgr::load_cam_tableau()
 {
  vgui_dialog_extensions params ("Camera Tableau");
  vcl_string ext, name, img_file, cam_file, empty="";
  vcl_vector<vcl_string> camera_types;
  int camera_type = 0;
  vpgl_camera<double> *camera = (vpgl_camera<double>*)0;

  camera_types.push_back("projective");
  camera_types.push_back("rational");
  params.field("Tableau Name", name);
  params.line_break();
  params.dir("Image...", ext, img_file);  
  params.line_break();
  params.choice("Camera Type", camera_types, camera_type);
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

  create_cam_tableau(name, img_file, cam_file, camera_type);
 }
    
void bwm_tableau_mgr::load_coin3d_tableau()
{
  vgui_dialog params ("Coin3D Tableau");
  vcl_string ext, cam_file, empty="";
  vcl_vector<vcl_string> camera_types;
  int camera_type = 0;
  //vpgl_camera<double> *camera = (vpgl_camera<double>*)0;

  camera_types.push_back("projective");
  camera_types.push_back("rational");  
    
  vcl_string name;
  params.field("Tableau Name", name);
  params.choice("Camera Type", camera_types, camera_type);
  params.file ("Camera...", ext, cam_file);  
  if (!params.ask())
    return;

  if  (cam_file == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a input file (prefix)." );
    error.ask();
    return;
  }
  create_coin3d_tableau(name, cam_file, camera_type); 
}


void bwm_tableau_mgr::load_proj2d_tableau()
{
  vgui_dialog_extensions params ("Project2D Tableau");
  vcl_string ext, img_file, cam_file, empty="";
  vcl_vector<vcl_string> camera_types;
  vcl_vector<vcl_string> tableau_types;
  vcl_string name;
  int tableau_type = 0;
  int camera_type = 0;

  tableau_types.push_back("simple");
  tableau_types.push_back("self");
  
  camera_types.push_back("projective");
  camera_types.push_back("rational");  
  
  params.field("Name..", name);
  params.line_break();
  params.choice("Tableau Type", tableau_types, tableau_type);
  params.line_break();
  params.dir("Image...", ext, img_file);
  params.line_break();
  params.choice("Camera Type", camera_types, camera_type);
  params.line_break();
  params.file ("Camera...", ext, cam_file);  
  params.line_break();
  if (!params.ask())
    return;

  if (img_file == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify an image file (prefix)." );
    error.ask();
    return;
  }

  if (cam_file == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a camera file (prefix)." );
    error.ask();
    return;
  }

  // if it is a simple proj2d tableau, there should be a connection to the coin3d tableau
  vcl_string coin3d;
  if (tableau_types[tableau_type] == "simple") {
    vgui_dialog d ("Select Coin3D Tableau");
    d.field("Coin3d Name..", coin3d);
    if (!d.ask())
      return;
  }

  create_proj2d_tableau(name, tableau_types[tableau_type], coin3d, img_file, cam_file, camera_type);
}

void bwm_tableau_mgr::load_lidar_tableau()
{
  vgui_dialog params ("Lidar Tableau");
  vcl_string ext, name, first_ret, second_ret, empty="";

  params.field("Tableau Name", name);
  params.file ("First Response Image...", ext, first_ret);
  params.file ("Second Response Image...", ext, second_ret);
  if (!params.ask())
    return;

  if (first_ret == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a first response lidar file (prefix)." );
    error.ask();
    return;
  }

  create_lidar_tableau(name, first_ret, second_ret);
}

bwm_io_config_parser* bwm_tableau_mgr::parse_config()
{
  vcl_string fname = bwm_utils::select_file();

  if (fname.size() == 0)
    return 0;

  bwm_io_config_parser* parser = new bwm_io_config_parser();
  vcl_FILE* xmlFile = vcl_fopen(fname.c_str(), "r");
  if (!xmlFile){
    fprintf(stderr, " %s error on opening", fname.c_str() );
    delete parser;
    return (0);
  }
  if (!parser->parseFile(xmlFile)) {
    fprintf(stderr,
      "%s at line %d\n",
      XML_ErrorString(parser->XML_GetErrorCode()),
      parser->XML_GetCurrentLineNumber()
      );

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
  /*vgui_poly_tableau* poly = static_cast<vgui_poly_tableau* > (grid_.as_pointer());
  vgui_poly_tableau::iterator it = poly->begin();
  while (it != poly->end()) {
    vgui_poly_tableau::item t = *it;
    vgui_parent_child_link link = t.tab;
    if (tab == link.child()) {
      grid_->erase(it);
      grid_->post_redraw();
      return;
    }
    it++;
  }*/

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
  if (vul_file::is_directory(filename)) {

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
bwm_tableau_mgr::read_projective_camera(vcl_string cam_path){

  vcl_ifstream cam_stream(cam_path.data());
  vpgl_proj_camera<double> cam;
  cam_stream >> cam;
  //vcl_cout << cam << vcl_endl;
  
  return cam;
}

//: manages creating new tableaus on the grid, decides on the layout 
// of the grid based on the nymber of current tableaus
void bwm_tableau_mgr::add_to_grid(vgui_tableau_sptr tab, unsigned& col,
                                  unsigned& row)
{
   if (tableaus_.size() == 0)
  {
    grid_->add_next(tab, col, row);
    return;  
   }

  if ((tableaus_.size()%2 == 0) && (grid_->rows()*grid_->cols() == tableaus_.size()))  {
    grid_->add_row();
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

vcl_vector<vcl_string> bwm_tableau_mgr::coin3d_tableau_names()
{
  
  vcl_map<vcl_string, vgui_tableau_sptr>::iterator iter = tableaus_.begin();
  vcl_vector<vcl_string> names(0);

  if (iter != tableaus_.end()) {
    if (iter->second->type_name().compare("bwm_tableau_coin3d") == 0)
      names.push_back(iter->first);
      iter++;
  }
  return names;
}


