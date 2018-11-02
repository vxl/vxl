#ifndef bwm_observer_video_h_
#define bwm_observer_video_h_
//:
// \file

#include <iostream>
#include <map>
#include "bwm_observer_cam.h"
#include "video/bwm_video_corr_sptr.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_plane_3d.h>
#include <vgui/vgui_style.h>
#include <vpgl/vpgl_camera.h>
#include <vidl/vidl_istream_sptr.h>
#include <bwm/video/bwm_video_cam_istream_sptr.h>
#include <bsta/bsta_histogram.h>

class vgui_soview2D_point;
class bwm_soview2D_cross;

class bwm_observer_video : public bwm_observer_cam
{
 public:

  bwm_observer_video(bgui_image_tableau_sptr const& img,
                     vpgl_camera<double> *camera, std::string cam_path)
  : bwm_observer_cam(img, camera, cam_path), display_corrs_(false),
    display_world_pts_(false), play_video_(false), time_interval_(0.0f),
    video_istr_(nullptr), cam_istr_(nullptr), tracked_corr_(nullptr)
    {init();}

  bwm_observer_video(bgui_image_tableau_sptr const& img)
  : bwm_observer_cam(img), display_corrs_(false),
    display_world_pts_(false), play_video_(false),
    time_interval_(0.0f), video_istr_(nullptr), cam_istr_(nullptr), tracked_corr_(nullptr)
    {init();}

  virtual ~bwm_observer_video(){this->clear_video_corrs_display();}

  bool handle(const vgui_event &e);

  virtual std::string type_name() const { return "bwm_observer_video"; }
  // these are syncronized streams
  // currently the camera stream is implemented as a set of files,
  // one for each video frame
  bool open_video_stream(std::string const& video_path);
  bool open_camera_stream(std::string const& camera_path);
  // virtual methods for projecting 3-d objects
  virtual void proj_point(vgl_point_3d<double> world_pt,
                          vgl_point_2d<double> &image_pt);

  virtual void proj_line(vsol_line_3d_sptr line_3d,
                         vsol_line_2d_sptr &line_2d);

  virtual void proj_poly(vsol_polygon_3d_sptr poly3d,
                         vsol_polygon_2d_sptr& poly2d);

  virtual bool intersect_ray_and_plane(vgl_point_2d<double> img_point,
                                       vgl_plane_3d<double> plane,
                                       vgl_point_3d<double> &world_point);
  // standard video display functions
  void display_current_frame();
  void next_frame();
  void previous_frame();
  void seek(unsigned frame_index);
  void play();
  void stop();
  void pause();

  // Save a video as image list
  bool save_as_image_list(std::string const& path);

  //correspondence edit methods
  void set_world_pt(double lat, double lon, double elev);

  //: add a match to an existing correspondence in the current frame
  bool add_match();

  //: removes a match to an existing correspondence in the current frame
  bool remove_match();

  //: updatesa match to an existing correspondence in the current frame
  bool update_match(vgl_point_2d<double> &pt);

  //: add video corr at a selected vertex
  bool add_match_at_vertex();

  //: remove the selected correspondence match
  void remove_selected_corr_match();

  //: remove the selected correspondence
  void remove_selected_corr();

  //: select corr for tracking
  void set_selected_corr_for_tracking();

  //: select corr for tracking
  void unset_selected_corr_for_tracking();

  // --- correspondence display methods ---------------------------------------

  //: display the correspondences for a given frame on the current frame
  void display_video_corrs(unsigned frame_index);

  //: display the current correspondences
  void display_current_video_corrs();

  //: clear the display
  void clear_video_corrs_display();

  //: access the current set of correspondences
  std::vector<bwm_video_corr_sptr> corrs();


  //: set the correspondences
  void set_corrs(std::vector<bwm_video_corr_sptr> const& corrs);

  //: turn on/off world point display
  void toggle_corr_display(){display_corrs_ = !display_corrs_;
  if (!display_corrs_) this->clear_video_corrs_display();}

  //: turn on/off world point display
  void toggle_world_pt_display(){display_world_pts_ = !display_world_pts_;}

  void display_selected_world_pt();

  //: extract a 3-d plane from three selected correspondences with world pts
  bool extract_world_plane(vgl_plane_3d<double>&  plane);

  //: get the video stream
  vidl_istream_sptr video_stream() {return  video_istr_;}

  //: get the camera stream
  bwm_video_cam_istream_sptr camera_stream() {return  cam_istr_;}

  //: extract neighborhoods around correspondences for the stream
  bool extract_neighborhoods(unsigned nbd_radius_x,unsigned nbd_radius_y,
                             std::vector<std::vector<vnl_matrix<float> > >& nhds);

  //: extract histograms of each frame
  bool extract_histograms(std::vector<bsta_histogram<float> >& hists);

  std::map<unsigned, std::vector<vsol_polygon_2d_sptr> > frame_polygon_map()
  { return this->frame_polygon_map_; }

  virtual unsigned create_polygon(vsol_polygon_2d_sptr);

  //: display the polygons mapped to a specific frame
  void display_polygons_frame();

  virtual void delete_polygon( vgui_soview* obj );

  virtual void delete_all();

  virtual void delete_all_frames();

  // mask operations

  //: initialize frame/change type/polygon map
  virtual void init_mask();

  //: associate a polygon with a change type
  virtual void add_poly_to_mask();

  //: remove the polygon's associated change type
  virtual void remove_poly_from_mask();

  //: save the frame/change type/polygon relation to a binary file
  virtual bool save_changes_binary();

  //: read /frame/change type/polygon relation binary file
  virtual bool load_changes_binary();

  //Internals --------------------------------------- -------------------------

 protected:
  vgui_style_sptr EDIT_STYLE;
  vgui_style_sptr CORR_STYLE;
  vgui_style_sptr MATCHED_STYLE;
  vgui_style_sptr PREV_STYLE;
  vgui_style_sptr POINT_3D_STYLE;

  void init();

  //: this function is called when a correspondence is set by observer_vgui.
  virtual void correspondence_action();

  //: add the current correspondence on bwm_observer_vgui as a video_corr
  bool add_video_corr();


  //: find the currently selected correspondence
  bool find_selected_video_corr(unsigned& frame, unsigned& corr_index,
                                bwm_soview2D_cross*& cross);
  //: find the soview for the correspondence at current frame with corr_index
  bwm_soview2D_cross* corr_soview(unsigned corr_index);

    //: display a single video correspondence
  void display_video_corr(bwm_video_corr_sptr const& corr,
                          unsigned frame_index,
                          vgui_style_sptr const& style);

  //: display the projected 3-d world point in the current frame
  void display_projected_3d_point(bwm_video_corr_sptr const& corr);

  //: is the correspondence displayed?
  bool is_displayed(bwm_video_corr_sptr const& corr, unsigned frame_index);

  //: select the correspondence from a different frame that is closest in time and position to the new correspondence
  void select_closest_match();

  //: clear the entire corr display map and remove corrs from display
  void clear_corr_display_map();

  //: clear the entire corr display map and remove corrs from display
  void clear_world_pt_display_map();

  //: display the current frame matches and those closest in time to the current frame
  void display_corr_track();

  //: display the correspondence index number as text
  void display_corr_index();

  //: display any 3-d objects
  void display_3d_objects();

  //: should the correspondences be displayed
  bool display_corrs_;
  //: should the world points be displayed? (requires cameras)
  bool display_world_pts_;
  //:the video play state - if true the video is playing
  bool play_video_;
  //:the delay before displaying a new frame - default is 0
  float time_interval_;
  //: the video input stream - currently only image list is supported
  vidl_istream_sptr video_istr_;
  //: the camera input stream - currently only camera list is supported
  bwm_video_cam_istream_sptr cam_istr_;

  //: a correspondence being tracked
  bwm_video_corr_sptr tracked_corr_;

  //: the video frame-to-frame correspondences, over all frames
  std::map<unsigned, bwm_video_corr_sptr> video_corrs_;

  //: relation between correspondence views and correspondences and frame
  std::map<unsigned, std::map<unsigned, bwm_soview2D_cross*> > corr_soview_map_;

  //: relation between 3-d point display and corr id
  std::map<unsigned, vgui_soview2D_point*> world_pt_map_;

  //: relation frames/polygons
  std::map<unsigned, std::vector<vsol_polygon_2d_sptr> > frame_polygon_map_;

  //: relation frames/change type
  std::map<unsigned, std::vector< std::string > > frame_change_map_;

  //: relation soviewID and vector position
  std::map<unsigned, unsigned> id_pos_map_;
};

#endif
