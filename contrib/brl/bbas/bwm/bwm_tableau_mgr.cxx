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
#include "algo/bwm_rat_proj_camera.h"
#include "algo/bwm_image_processor.h"
#include "algo/bwm_utils.h"

#include <vgui/vgui_poly_tableau.h>
#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_image_utils.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_range_map_params.h>
#include <vgui/vgui_dialog.h>
#include <bgui/bgui_image_utils.h>

#include <bgui3d/bgui3d.h>

#include <vil/vil_load.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vil/vil_property.h>

#include <vul/vul_file.h>
#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>
#include <Inventor/nodes/SoSelection.h>

#define XML

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
  obs->set_viewer(viewer);
  add_to_grid(viewer);
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
    camera_rat = this->extract_nitf_camera(img_res);
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
  add_to_grid(viewer);
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
    vcl_cout << tab->type_name() << vcl_endl;
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
  add_to_grid(viewer);
  tableaus_[name] = tab;
}

void bwm_tableau_mgr::load_tableaus()
{
#ifdef XML 
  bwm_io_config_parser* parser = parse_config();
  vcl_vector<bwm_io_tab_config* > tableaus =  parser->tableau_config();

  for (unsigned i=0; i<tableaus.size(); i++) {
    bwm_io_tab_config* t = tableaus[i];
    if (t->type_name.compare("ImageTableau") == 0) {
      bwm_io_tab_config_img* img_tab = static_cast<bwm_io_tab_config_img* > (t);
      vcl_string name = img_tab->name;
      vcl_string path = img_tab->img_path;
      create_img_tableau(name, path);

    } else if (t->type_name.compare("CameraTableau") == 0) {
      bwm_io_tab_config_cam* cam_tab = static_cast<bwm_io_tab_config_cam* > (t);
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

    } else if (t->type_name.compare("Coin3DTableau") == 0) {
      bwm_io_tab_config_coin3d* coin3d_tab = static_cast<bwm_io_tab_config_coin3d* > (t);
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
      X = elm[i].second.x();
      Y = elm[i].second.y();
      vgui_tableau_sptr tab = this->find_tableau(tab_name);
      if (tab) {
        vcl_cout << tab->type_name() << vcl_endl;
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

#else

  vcl_string filename = select_file();
  if (filename.empty())
    {
      vcl_cout << "Bad filename\n";
      return;
    }
  vcl_ifstream is(filename.data());
  if(!is.is_open())  {
      vcl_cout << "Can't open file\n";
      return;
    }

  vcl_string type, name, str;
  while (!is.eof()) {
    is >> type;

    // IMAGE TABLEAU
    if(type == "IMAGE_TAB:") {
      is >> name;
      is >> str;
      if(str != "IMAGE:")  {
          vcl_cout << "Bad file parse\n";
          return;
      }

      vcl_string image_path;
      is >> image_path;
      //is >> str;
      create_img_tableau(name, image_path);
    }

    // CAMERA TABLEAU
    if(type == "CAM_TAB:") {  
      is >> name;
      is >> str;
      if(str != "IMAGE:")  {
          vcl_cout << "Bad file parse\n";
          return;
      }

      vcl_string image_path;
      is >> image_path;
      is >> str;
      if(str != "CAMERA_TYPE:") {
          vcl_cout << "Bad file parse\n";
          return;
      }

      vcl_string camera_type;
      is >> camera_type;
      is >> str;
      if(str != "CAMERA_PATH:") {
          vcl_cout << "Bad file parse\n";
          return;
      }

      vcl_string camera_path;
      is >> camera_path;
      int cam_type;
      if (camera_type == "projective")
        cam_type = 0;
      if (camera_type == "rational")
        cam_type = 1;
      create_cam_tableau(name, image_path, camera_path, cam_type);

    // COIN3D Tableau
    } else if (type == "COIN3D_TAB:") {
      is >> name;
      is >> str;
      if(str != "CAMERA_TYPE:") {
          vcl_cout << "Bad file parse\n";
          return;
      }

      vcl_string camera_type;
      is >> camera_type;
      is >> str;
      if(str != "CAMERA_PATH:") {
          vcl_cout << "Bad file parse\n";
          return;
      }

      vcl_string camera_path;
      is >> camera_path;
      int cam_type;
      if (camera_type == "projective")
        cam_type = 0;
      if (camera_type == "rational")
        cam_type = 1;
      create_coin3d_tableau(name, camera_path, cam_type);

    // PROJ2D TABLEAU
    } else if (type == "PROJ2D_TAB:") {
      is >> name;
      is >> str;
      if(str != "TYPE:") {
          vcl_cout << "Bad file parse\n";
          return;
      }

      vcl_string type;
      is >> type;

      vcl_string coin3d_name = "";
      if (type == "simple") { 
          is >> str;
          if (str != "COIN3D:") {
            vcl_cout << "Bad file parse\n";
            return;
          }
          is >> coin3d_name;
      }

      is >> str;
      if(str != "IMAGE:") {
        vcl_cout << "Bad file parse\n";
        return;
      }
      vcl_string image_path;
      is >> image_path;
      is >> str;
      if(str != "CAMERA_TYPE:") {
        vcl_cout << "Bad file parse\n";
        return;
      }
      vcl_string camera_type;
      is >> camera_type;
      is >> str;
      if(str != "CAMERA_PATH:") {
        vcl_cout << "Bad file parse\n";
        return;
      }
      vcl_string camera_path;
      is >> camera_path;
      int cam_type;
      if (camera_type == "projective")
        cam_type = 0;
      if (camera_type == "rational")
        cam_type = 1;
      create_proj2d_tableau(name, type, coin3d_name, image_path, camera_path, cam_type);
    } else if (type == "LIDAR_TAB:") {
      is >> name;
      is >> str;
      if(str != "FIRST RESPONSE IMAGE:") {
        vcl_cout << "Bad file parse\n";
        return;
      } 
      vcl_string first_ret = str;
      is >> str;
      if(str != "SECOND RESPONSE IMAGE:") {
        vcl_cout << "Bad file parse\n";
        return;
      } 
      vcl_string second_ret = str;
      vcl_string image_path;
      is >> image_path;
      create_lidar_tableau(name, first_ret, second_ret);
    } else if (type == "CORRESPONDENCES:") {
      int num, c_num;
      is >> c_num;

      vcl_string mode;
      is >> str;
      if (str != "CORR_MODE:") {
        vcl_cerr << "Correspondence mode is missing" << vcl_endl;
        return;
      }
      is >> mode;
      if ((mode != "IMAGE_TO_IMAGE") && (mode != "WORLD_TO_IMAGE")) {
        vcl_cerr << "Invalid correspondence mode" << vcl_endl;
        return;
      }
        // read the correspondences
        for (int i=0; i<c_num; i++) {
          is >> str;
          if (str != "C:" ) 
            vcl_cerr << "Invalid correspondence format" << vcl_endl;
          else {
            is >> num;
            bwm_corr_sptr corr = new bwm_corr();
            if (mode == "WORLD_TO_IMAGE") {
              is >> str;

              if (str != "WORLD_POINT:") {
                vcl_cerr << "wrong corr format" << vcl_endl;
                return;
              }

              double wx, wy, wz;
              is >> wx;
              is >> wy; 
              is >> wz;
              corr->set_mode(false);
              corr->set_world_pt(vgl_point_3d<double> (wx, wy, wz));
            }
            vcl_string tab_name;
            double X, Y;
            for (int j=0; j<num; j++) {
              is >> tab_name;
              is >> X;
              is >> Y;
              vgui_tableau_sptr tab = this->find_tableau(tab_name);
              if (tab) {
                vcl_cout << tab->type_name() << vcl_endl;
                if ((tab->type_name().compare("bwm_tableau_proj_cam") == 0)
                  || (tab->type_name().compare("bwm_tableau_rat_cam") == 0)) {
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
      }else if (type == "END") 
       return;
  }
#endif
}
 
void bwm_tableau_mgr::load_img_tableau()
 {
  vgui_dialog params ("Image Tableau");
  vcl_string ext, name, img_file, empty="";

  params.field("Tableau Name", name);
  params.file ("Image...", ext, img_file);
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
  vgui_dialog params ("Camera Tableau");
  vcl_string ext, name, img_file, cam_file, empty="";
  vcl_vector<vcl_string> camera_types;
  int camera_type = 0;
  vpgl_camera<double> *camera = (vpgl_camera<double>*)0;

  camera_types.push_back("projective");
  camera_types.push_back("rational");
  params.field("Tableau Name", name);
  params.file ("Image...", ext, img_file);  
  params.choice("Camera Type", camera_types, camera_type);
  params.file ("Camera...", ext, cam_file);  
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
  vgui_dialog params ("Project2D Tableau");
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
  params.choice("Tableau Type", tableau_types, tableau_type);
  params.file ("Image...", ext, img_file);  
  params.choice("Camera Type", camera_types, camera_type);
  params.file ("Camera...", ext, cam_file);  
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
  biu.default_range_map(rmps);

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
void bwm_tableau_mgr::add_to_grid(vgui_tableau_sptr tab)
{
   if (tableaus_.size() == 0)
  {
    grid_->add_next(tab);
    return;  
   }

  if ((tableaus_.size()%2 == 0) && (grid_->rows()*grid_->cols() == tableaus_.size()))  {
    grid_->add_row();
  }
  grid_->add_next(tab);
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

vpgl_rational_camera<double> * 
bwm_tableau_mgr::extract_nitf_camera(vil_image_resource_sptr img)
{
  //vil_image_resource_sptr img = this->selected_image();
  if (!img)
  {
    vcl_cerr << "Null image in bwm_tableau_mgr::extract_nitf_camera\n";
    return 0;
  }

  vil_nitf2_image* nitf = 0;
  vcl_string format = img->file_format();
  vcl_string prefix = format.substr(0,4);
  if (prefix == "nitf") {
    nitf = (vil_nitf2_image*)img.ptr();
    vgui_dialog file_dialog("Save NITF Camera");
    static vcl_string image_file;
    static vcl_string ext = "rpc";
    file_dialog.file("Image Filename:", ext, image_file);
    if (!file_dialog.ask())
      return 0;
    vpgl_nitf_rational_camera* rpcam = new vpgl_nitf_rational_camera(nitf, true);
    return rpcam;
    //rpcam.save(image_file);
  } else {
    vcl_cout << "The image is not an NITF" << vcl_endl;
    return 0;
  }
  
}