#include <iostream>
#include <sstream>
#include "bwm_observer_video.h"
//:
// \file
#include <bwm/algo/bwm_algo.h>
#include <bwm/algo/bwm_utils.h>
#include "bwm_observer_mgr.h"
#include "video/bwm_video_corr.h"
#include "algo/bwm_soview2D_cross.h"
#include <bsta/bsta_histogram.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vul/vul_timer.h>
#include <vul/vul_file.h>
#include <vnl/vnl_numeric_traits.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vidl/vidl_config.h>
#include <vidl/vidl_image_list_istream.h>
#include <vidl/vidl_image_list_ostream.h>
#if VIDL_HAS_DSHOW
#include <vidl/vidl_dshow_file_istream.h>
#endif
#include <bwm/video/bwm_video_cam_istream.h>
#include <bwm/bwm_world.h>
#include <vidl/vidl_frame.h>
#include <vidl/vidl_convert.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

bool bwm_observer_video::handle(const vgui_event &e)
{
  if (!video_istr_) return bwm_observer_cam::handle(e);
  if (e.type==vgui_KEY_PRESS && e.key == vgui_CURSOR_LEFT)
  {
    unsigned cur_frame =  video_istr_->frame_number();
    if (e.modifier == vgui_SHIFT) {
      if (cur_frame<5)
        this->seek(0);
      else
        this->seek(cur_frame-5);
    }
    else
      if (cur_frame > 0)
        this->seek(cur_frame-1);
      else seek(0);
    return true;
  }
  if (e.type==vgui_KEY_PRESS && e.key == vgui_CURSOR_RIGHT)
  {
    unsigned cur_frame =  video_istr_->frame_number();
    if (e.modifier == vgui_SHIFT)
      this->seek(cur_frame+5);
    else
      this->seek(cur_frame+1);
    return true;
  }
  if (e.type==vgui_KEY_PRESS && e.key == 'a')
  {
    this->add_match();
    return true;
  }

  if (e.type==vgui_KEY_PRESS && e.key == 'r')
  {
    return this->remove_match();
  }

  if (e.type == vgui_BUTTON_DOWN &&
      e.button == vgui_MIDDLE &&
      e.modifier == vgui_SHIFT &&
      bwm_observer_mgr::instance()->in_corr_picking())
  {
    vgui_projection_inspector pi;
    float x,y;
    pi.window_to_image_coordinates(e.wx, e.wy, x, y);
    vgl_point_2d<double> pt(x,y);
    return this->update_match(pt);
  }

  return bwm_observer_cam::handle(e);
}

//: initialization
void bwm_observer_video::init()
{
  EDIT_STYLE =  vgui_style::new_style(1.0f, 1.0f, 0.0f, 1.0f, 1.0f);
  CORR_STYLE =  vgui_style::new_style(0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
  MATCHED_STYLE =  vgui_style::new_style(1.0f, 0.0f, 1.0f, 1.0f, 1.0f);
  PREV_STYLE =  vgui_style::new_style(0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
  POINT_3D_STYLE = vgui_style::new_style(0.75f, 1.0f, 0.25f, 5.0f, 1.0f);
}


bool bwm_observer_video::open_video_stream(std::string const& video_path)
{
  std::string vpath = video_path;
  if (video_path=="")
    return false;
  bool glob = false;
  if (vul_file::is_directory(video_path)) glob = true;
  else
    for (std::string::const_iterator cit = video_path.begin();
         cit != video_path.end()&&!glob; ++cit) {
      char x = *cit;
      if (x=='*')
        glob = true;
    }
  //try to open the path as an image
  //if successful, then open the directory as an image list
  if (!glob) {
    vil_image_resource_sptr resc = vil_load_image_resource(video_path.c_str());
    if (resc) {
      vpath = vul_file::dirname(video_path);
      std::string ex = vul_file::extension(video_path);
      vpath = vpath + "/*" + ex;
      glob = true;
    }
  }
  // assume we are opening an image_list codec
  if (glob)
    video_istr_ = new vidl_image_list_istream(vpath);
#if VIDL_HAS_DSHOW
  else  video_istr_ = new vidl_dshow_file_istream(video_path);
#endif
  if (!video_istr_) return false;
  bool open = video_istr_->is_open();
  if (open&&video_istr_->is_seekable())
    this->seek(0);
  this->range_map();
  return open;
}

bool bwm_observer_video::open_camera_stream(std::string const& camera_glob)
{
  if (camera_glob=="")
    return false;
  cam_istr_ = new bwm_video_cam_istream(camera_glob);
  bool open = cam_istr_->is_open();
  if (open)
    cam_istr_->seek_camera(0);
  return open;
}

void bwm_observer_video::display_corr_index()
{
  vgui_text_tableau_sptr tt = img_tab_->text_tab();
  if (!tt) return;
  if (!video_istr_) return;
  tt->clear();
  unsigned cur_frame = video_istr_->frame_number();
  std::map<unsigned, bwm_video_corr_sptr>::iterator cit = video_corrs_.begin();
  for (; cit != video_corrs_.end(); ++cit)
  {
    bwm_video_corr_sptr c = (*cit).second;
    vgl_point_2d<double> pt;
    if (!c->match(cur_frame, pt)) continue;
    unsigned index = (*cit).first;
    std::stringstream ind_str;
    ind_str << index;
    float x = static_cast<float>(pt.x()+2.0);
    float y = static_cast<float>(pt.y()-2.0);
    tt->add(x, y, ind_str.str());
  }
  tt->post_redraw();
}

void bwm_observer_video::display_current_frame()
{
  if (video_istr_)
  {
    unsigned int frame_num = video_istr_->frame_number();
    if (bwm_observer_mgr::instance()->in_corr_picking()) {
      img_tab_->lock_linenum(true);
      bwm_observer_img::lock_vgui_status(true);
      display_corrs_ = false;
    }
    else {
      if (!bwm_observer_img::vgui_status_on())
        img_tab_->lock_linenum(false);
      bwm_observer_img::lock_vgui_status(false);
    }
    if (frame_num == unsigned(-1))
      vgui::out << "invalid frame\n";
    else
      vgui::out << "frame["<< frame_num <<"]\n";

    vidl_frame_sptr frame = video_istr_->current_frame();
    if (!frame)
      img_tab_->set_image_resource(nullptr);
#if 0  // Now not needed since vidl handles these types
    else if (frame->pixel_format() == VIDL_PIXEL_FORMAT_MONO_16) {
      static vil_image_view<vxl_uint_16> img;
      if (vidl_convert_to_view(*frame,img))
        img_tab_->set_image_view(img);
      else
        img_tab_->set_image_resource(NULL);
    }
    else if (frame->pixel_format() == VIDL_PIXEL_FORMAT_COLOR_RGB) {
      static vil_image_view<vxl_byte> img;
      if (vidl_convert_to_view(*frame,img,VIDL_PIXEL_COLOR_RGB))
        img_tab_->set_image_view(img);
      else
        img_tab_->set_image_resource(NULL);
    }
#endif
    else {
      vil_image_view_base_sptr vb = vidl_convert_wrap_in_view(*frame);
      if (vb) {
        img_tab_->set_image_view(*vb);
        vidl_image_list_istream* ilist_str = dynamic_cast<vidl_image_list_istream*>(video_istr_.ptr());
        if (ilist_str) {
          std::string cur_path = ilist_str->current_path();
          img_tab_->set_file_name(cur_path);
        }
      }
    }
  }
  if (bwm_observer_mgr::instance()->in_corr_picking())
  {
    this->clear_video_corrs_display();
    this->display_corr_track();
  }
  else if (display_corrs_) {
    this->clear_video_corrs_display();
    this->display_current_video_corrs();
  }
  else {//haven't explicitly asked to display correspondences and are
        // not explicitly picking new ones.
    this->clear_video_corrs_display();
  }
  this->display_corr_index();
  this->display_3d_objects();
  this->display_polygons_frame();
  img_tab_->post_redraw();
  vgui::run_till_idle();
}

void bwm_observer_video::next_frame()
{
  if (video_istr_->advance()) {
    if (cam_istr_)
      cam_istr_->advance();
    this->display_current_frame();
  }
}

void bwm_observer_video::previous_frame()
{
  if (!video_istr_) return;
  int prev_frame = video_istr_->frame_number() - 1;
  if (prev_frame >= 0)
    video_istr_->seek_frame(prev_frame);
  else return;

  if (cam_istr_) {
    int prev_cam_frame = cam_istr_->camera_number() -1;
    cam_istr_->seek_camera(prev_cam_frame);
    if (prev_cam_frame>=0)
      cam_istr_->seek_camera(prev_cam_frame);
    else return;
  }
  this->display_current_frame();
}

void bwm_observer_video::seek(unsigned index)
{
  if (!video_istr_) return;
  video_istr_->seek_frame(index);
  if (cam_istr_)
    cam_istr_->seek_camera(index);
  this->display_current_frame();
}

void bwm_observer_video::play()
{
  if (play_video_)
    return;
  if (!video_istr_) {
    std::cout << "No movie has been loaded\n";
    return;
  }

  play_video_ = true;
  vul_timer t,t2;
  int count = 0;

  while (play_video_ && video_istr_->is_valid() && video_istr_->advance())
  {
    if (cam_istr_ && cam_istr_->is_valid())
      cam_istr_->advance();
    this->display_current_frame();
    // Delay until the time interval has passed
    while (t.all()<time_interval_) ;
    t.mark();
    ++count;
  }
  long time = t2.all();
  std::cout << "average play time " << double(time)/count << std::endl;

  // if played to the end, go back to the first frame;
  if (play_video_)
    this->stop();
}

// Stop the video and return to the first frame
void bwm_observer_video::stop()
{
  play_video_ = false;
  if (video_istr_ && video_istr_->is_seekable()) {
    video_istr_->seek_frame(0);
    if (cam_istr_)
      cam_istr_->seek_camera(0);
    this->display_current_frame();
  }
}

// Stop the video without returning to the first frame
void bwm_observer_video::pause()
{
  play_video_ = false;
}

bool bwm_observer_video::find_selected_video_corr(unsigned& frame,
                                                  unsigned& corr_index,
                                                  bwm_soview2D_cross*& cross)
{
  vgui_soview2D* vgso = this->get_selected_object("bwm_soview2D_cross");
  cross = static_cast<bwm_soview2D_cross*>(vgso);
  if (!cross)
    return false;
  std::map<unsigned, std::map<unsigned, bwm_soview2D_cross*> >::iterator fit =
    corr_soview_map_.begin();
  bool found = false;
  for (; fit != corr_soview_map_.end()&&!found; ++fit)
    for (std::map<unsigned, bwm_soview2D_cross*>::iterator mit = (*fit).second.begin();
         mit != (*fit).second.end()&&!found; ++mit)
      if ((*mit).second == cross)
      {
        found = true;
        frame = (*fit).first;
        corr_index = (*mit).first;
      }

  return found;
}

//: select selected corr for tracking
void bwm_observer_video::set_selected_corr_for_tracking()
{
  unsigned frame = 0, corr_index = 0;
  bwm_soview2D_cross* cross =nullptr;
  if (!this->find_selected_video_corr(frame, corr_index, cross))
    return;
  tracked_corr_ = video_corrs_[corr_index];
}


//: unset selected corr for tracking
void bwm_observer_video::unset_selected_corr_for_tracking()
{
  tracked_corr_ = nullptr;
}


void bwm_observer_video::remove_selected_corr_match()
{
  unsigned frame = 0, corr_index = 0;
  bwm_soview2D_cross* cross =nullptr;
  if (!this->find_selected_video_corr(frame, corr_index, cross))
    return;
  // remove the cross from the tableau
  this->remove(cross);
  // remove the cross from the soview map
  corr_soview_map_[frame].erase(corr_index);
  // remove the correspondence displayed by cross
  bwm_video_corr_sptr c = video_corrs_[corr_index];
  c->remove(frame);
  // If the correspondence has no matches, then remove it entirely
  if (!c->num_matches())
    {video_corrs_.erase(corr_index);}
  // redisplay
  this->post_redraw();
}

//: remove the selected correspondence entirely
void bwm_observer_video::remove_selected_corr()
{
  unsigned frame = 0, corr_index = 0;
  bwm_soview2D_cross* cross =nullptr;
  if (!this->find_selected_video_corr(frame, corr_index, cross))
    return;
  bwm_video_corr_sptr c = video_corrs_[corr_index];
  // remove any crosses displayed by c
  std::map<unsigned, std::map<unsigned, bwm_soview2D_cross*> >::iterator cit;
  cit = corr_soview_map_.begin();
  for (; cit != corr_soview_map_.end(); ++cit)
  {
    std::map<unsigned, bwm_soview2D_cross*>& cm = (*cit).second;
    std::map<unsigned, bwm_soview2D_cross*>::iterator  fit;
    fit = cm.find(corr_index);
    if (fit == cm.end())
      continue;
    this->remove((*fit).second);
    cm.erase(fit);
  }
  video_corrs_.erase(corr_index);
  // redisplay
  this->post_redraw();
}

//: select the correspondence from the previous frame that is closest to the new correspondence
void bwm_observer_video::select_closest_match()
{
  this->deselect_all();
  // correspondence
  vgl_point_2d<double> pt;
  bool valid = this->corr_image_pt(pt);
  if (!valid)
    return;

  // find the closest correspondence in position and time (not in current frame)
  unsigned cur_frame =  video_istr_->frame_number();
  double min_dist = vnl_numeric_traits<double>::maxval;
  unsigned corr_index = 0;
  unsigned frame = 0;
  bool found_closest = false;
  std::map<unsigned, bwm_video_corr_sptr>::iterator cit = video_corrs_.begin();
  for (; cit != video_corrs_.end(); ++cit)
  {
    unsigned nframe=0;
    if (!(*cit).second->nearest_frame(cur_frame, nframe))
      continue;
    if (nframe == cur_frame)
      continue;
    vgl_point_2d<double> ppt;
    if (!(*cit).second->match(nframe, ppt))
      continue;
    double d = vgl_distance(pt, ppt);
    if (d<min_dist)
    {
      min_dist = d;
      corr_index = (*cit).second->id();
      frame = nframe;
      found_closest = true;
    }
  }
  if (!found_closest)
    return;
  // now find the associated soview and select it
  bwm_soview2D_cross* cross = corr_soview_map_[frame][corr_index];
  if (!cross) return;
  unsigned cid = cross->get_id();
  this->select(cid);
}

// this virtual method is called when a new correspondence cross is created by
// observer_vgui's handle method.
void bwm_observer_video::correspondence_action()
{
  if (!tracked_corr_)
    this->select_closest_match();
}

// Create a new video correspondence from the corr currently set
// on bwm_observer_vgui. Add to the list of video correspondences for
// *this* observer
bool bwm_observer_video::add_video_corr()
{
  if (!video_istr_)
    return false;
  vgl_point_2d<double> pt;
  bool valid = this->corr_image_pt(pt);
  if (!valid)
    return false;
  unsigned frame_index = video_istr_->frame_number();
  bwm_video_corr_sptr corr = new bwm_video_corr(this, frame_index, pt);
  video_corrs_[corr->id()] = corr;
  this->remove_corr_pt();
  this->display_video_corr(corr, frame_index, CORR_STYLE);
  return true;
}


void bwm_observer_video::set_world_pt(double lat, double lon, double elev)
{
  unsigned frame = 0, corr_index = 0;
  bwm_soview2D_cross* cross =nullptr;
  if (!this->find_selected_video_corr(frame, corr_index, cross))
    return;
  bwm_video_corr_sptr c = video_corrs_[corr_index];
  c->set_world_pt(vgl_point_3d<double>(lon,lat,elev));
}

bool bwm_observer_video::add_match()
{
  if (!video_istr_) return false;
  unsigned cur_frame =  video_istr_->frame_number();
  vgl_point_2d<double> pt;
  if (!tracked_corr_)
  {
    unsigned frame = 0, corr_index = 0;
    bwm_soview2D_cross* cross =nullptr;
    if (!this->find_selected_video_corr(frame, corr_index, cross))
    {
      // no corr exists or no previous corr selected
      return this->add_video_corr();
    }
    // if the correspondence is in *this* frame then the match already exists
    // and there is nothing to do
    if (frame == cur_frame)
      return true;

    bool valid = this->corr_image_pt(pt);
    if (!valid)
      return false;
    this->remove_corr_pt();
    bwm_video_corr_sptr c = video_corrs_[corr_index];
    if (!c) return false;
    // if the frame match already exists do nothing
    if (!c->add(cur_frame, pt))
      return true;
    // display the new match
    this->display_video_corr(c, cur_frame, CORR_STYLE);
    // display the fact that the match to the previous frame soview is successful
    this->remove(cross);
    corr_soview_map_[frame].erase(corr_index);
    this->display_video_corr(c, frame, MATCHED_STYLE);
    return true;
  }
  bool valid = this->corr_image_pt(pt);
  if (!valid)
    return false;
  this->remove_corr_pt();
  // if the frame match already exists do nothing
  if (!tracked_corr_->add(cur_frame, pt))
    return true;
  // display the new match
  this->display_video_corr(tracked_corr_, cur_frame, CORR_STYLE);
  return true;
}

bool bwm_observer_video::remove_match()
{
  unsigned frame = 0, corr_index = 0;
  bwm_soview2D_cross* cross =nullptr;
  if (!this->find_selected_video_corr(frame, corr_index, cross))
    return false;

  this->remove_selected_corr_match();

  bwm_video_corr_sptr c = video_corrs_[corr_index];
  c->nearest_frame(video_istr_->frame_number(), frame);
  this->display_video_corr(c, frame, CORR_STYLE);
  return true;
}

bool bwm_observer_video::update_match(vgl_point_2d<double> &pt)
{
  unsigned cur_frame =  video_istr_->frame_number();

  if (!tracked_corr_)
  {
    unsigned frame = 0, corr_index = 0;
    bwm_soview2D_cross* cross =nullptr;
    if (this->find_selected_video_corr(frame, corr_index, cross))
    {//Selected cross found!
      if (cur_frame == frame)
      {
        this->video_corrs_[corr_index]->update(cur_frame, pt);

        cross->x = pt.x(); //Update graphics object
        cross->y = pt.y(); //Update graphics object
        //Yeah, I get I'm probably not supposed to do it this way
        //but I don't currently know how I'm supposed to, and this
        //worked safey.
      }
      else
      {//Else, it was a missing point that needs to be added CAREFULLY
        this->video_corrs_[corr_index]->add(cur_frame, pt);
        cross->x = pt.x(); //Update graphics object
        cross->y = pt.y(); //Update graphics object
        cross->set_colour(PREV_STYLE->rgba[0], PREV_STYLE->rgba[1], PREV_STYLE->rgba[2]);
      }
    }
    return true;
  }
  return false;
}


bwm_soview2D_cross* bwm_observer_video::corr_soview(unsigned corr_index)
{
  if (!video_istr_)
    return nullptr;

  unsigned frame_index = video_istr_->frame_number();
  return corr_soview_map_[frame_index][corr_index];
}

  //: add video corr at a selected vertex
bool bwm_observer_video::add_match_at_vertex()
{
  if (!video_istr_)
    return false;
    // get the currently selected vertex
  bwm_soview2D_vertex* sov =
    (bwm_soview2D_vertex*)bwm_observer_img::get_selected_object("bwm_soview2D_vertex");
  // extract the location
  if (!sov)
  {
    std::cerr << "In bwm_observer_video::add_video_corr_at_vertex() -"
             << " no vertex selected\n";
    return false;
  }
  this->add_cross(sov->x, sov->y, 2.0f);
  corr_valid_ = true;
  if (!this->add_match()) return false;
  if (!tracked_corr_) {
    std::map<unsigned, bwm_video_corr_sptr>::iterator cit = video_corrs_.end();
    //get the last match added
    cit--;
    unsigned corr_id = (*cit).first;
    bwm_soview2D_cross* soview = this->corr_soview(corr_id);
    this->select(soview->get_id());
    this->post_redraw();
  }
  return true;
}

bool bwm_observer_video::is_displayed(bwm_video_corr_sptr const& corr,
                                      unsigned frame_index)
{
  if (!corr)
    return false;
  // find the correspondence in the display map
  std::map<unsigned, bwm_soview2D_cross*>::iterator fit;
  fit = corr_soview_map_[frame_index].find(corr->id());
  if (fit == corr_soview_map_[frame_index].end())
    return false;
  return true;
}

// display a single video correspondence
void bwm_observer_video::display_video_corr(bwm_video_corr_sptr const& corr,
                                            unsigned frame_index,
                                            vgui_style_sptr const& style)
{
  // if there is no match for the specified frame, do nothing
  vgl_point_2d<double> pt;
  if (!corr||!corr->match(frame_index, pt))
    return;
  // if already in the display, do nothing
  if (this->is_displayed(corr, frame_index))
    return;
  // add to the display
  bwm_soview2D_cross* cross = new bwm_soview2D_cross(pt.x(), pt.y(), 2.0f);
  cross->set_style(style);
  // if the first corr for frame_index must add a map
  std::map<unsigned, std::map<unsigned, bwm_soview2D_cross*> >::iterator fit;
  fit = corr_soview_map_.find(frame_index);
  if (fit == corr_soview_map_.end())
    corr_soview_map_[frame_index] = std::map<unsigned, bwm_soview2D_cross*>();
  corr_soview_map_[frame_index][corr->id()]=cross;
  // skip over vgui_easy2D, since that tableau overrides the style setting
  vgui_displaylist2D_tableau::add(cross);
  this->post_redraw();
}

void bwm_observer_video::
display_projected_3d_point(bwm_video_corr_sptr const& corr)
{
  vgui_soview2D_point* pview = world_pt_map_[corr->id()];
  if (pview) this->remove(pview);
  vgl_point_2d<double> image_pt;
  this->proj_point(corr->world_pt(), image_pt);
  pview = new vgui_soview2D_point(image_pt.x(), image_pt.y());
  pview->set_style(POINT_3D_STYLE);
  vgui_displaylist2D_tableau::add(pview);
  world_pt_map_[corr->id()]=pview;
}

// display the current set of accepted video correspondences for the current frame
void bwm_observer_video::display_video_corrs(unsigned frame_index)
{
  unsigned cur_frame =  video_istr_->frame_number();
  vgui_style_sptr style = CORR_STYLE;
  if (frame_index!=cur_frame) {
    style = PREV_STYLE;
  }
  std::map<unsigned, bwm_video_corr_sptr>::iterator cit = video_corrs_.begin();
  for (;cit != video_corrs_.end(); ++cit)
  {
    bwm_video_corr_sptr c = (*cit).second;
    this->display_video_corr(c, frame_index, style);
    if (display_world_pts_)
      this->display_projected_3d_point(c);
  }
}

// display correspondences defined for current frame
void bwm_observer_video::display_current_video_corrs()
{
  if (!video_istr_)
    return;
  unsigned frame_index = video_istr_->frame_number();
  this->display_video_corrs(frame_index);
}

void bwm_observer_video::display_corr_track()
{
  if (!video_istr_) return;
  unsigned cur_frame = video_istr_->frame_number();
  // find the nearest previous frame that has a correspondence match
  if (!video_corrs_.size()) return;
  // display the closest match to the current frame
  std::map<unsigned, bwm_video_corr_sptr>::iterator cit = video_corrs_.begin();
  for (; cit != video_corrs_.end(); ++cit)
  {
    bwm_video_corr_sptr c = (*cit).second;
    unsigned nframe=0;
    if (!c->nearest_frame(cur_frame, nframe))
      continue;
    if (nframe!=cur_frame)
      this->display_video_corr(c, nframe, CORR_STYLE);
    else
      this->display_video_corr(c, nframe, PREV_STYLE); //Works

    if (cam_istr_&&c->world_pt_valid()&&display_world_pts_)
      this->display_projected_3d_point(c);
  }
}

void bwm_observer_video::display_selected_world_pt()
{
  std::vector<vgui_soview2D*> selected =
    this->get_selected_objects("bwm_soview2D_cross");
  if (selected.size()<1) {
    std::cerr << "Select 2-d points\n";
    return ;
  }
  std::vector<bwm_video_corr_sptr> corrs;
  for (std::vector<vgui_soview2D*>::iterator cit = selected.begin();
       cit != selected.end(); ++cit)
  {
    bwm_soview2D_cross* cross = static_cast<bwm_soview2D_cross*>(*cit);
    if (!cross) return ;
    std::map<unsigned, std::map<unsigned, bwm_soview2D_cross*> >::iterator fit=
      corr_soview_map_.begin();
    bool found = false;
    unsigned corr_index = 0;
    for (; fit != corr_soview_map_.end()&&!found; ++fit)
      for (std::map<unsigned, bwm_soview2D_cross*>::iterator mit = (*fit).second.begin();
           mit != (*fit).second.end()&&!found; ++mit)
        if ((*mit).second == cross)
        {
          found = true;
          corr_index = (*mit).first;
        }
    if (found) {
      bwm_video_corr_sptr corr = video_corrs_[corr_index];
      if (corr)
        this->display_projected_3d_point(corr);
    }
  }
}

// clear the entire corr display map
void bwm_observer_video::clear_corr_display_map()
{
  std::map<unsigned, std::map<unsigned, bwm_soview2D_cross*> >::iterator fit =
    corr_soview_map_.begin();
  for (; fit != corr_soview_map_.end(); ++fit) {
    std::map<unsigned, bwm_soview2D_cross*>::iterator cit =
      (*fit).second.begin();
    for (; cit != (*fit).second.end(); ++cit)
      if ((*cit).second) remove((*cit).second);
    (*fit).second.clear();
  }
  corr_soview_map_.clear();
}

void bwm_observer_video::clear_world_pt_display_map()
{
  std::map<unsigned, vgui_soview2D_point*>::iterator wit =
    world_pt_map_.begin();
  for (; wit != world_pt_map_.end(); ++wit)
    this->remove((*wit).second);
  this->world_pt_map_.clear();
}

// clear coor display and world point display
void bwm_observer_video::clear_video_corrs_display()
{
  this->remove_corr_pt();
  this->clear_corr_display_map();
  this->clear_world_pt_display_map();
}

std::vector<bwm_video_corr_sptr> bwm_observer_video::corrs()
{
  std::vector<bwm_video_corr_sptr> temp;
  std::map<unsigned, bwm_video_corr_sptr>::iterator cit = video_corrs_.begin();
  for (; cit != video_corrs_.end(); ++cit)
    temp.push_back((*cit).second);
  return temp;
}

void
bwm_observer_video::set_corrs(std::vector<bwm_video_corr_sptr> const& corrs)
{
  for (std::vector<bwm_video_corr_sptr>::const_iterator cit = corrs.begin();
       cit != corrs.end(); ++cit)
    video_corrs_[(*cit)->id()] = (*cit);
}

bool bwm_observer_video::extract_world_plane(vgl_plane_3d<double>&  plane)
{
  std::vector<vgui_soview2D*> selected =
    this->get_selected_objects("bwm_soview2D_cross");
  if (selected.size()!=3) {
    std::cerr << "Select exactly 3 correspondences to specify the vertices of a triangle\n";
    return false;
  }
  std::vector<bwm_video_corr_sptr> corrs;
  for (std::vector<vgui_soview2D*>::iterator cit = selected.begin();
       cit != selected.end(); ++cit)
  {
    bwm_soview2D_cross* cross = static_cast<bwm_soview2D_cross*>(*cit);
    if (!cross) return false;
    std::map<unsigned, std::map<unsigned, bwm_soview2D_cross*> >::iterator fit=
      corr_soview_map_.begin();
    bool found = false;
    unsigned corr_index = 0;
    for (; fit != corr_soview_map_.end()&&!found; ++fit)
      for (std::map<unsigned, bwm_soview2D_cross*>::iterator mit = (*fit).second.begin();
           mit != (*fit).second.end()&&!found; ++mit)
        if ((*mit).second == cross)
        {
          found = true;
          corr_index = (*mit).first;
        }
    if (!found)
      return false;
    bwm_video_corr_sptr corr = video_corrs_[corr_index];
    if (!corr)
      return false;
    corrs.push_back(corr);
  }
  vgl_point_3d<double> pt0 = corrs[0]->world_pt();
  vgl_point_3d<double> pt1 = corrs[1]->world_pt();
  vgl_point_3d<double> pt2 = corrs[2]->world_pt();
  vgl_plane_3d<double> pl(pt0, pt1, pt2);
  double a = pl.a(), b= pl.b(), c=pl.c();
  double norm = std::sqrt(a*a + b*b +c*c);
  plane = vgl_plane_3d<double>(a/norm, b/norm, c/norm, pl.d()/norm);
  return true;
}

//: extract two-class neighborhoods from a video stream
bool bwm_observer_video::
extract_neighborhoods(unsigned nhd_radius_x,unsigned nhd_radius_y,
                      std::vector<std::vector<vnl_matrix<float> > >& nhds)
{
  std::vector<vgui_soview2D*> selected =
    this->get_selected_objects("bwm_soview2D_cross");

  if (selected.size()==0) {
    std::cerr << "Select one or more correspondences to specify each neighborhood center\n";
    return false;
  }

  nhds.clear();
  int rd_x = nhd_radius_x;
  int rd_y = nhd_radius_y;
  std::vector<bwm_video_corr_sptr> corrs;
  for (std::vector<vgui_soview2D*>::iterator cit = selected.begin();
       cit != selected.end(); ++cit)
  {
    bwm_soview2D_cross* cross = static_cast<bwm_soview2D_cross*>(*cit);
    if (!cross) return false;
    std::map<unsigned, std::map<unsigned, bwm_soview2D_cross*> >::iterator fit=
      corr_soview_map_.begin();
    bool found = false;
    unsigned corr_index = 0;
    for (; fit != corr_soview_map_.end()&&!found; ++fit)
      for (std::map<unsigned, bwm_soview2D_cross*>::iterator mit = (*fit).second.begin();
           mit != (*fit).second.end()&&!found; ++mit)
        if ((*mit).second == cross)
        {
          found = true;
          corr_index = (*mit).first;
        }
    if (!found)
      return false;
    bwm_video_corr_sptr corr = video_corrs_[corr_index];
    if (!corr)
      return false;
    corrs.push_back(corr);
  }
  if (corrs.size()==0)
    return false;
  if (!video_istr_||!video_istr_->is_open()||!video_istr_->is_seekable())
    return false;
  unsigned dim_x = 2*nhd_radius_x +1;
  unsigned dim_y = 2*nhd_radius_y +1;

  for (unsigned i = 0; i<corrs.size(); ++i)
  {
    std::vector<vnl_matrix<float> > temp;
    bwm_video_corr_sptr corr = corrs[i];
    video_istr_->seek_frame(0);
    unsigned index = 0;
    while (true) {
      vidl_frame_sptr frame = video_istr_->current_frame();
      if (!frame)
        return false;
      vil_image_view_base_sptr fb = vidl_convert_wrap_in_view(*frame);
      vil_image_view<float> fimg = *vil_convert_cast(float(), fb);
      // assume for now there are no problems with image boundaries
      vgl_point_2d<double> pt;
      if (corr->match(index, pt)) {
        int u = static_cast<unsigned>(pt.x()),
          v = static_cast<unsigned>(pt.y());
        vnl_matrix<float> nb(dim_y, dim_x);
        for (int ir = -rd_y; ir<= rd_y; ++ir)
          for (int ic = -rd_x; ic<= rd_x; ++ic)
          {
            int r = nhd_radius_y+ir, c = nhd_radius_x+ic;
            nb[r][c]=fimg(u+ic, v+ir);
          }

        temp.push_back(nb);
      }
      if (!video_istr_->advance())
        break;
      index++;
    }
    nhds.push_back(temp);
  }
  return true;
}

bool bwm_observer_video::
extract_histograms(std::vector<bsta_histogram<float> >& hists)
{
  video_istr_->seek_frame(0);
  while (true) {
    vidl_frame_sptr frame = video_istr_->current_frame();
    if (!frame)
      return false;
    bsta_histogram<float> h(0.0f, 255.0f, 256);
    vil_image_view_base_sptr fb = vidl_convert_wrap_in_view(*frame);
    vil_image_view<float> fimg = *vil_convert_cast(float(), fb);
    unsigned ni = fimg.ni(), nj = fimg.nj();
    for (unsigned j = 0; j<nj; ++j)
      for (unsigned i = 0; i<ni; ++i)
      {
        float v = fimg(i, j);
        h.upcount(v, 1.0f);
      }
    hists.push_back(h);
    if (!video_istr_->advance())
      break;
  }
  return true;
}

bool bwm_observer_video::
save_as_image_list(std::string const& path)
{
  if (!vul_file::is_directory(path))
    return false;
  vidl_image_list_ostream os(path);
  if (!os.is_open())
    return false;

  video_istr_->seek_frame(0);
  while (true) {
    vidl_frame_sptr frame = video_istr_->current_frame();
    if (!frame)
      return false;
    if (!os.write_frame(frame))
      return false;
    if (!video_istr_->advance())
      break;
  }
  os.close();
  return true;
}

void bwm_observer_video::proj_point(vgl_point_3d<double> world_pt,
                                    vgl_point_2d<double> &image_pt)
{
  vpgl_perspective_camera<double>* cam = cam_istr_->current_camera();
  if (!cam) {
    std::cout << "ERROR: null camera in ::proj_point(..)\n";
    image_pt.set(0.0, 0.0);
    return;
  }
  bwm_observer_cam::camera_ = cam;
  bwm_observer_cam::proj_point(world_pt, image_pt);
}

void bwm_observer_video::proj_line(vsol_line_3d_sptr line_3d,
                                   vsol_line_2d_sptr &line_2d)
{
  vpgl_perspective_camera<double>* cam = cam_istr_->current_camera();
  if (!cam) {
    std::cout << "ERROR: null camera in ::proj_line(..)\n";
    line_2d = nullptr;
    return;
  }
  bwm_observer_cam::camera_ = cam;
  bwm_observer_cam::proj_line(line_3d, line_2d);
}

void bwm_observer_video::proj_poly(vsol_polygon_3d_sptr poly3d,
                                   vsol_polygon_2d_sptr& poly2d)
{
  vpgl_perspective_camera<double>* cam = cam_istr_->current_camera();
  if (!cam) {
    std::cout << "ERROR: null camera in ::proj_poly(..)\n";
    poly2d = nullptr;
    return;
  }
  bwm_observer_cam::camera_ = cam;
  bwm_observer_cam::proj_poly(poly3d, poly2d);
}

void bwm_observer_video::display_3d_objects()
{
  //check if there are any to display
  unsigned nobj = bwm_world::instance()->n_objects();
  if (!nobj) return;
  //tell each observable object to notify observers
  std::vector<bwm_observable_sptr> objs = bwm_world::instance()->objects();
  for (std::vector<bwm_observable_sptr>::iterator oit = objs.begin();
       oit != objs.end(); ++oit) {
    (*oit)->send_update();
  }
}

bool bwm_observer_video::
intersect_ray_and_plane(vgl_point_2d<double> img_point,
                        vgl_plane_3d<double> plane,
                        vgl_point_3d<double> &world_point)
{
  if (!camera_)
    return false;
  vpgl_proj_camera<double>* proj_cam =
    static_cast<vpgl_proj_camera<double> *> (cam_istr_->current_camera());//camera_);
  vgl_homg_point_2d<double> img_point_h(img_point);
  vgl_homg_line_3d_2_points<double> ray = proj_cam->backproject(img_point_h);
  vgl_homg_operators_3d<double> oper;
  vgl_homg_point_3d<double> p = oper.intersect_line_and_plane(ray,plane);
  world_point = p;
  return true;
}

unsigned bwm_observer_video::create_polygon(vsol_polygon_2d_sptr poly2d)
{
  unsigned frame = this->video_istr_->frame_number();

  float *x, *y;
  bwm_algo::get_vertices_xy(poly2d, &x, &y);
  unsigned nverts = poly2d->size();

  this->set_foreground(1,1,0);
  bgui_vsol_soview2D_polygon* polygon = this->add_vsol_polygon_2d(poly2d);
  obj_list[polygon->get_id()] = polygon;
  if (draw_mode_ == 1)
    polygon->set_selectable(false);

  std::vector<bwm_soview2D_vertex*> verts;
  this->set_foreground(0,1,0);
  for (unsigned i = 0; i<nverts; ++i) {
    bwm_soview2D_vertex* vertex = new bwm_soview2D_vertex(x[i],y[i],0.5f, polygon, i);
    if (draw_mode_ == 0)
      vertex->set_selectable(false);
    this->add(vertex);
    verts.push_back(vertex);
  }
  vert_list[polygon->get_id()] = verts;

  poly2d->set_id(polygon->get_id());
  this->frame_polygon_map_[frame].push_back(poly2d);
  this->frame_change_map_[frame].push_back("no change");
  this->id_pos_map_[polygon->get_id()] = this->frame_polygon_map_[frame].size()-1;
  return polygon->get_id();
}

void bwm_observer_video::display_polygons_frame()
{
  unsigned frame = this->video_istr_->frame_number();

  std::map<unsigned, std::vector<vsol_polygon_2d_sptr> >::iterator
    f_itr = this->frame_polygon_map_.find(frame);

  std::map<unsigned,std::vector<std::string> >::iterator
    s_itr = this->frame_change_map_.find(frame);

  vgui_text_tableau_sptr tt = img_tab_->text_tab();
  tt->clear();

  //clear the objects
  this->id_pos_map_.clear();

  this->clear_objects();

  if ( f_itr != this->frame_polygon_map_.end() )
  {
    for ( unsigned i = 0; i < f_itr->second.size(); ++i )
    {
      float *x, *y;
      bwm_algo::get_vertices_xy(f_itr->second[i], &x, &y);
      unsigned nverts = f_itr->second[i]->size();

      this->set_foreground(1,1,0);
      bgui_vsol_soview2D_polygon* polygon = this->add_vsol_polygon_2d(f_itr->second[i]);
      obj_list[polygon->get_id()] = polygon;
      if (draw_mode_ == 1)
        polygon->set_selectable(false);

      std::vector<bwm_soview2D_vertex*> verts;
      this->set_foreground(0,1,0);
      for (unsigned j = 0; j<nverts; ++j) {
        bwm_soview2D_vertex* vertex = new bwm_soview2D_vertex(x[j],y[j],0.5f, polygon, j);
        if (draw_mode_ == 0)
          vertex->set_selectable(false);
        this->add(vertex);
        verts.push_back(vertex);
      }
      vert_list[polygon->get_id()] = verts;

      f_itr->second[i]->set_id(polygon->get_id());

      this->id_pos_map_[polygon->get_id()] = i;

      if ( s_itr != this->frame_change_map_.end() )
      {
        if ( s_itr->second.size() > i )
        {
          std::string change_type = s_itr->second[i];
          vsol_polygon_2d_sptr poly = f_itr->second[i];
          vsol_point_2d_sptr cent = poly->centroid();
          tt->add((poly->centroid())->x(),cent->y(),change_type);
        } //end check if we have correct number of change types
      } //check if we have change types
#if 0
      return polygon->get_id();
#endif
    } //end polygon iteration
  }
}

void bwm_observer_video::delete_polygon( vgui_soview* obj )
{
  // remove the polygon
  unsigned poly_id = obj->get_id();
  this->remove(obj);
  obj_list.erase(poly_id);

  // remove the vertices
  std::vector<bwm_soview2D_vertex*>  v = vert_list[poly_id];
  for (unsigned i=0; i<v.size(); i++) {
    this->remove(v[i]);
  }
  vert_list.erase(poly_id);

  unsigned frame = this->video_istr_->frame_number();

  unsigned poly_idx = this->id_pos_map_[poly_id];

  this->frame_polygon_map_[frame].erase(this->frame_polygon_map_[frame].begin()+poly_idx);

  this->frame_change_map_[frame].erase(this->frame_change_map_[frame].begin()+poly_idx);

  this->id_pos_map_.clear();

  for (unsigned i = 0; i < this->frame_polygon_map_[frame].size(); ++i )
    this->id_pos_map_[this->frame_polygon_map_[frame][i]->get_id()] = i;

  this->post_redraw();
}

void bwm_observer_video::delete_all()
{
  unsigned frame = this->video_istr_->frame_number();
  this->clear_objects();
  this->frame_polygon_map_[frame].clear();
  this->frame_change_map_[frame].clear();
  this->id_pos_map_.clear();
  this->display_current_frame();
}

void bwm_observer_video::delete_all_frames()
{
  this->clear_objects();
  this->frame_polygon_map_.clear();
  this->frame_change_map_.clear();
  this->id_pos_map_.clear();
  this->display_current_frame();
}

void bwm_observer_video::init_mask()
{
  std::map<unsigned, std::vector< std::string> >::iterator
    frame_itr, frame_end = this->frame_change_map_.end();

  for ( frame_itr = this->frame_change_map_.begin(); frame_itr != frame_end; ++frame_itr )
  {
    std::vector<std::string>::iterator string_itr, string_end = frame_itr->second.end();

    for ( string_itr = frame_itr->second.begin(); string_itr != string_end; ++string_itr )
      *string_itr = "no change";
  }//end frame iteration
}

void bwm_observer_video::add_poly_to_mask()
{
  unsigned frame = this->video_istr_->frame_number();
  std::vector<vgui_soview2D*> polys = get_selected_objects(POLYGON_TYPE);
  for ( unsigned i = 0; i < polys.size(); ++i )
  {
    unsigned poly_idx = this->id_pos_map_[polys[i]->get_id()];
    this->frame_change_map_[frame][poly_idx] = this->change_type_;
  }
}

void bwm_observer_video::remove_poly_from_mask()
{
  unsigned frame = this->video_istr_->frame_number();
  std::vector<vgui_soview2D*> polys = get_selected_objects(POLYGON_TYPE);
  for ( unsigned i = 0; i < polys.size(); ++i )
  {
    unsigned poly_idx = this->id_pos_map_[polys[i]->get_id()];
    this->frame_change_map_[frame][poly_idx] = "no change";
  }
}

bool bwm_observer_video::save_changes_binary()
{
  std::string fname = bwm_utils::select_file();
  vsl_b_ofstream os(fname);

  // total number of frames but there may not be a change polygon at a frame

  std::map<unsigned, unsigned> frame_num_changes;

  std::map<unsigned, std::vector< std::string> >::const_iterator
    frame_change_itr, frame_change_end = this->frame_change_map_.end();

  for ( frame_change_itr = this->frame_change_map_.begin(); frame_change_itr != frame_change_end; ++frame_change_itr )
  {
    std::vector< std::string >::const_iterator string_itr, string_end = frame_change_itr->second.end();

    unsigned nchanges = 0;
    for ( string_itr = frame_change_itr->second.begin(); string_itr != string_end; ++string_itr )
    {
      if ( string_itr->compare("no change") != 0 )
        ++nchanges;
    }//end string vector iteration

    if ( nchanges > 0 )
      frame_num_changes[frame_change_itr->first] = nchanges;
  }//end frame iteration


  if ( frame_num_changes.size() > 0 )
  {
    //write the number of frames that have a change
    vsl_b_write(os,frame_num_changes.size());

    std::map<unsigned, unsigned>::const_iterator frame_itr, frame_end = frame_num_changes.end();

    for ( frame_itr = frame_num_changes.begin(); frame_itr != frame_end; ++frame_itr )
    {
      //write the frame number
      vsl_b_write(os,frame_itr->first);

      //write the number of changes
      vsl_b_write(os,frame_itr->second);

      for ( unsigned i = 0; i < this->frame_change_map_[frame_itr->first].size(); ++i )
      {
        if ( this->frame_change_map_[frame_itr->first][i].compare("no change") != 0 )
        {
          //write the polygon
          vsl_b_write(os,this->frame_polygon_map_[frame_itr->first][i].as_pointer());
          //write the change type
          vsl_b_write(os,this->frame_change_map_[frame_itr->first][i]);
        }
      }//end string iteration
    }//end frame iteration
  } //end if ( nframes > 0 )

  return true;
}

bool bwm_observer_video::load_changes_binary()
{
  std::string fname = bwm_utils::select_file();
  vsl_b_ifstream is(fname);

  // total number of frames but there may not be a change polygon at a frame
  unsigned nframes = 0;

  vsl_b_read(is, nframes);

  if ( nframes > 0 )
  {
    for ( unsigned frame_idx = 0; frame_idx < nframes; ++frame_idx )
    {
      //read frame number
      unsigned frame;
      vsl_b_read(is, frame);

      //read number of changes
      unsigned nchanges;
      vsl_b_read(is, nchanges);

      for ( unsigned i = 0; i < nchanges; ++i )
      {
        //read the polygon
        vsol_polygon_2d* polygon_ptr = new vsol_polygon_2d;
        vsl_b_read(is, polygon_ptr);
        vsol_polygon_2d_sptr polygon_sptr(polygon_ptr);

        this->frame_polygon_map_[frame].push_back(polygon_sptr);

        //read the change type
        std::string change_type;
        vsl_b_read(is,change_type);
        this->frame_change_map_[frame].push_back(change_type);
      }//end change iteration
    }//end frame iteration
  } //end if ( nframes > 0 )

  this->display_current_frame();

  return true;
}
