#ifndef bwm_observer_video_h_
#define bwm_observer_video_h_
//:
// \file

#include "bwm_observer_cam.h"
#include "video/bwm_video_corr_sptr.h"
#include <vcl_iostream.h>
#include <vcl_map.h>
#include <vgui/vgui_style.h>
#include <vpgl/vpgl_camera.h>
#include <vidl2/vidl2_istream_sptr.h>
#include <bwm/video/bwm_video_cam_istream_sptr.h>

class vgui_soview2D_point;
class bwm_soview2D_cross;

class bwm_observer_video : public bwm_observer_cam
{
 public:

  bwm_observer_video(bgui_image_tableau_sptr const& img,
                     vpgl_camera<double> *camera, vcl_string cam_path)
    : bwm_observer_cam(img, camera, cam_path), play_video_(false),
    time_interval_(0.0f), video_istr_(0), cam_istr_(0),
    display_world_pts_(false)
    {init();}

  bwm_observer_video(bgui_image_tableau_sptr const& img)
    : bwm_observer_cam(img), play_video_(false), time_interval_(0.0f),
    video_istr_(0), cam_istr_(0), display_world_pts_(false)
    {init();}

  virtual ~bwm_observer_video(){this->clear_video_corrs_display();}

  bool handle(const vgui_event &e);

  virtual vcl_string type_name() const { return "bwm_observer_video"; }
  //:these are syncronized streams
  // currently the camera stream is implemented as a set of files, 
  // one for each video frame
  bool open_video_stream(vcl_string const& video_path);
  bool open_camera_stream(vcl_string const& camera_path);
  // standard video display functions
  void display_current_frame();
  void next_frame();
  void previous_frame();
  void seek(unsigned frame_index);
  void play();
  void stop();
  void pause();
  //correspondence edit methods

  //: add a match to an existing correspondence in the current frame
  void add_match();

  //: remove the selected correspondence match
  void remove_selected_corr_match();

  //: remove the selected correspondence
  void remove_selected_corr();

  //correspondence display methods
  //: display the correspondences for a given frame on the current frame
  void display_video_corrs(unsigned frame_index);

  //: display the current correspondences
  void display_current_video_corrs();

  //: clear the display
  void clear_video_corrs_display();

  //: access the current set of correspondences
  vcl_vector<bwm_video_corr_sptr> corrs();

  //: set the correspondences
  void set_corrs(vcl_vector<bwm_video_corr_sptr> const& corrs);

  //: turn on/off world point display
  void toggle_world_pt_display(){display_world_pts_ = !display_world_pts_;}

 protected:
  //Internals
	 protected:
  vgui_style_sptr EDIT_STYLE; 
  vgui_style_sptr CORR_STYLE; 
  vgui_style_sptr MATCHED_STYLE; 
  vgui_style_sptr PREV_STYLE;
  vgui_style_sptr POINT_3D_STYLE;
  //:initialization
  void init()
    {
      EDIT_STYLE =  vgui_style::new_style(1.0f, 1.0f, 0.0f, 1.0f, 1.0f);
      CORR_STYLE =  vgui_style::new_style(0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
      MATCHED_STYLE =  vgui_style::new_style(1.0f, 0.0f, 1.0f, 1.0f, 1.0f);
      PREV_STYLE =  vgui_style::new_style(0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
      POINT_3D_STYLE = vgui_style::new_style(0.75f, 1.0f, 0.25f, 5.0f, 1.0f);
    }
  //: this function is called when a correspondence is set by observer_vgui.
  virtual void correspondence_action();


  //: add the current correspondence on bwm_observer_vgui as a video_corr
  void add_video_corr();

  //: find the currently selected correspondence
  bool find_selected_video_corr(unsigned& frame, unsigned& corr_index,
                                bwm_soview2D_cross*& cross);
  //: display a single video correspondence
  void display_video_corr(bwm_video_corr_sptr const& corr, 
                          unsigned frame_index,
                          vgui_style_sptr const& style);

  //: display the projected 3-d world point in the current frame
  void display_projected_3d_point(bwm_video_corr_sptr const& corr);

  //: is the correspondence displayed?
  bool is_displayed(bwm_video_corr_sptr const& corr, unsigned frame_index);

  //: select the correspondence from a different frame that is 
  // closest in time and position to the new correspondence
  void select_closest_match();

  //: clear the entire display map
  void clear_display_map();

  //: display the current frame matches and those closest in time to the current frame
  void display_corr_track();

  //: should the world points be displayed? (requires cameras)
  bool display_world_pts_;
  //:the video play state - if true the video is playing
  bool play_video_;
  //:the delay before displaying a new frame - default is 0
  float time_interval_;
  //: the video input stream - currently only image list is supported
  vidl2_istream_sptr video_istr_;
  //: the camera input stream - currently only camera list is supported
  bwm_video_cam_istream_sptr cam_istr_;

  //: the video frame-to-frame correspondences, over all frames
  vcl_map<unsigned, bwm_video_corr_sptr> video_corrs_;
  
  //: relation between correspondence views and correspondences and frame
  vcl_map<unsigned, vcl_map<unsigned, bwm_soview2D_cross*> > corr_soview_map_;

  //: relation between 3-d point display and corr id
  vcl_map<unsigned, vgui_soview2D_point*> world_pt_map_;
};

#endif
