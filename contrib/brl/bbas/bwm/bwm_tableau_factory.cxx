#include "bwm_tableau_factory.h"

#include "bwm_tableau_img.h"
#include "bwm_observer_img.h"

#include "bwm_tableau_proj_cam.h"
#include "bwm_observer_proj_cam.h"
#include "bwm_observer_generic_cam.h"
#include "bwm_tableau_generic_cam.h"
#include "bwm_tableau_rat_cam.h"
#include  "bwm_tableau_geo_cam.h"
#include "bwm_tableau_video.h"

#include "bwm_observer_mgr.h"

#include "algo/bwm_utils.h"

//note that it is essential for a tab_config to have a name
//or it can't be retrieved from the map index
//if the name is null then the most relevant file path is used.
bwm_tableau_img*
bwm_tableau_factory::create_tableau(bwm_io_tab_config* t)
{
  if (t->type_name.compare(IMAGE_TABLEAU_TAG) == 0) {
    bwm_io_tab_config_img* img_tab = static_cast<bwm_io_tab_config_img*> (t);
    std::string name = img_tab->name;
    std::string path = img_tab->img_path;
    if (t->name == "") t->name = path;
    bgui_image_tableau_sptr img = bgui_image_tableau_new();
    bwm_observer_img* obs = new bwm_observer_img(img, name, path, false);
    bwm_tableau_img* tab = new bwm_tableau_img(obs);
    return tab;
  }
  else if (t->type_name.compare(CAMERA_TABLEAU_TAG) == 0) {
    bwm_io_tab_config_cam* cam_tab = static_cast<bwm_io_tab_config_cam* > (t);
    if (cam_tab->cam_type.compare("projective") == 0||
        cam_tab->cam_type.compare("perspective") == 0)
    {
      bgui_image_tableau_sptr img = bgui_image_tableau_new();
      bwm_observer_proj_cam* o =
        new bwm_observer_proj_cam(img, cam_tab->name,
                                  cam_tab->img_path, cam_tab->cam_path, cam_tab->cam_type, false);
      bwm_tableau_proj_cam* tab = new bwm_tableau_proj_cam(o);
      if (t->name == "") t->name = cam_tab->cam_path;
      return tab;
    }
    else if (cam_tab->cam_type.compare("rational") == 0) {
      bgui_image_tableau_sptr img = bgui_image_tableau_new();
      bwm_observer_rat_cam* o =
        new bwm_observer_rat_cam(img, cam_tab->name,
                                 cam_tab->img_path, cam_tab->cam_path, false);
      bwm_tableau_rat_cam* tab = new bwm_tableau_rat_cam(o);
      if (t->name == "") t->name = cam_tab->cam_path;
      return tab;
    }
    else if (cam_tab->cam_type.compare("geo") == 0) {
      bgui_image_tableau_sptr img = bgui_image_tableau_new();
      bwm_observer_geo_cam* o =
        new bwm_observer_geo_cam(img, cam_tab->name,
                                 cam_tab->img_path, cam_tab->cam_path, false);
      bwm_tableau_geo_cam* tab = new bwm_tableau_geo_cam(o);
      if (t->name == "") t->name = cam_tab->cam_path;
      return tab;
    }
    else if (cam_tab->cam_type.compare("generic") == 0) {
      bgui_image_tableau_sptr img = bgui_image_tableau_new();
      bwm_observer_generic_cam* o =
        new bwm_observer_generic_cam(img, cam_tab->name,
                                     cam_tab->img_path, cam_tab->cam_path, cam_tab->cam_type, false);
      bwm_tableau_generic_cam* tab = new bwm_tableau_generic_cam(o);
      if (t->name == "") t->name = cam_tab->cam_path;
      return tab;
    }
    else if (cam_tab->cam_type.compare("identity") == 0) {
      // Yi Feb07, 2013 , hack here go create a perspective camera for camera type 'identity', since depth_map_scene has perspecitive camera
      bgui_image_tableau_sptr img = bgui_image_tableau_new();
      bwm_observer_proj_cam* o =
        new bwm_observer_proj_cam(img, cam_tab->name,
                                  cam_tab->img_path, cam_tab->cam_path, cam_tab->cam_type, false);
      bwm_tableau_proj_cam* tab = new bwm_tableau_proj_cam(o);
      if (t->name == "") t->name = cam_tab->cam_path;
      return tab;
    }
    else {
      std::cerr << "Unknown camera type " << cam_tab->cam_type << "coming from parser!\n";
      return nullptr;
    }
  }

  else if (t->type_name.compare(VIDEO_TABLEAU_TAG) == 0) {
    bwm_io_tab_config_video* tab = static_cast<bwm_io_tab_config_video* > (t);
    std::string name = tab->name;
    std::string video_path = tab->video_path;
    std::string camera_glob = tab->camera_glob;

    if (video_path == "")
      return nullptr;

    bgui_image_tableau_sptr img = bgui_image_tableau_new();
    img->set_file_name(video_path);
    bwm_observer_video* obs = new bwm_observer_video(img);
    bwm_tableau_video* t = new bwm_tableau_video(obs);
    vgui_viewer2D_tableau_sptr viewer = vgui_viewer2D_tableau_new(t);
    obs->set_tab_name(name);
    obs->set_viewer(viewer);
    obs->set_camera_path(camera_glob);
    bwm_observer_mgr::instance()->add(obs);
    bool open = obs->open_video_stream(video_path);
    if (camera_glob != ""){
      open = open && obs->open_camera_stream(camera_glob);
#if 0//don't really want to see the world points
      if (open)
        obs->toggle_world_pt_display();
#endif
    }

    if (open)
      obs->display_current_frame();

    return t;
  }

  else
    return nullptr;
}
