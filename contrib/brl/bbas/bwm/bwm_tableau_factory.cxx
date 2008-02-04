#include "bwm_tableau_factory.h"

#include "bwm_tableau_img.h"
#include "bwm_observer_img.h"

#include "bwm_tableau_proj_cam.h"
#include "bwm_observer_proj_cam.h"
#include "bwm_tableau_rat_cam.h"
#include "bwm_tableau_video.h"

#include "bwm_observer_mgr.h"

#include "algo/bwm_utils.h"

vgui_tableau_sptr
bwm_tableau_factory::create_tableau(bwm_io_tab_config* t)
{
  if (t->type_name.compare(IMAGE_TABLEAU_TAG) == 0) {
    bwm_io_tab_config_img* img_tab = static_cast<bwm_io_tab_config_img*> (t);
    vcl_string name = img_tab->name;
    vcl_string path = img_tab->img_path;
    bgui_image_tableau_sptr img = bgui_image_tableau_new();
    bwm_observer_img* obs = new bwm_observer_img(img, name, path, false);
    bwm_tableau_img* tab = new bwm_tableau_img(obs);
    return tab;
  }
  else if (t->type_name.compare(CAMERA_TABLEAU_TAG) == 0) {
    bwm_io_tab_config_cam* cam_tab = static_cast<bwm_io_tab_config_cam* > (t);
    if (cam_tab->cam_type.compare("projective") == 0) {
      bgui_image_tableau_sptr img = bgui_image_tableau_new();
      bwm_observer_proj_cam* o = new bwm_observer_proj_cam(img, cam_tab->name,
        cam_tab->img_path, cam_tab->cam_path, false);
      bwm_tableau_proj_cam* tab = new bwm_tableau_proj_cam(o);
      return tab;
    }
    else if (cam_tab->cam_type.compare("rational") == 0) {
      bgui_image_tableau_sptr img = bgui_image_tableau_new();
      bwm_observer_rat_cam* o = new bwm_observer_rat_cam(img, cam_tab->name,
        cam_tab->img_path, cam_tab->cam_path, false);
      bwm_tableau_rat_cam* tab = new bwm_tableau_rat_cam(o);
      return tab;
    }
    else {
      vcl_cerr << "Unknown camera type " << cam_tab->cam_type << "coming from parser!\n";
      return 0;
    }
  }

  else if (t->type_name.compare(VIDEO_TABLEAU_TAG) == 0) {
    bwm_io_tab_config_video* tab = static_cast<bwm_io_tab_config_video* > (t);
    vcl_string name = tab->name;
    vcl_string frame_glob = tab->frame_glob;
    vcl_string camera_glob = tab->camera_glob;

    if (frame_glob == "")
      return 0;

    bgui_image_tableau_sptr img = bgui_image_tableau_new();
    img->set_file_name(frame_glob);
    bwm_observer_video* obs = new bwm_observer_video(img);
    bwm_tableau_video* t = new bwm_tableau_video(obs);
    vgui_viewer2D_tableau_sptr viewer = vgui_viewer2D_tableau_new(t);
    obs->set_tab_name(name);
    obs->set_viewer(viewer);
    obs->set_camera_path(camera_glob);
    bwm_observer_mgr::instance()->add(obs);
    bool open = obs->open_video_stream(frame_glob);

    if (camera_glob != ""){
      open = open && obs->open_camera_stream(camera_glob);
      if (open)
        obs->toggle_world_pt_display();
    }

    if (open)
      obs->display_current_frame();

    return t;
  }
  return 0;
}
