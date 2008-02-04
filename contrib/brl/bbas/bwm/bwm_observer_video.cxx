#include "bwm_observer_video.h"
//:
// \file
#include "bwm_observer_mgr.h"
#include "video/bwm_video_corr.h"
#include "algo/bwm_soview2D_cross.h"
#include <vcl_sstream.h>
#include <vgl/vgl_distance.h>
#include <vgui/vgui.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_numeric_traits.h>
#include <vil/vil_image_view.h>
#include <vidl2/vidl2_image_list_istream.h>
#include <bwm/video/bwm_video_cam_istream.h>
#include <vidl2/vidl2_frame.h>
#include <vidl2/vidl2_convert.h>

bool bwm_observer_video::handle(const vgui_event &e)
{
  if (!video_istr_) return bwm_observer_cam::handle(e);
  if (e.type==vgui_KEY_PRESS && e.key == vgui_CURSOR_LEFT)
  {
    unsigned cur_frame =  video_istr_->frame_number();
    if (e.modifier == vgui_SHIFT){
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


bool bwm_observer_video::open_video_stream(vcl_string const& video_glob)
{
  //for now assume we are opening an image_list codec
  video_istr_ = new vidl2_image_list_istream(video_glob);
  bool open = video_istr_->is_open();
  if  (open)
    this->seek(0);
  return open;
}

bool bwm_observer_video::open_camera_stream(vcl_string const& camera_glob)
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
  vcl_map<unsigned, bwm_video_corr_sptr>::iterator cit = video_corrs_.begin();
  for (;cit != video_corrs_.end(); ++cit)
  {
    bwm_video_corr_sptr c = (*cit).second;
    vgl_point_2d<double> pt;
    if (!c->match(cur_frame, pt)) continue;
    unsigned index = (*cit).first;
    vcl_stringstream ind_str;
    ind_str << index;
    float x = static_cast<float>(pt.x()+2.0);
    float y = static_cast<float>(pt.y()-2.0);
    tt->add(x, y, ind_str.str());
  }
  tt->post_redraw();
}

void bwm_observer_video::display_current_frame()
{
  if (video_istr_) {
    unsigned int frame_num = video_istr_->frame_number();
    if (bwm_observer_mgr::instance()->in_corr_picking())
      img_tab_->lock_linenum(true);
    else
      img_tab_->lock_linenum(false);
    if (frame_num == unsigned(-1))
      vgui::out << "invalid frame\n";
    else
      vgui::out << "frame["<< frame_num <<"]\n";

    vidl2_frame_sptr frame = video_istr_->current_frame();
    if (!frame)
      img_tab_->set_image_resource(NULL);
    else if (frame->pixel_format() == VIDL2_PIXEL_FORMAT_MONO_16){
      static vil_image_view<vxl_uint_16> img;
      if (vidl2_convert_to_view(*frame,img))
        img_tab_->set_image_view(img);
      else
        img_tab_->set_image_resource(NULL);
    }
    else{
      static vil_image_view<vxl_byte> img;
      if (vidl2_convert_to_view(*frame,img,VIDL2_PIXEL_COLOR_RGB))
        img_tab_->set_image_view(img);
      else
        img_tab_->set_image_resource(NULL);
    }
  }
  img_tab_->post_redraw();
  vgui::run_till_idle();
  if (bwm_observer_mgr::instance()->in_corr_picking())
  {
    this->clear_video_corrs_display();
      this->display_corr_track();
  }
  this->display_corr_index();
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
    vcl_cout << "No movie has been loaded\n";
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
    //Delay until the time interval has passed
    while (t.all()<time_interval_) ;
    t.mark();
    ++count;
  }
  long time = t2.all();
  vcl_cout << "average play time " << double(time)/count << vcl_endl;

  // if played to the end, go back to the first frame;
  if (play_video_)
    this->stop();
}

//Stop the video and return to the first frame
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

//Stop the video without returning to the first frame
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
  vcl_map<unsigned, vcl_map<unsigned, bwm_soview2D_cross*> >::iterator fit =
    corr_soview_map_.begin();
  bool found = false;
  for (; fit != corr_soview_map_.end()&&!found; ++fit)
    for (vcl_map<unsigned, bwm_soview2D_cross*>::iterator mit = (*fit).second.begin();
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
  bwm_soview2D_cross* cross =0;
  if (!this->find_selected_video_corr(frame, corr_index, cross))
    return;
  tracked_corr_ = video_corrs_[corr_index];
}


//: unset selected corr for tracking
void bwm_observer_video::unset_selected_corr_for_tracking()
{
  tracked_corr_ = 0;
}


void bwm_observer_video::remove_selected_corr_match()
{
  unsigned frame = 0, corr_index = 0;
  bwm_soview2D_cross* cross =0;
  if (!this->find_selected_video_corr(frame, corr_index, cross))
    return;
  //remove the cross from the tableau
  this->remove(cross);
  //remove the cross from the soview map
  corr_soview_map_[frame].erase(corr_index);
  //remove the correspondence displayed by cross
  bwm_video_corr_sptr c = video_corrs_[corr_index];
  c->remove(frame);
  //If the correspondence has no matches, then remove it entirely
  if (!c->num_matches())
    {video_corrs_.erase(corr_index);}
  //redisplay
  this->post_redraw();
}

//: remove the selected correspondence entirely
void bwm_observer_video::remove_selected_corr()
{
  unsigned frame = 0, corr_index = 0;
  bwm_soview2D_cross* cross =0;
  if (!this->find_selected_video_corr(frame, corr_index, cross))
    return;
  bwm_video_corr_sptr c = video_corrs_[corr_index];
  //remove any crosses displayed by c
  vcl_map<unsigned, vcl_map<unsigned, bwm_soview2D_cross*> >::iterator cit;
  cit = corr_soview_map_.begin();
  for (; cit != corr_soview_map_.end(); ++cit)
  {
    vcl_map<unsigned, bwm_soview2D_cross*>& cm = (*cit).second;
    vcl_map<unsigned, bwm_soview2D_cross*>::iterator  fit;
    fit = cm.find(corr_index);
    if (fit == cm.end())
      continue;
    this->remove((*fit).second);
    cm.erase(fit);
  }
  video_corrs_.erase(corr_index);
  //redisplay
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

  //find the closest correspondence in position and time (not in current frame)
  unsigned cur_frame =  video_istr_->frame_number();
  double min_dist = vnl_numeric_traits<double>::maxval;
  unsigned corr_index = 0;
  unsigned frame = 0;
  bool found_closest = false;
  vcl_map<unsigned, bwm_video_corr_sptr>::iterator cit = video_corrs_.begin();
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
  //now find the associated soview and select it
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

//Create a new video correspondence from the corr currently set
//on bwm_observer_vgui. Add to the list of video correspondences for
//*this* observer
void bwm_observer_video::add_video_corr()
{
  if (!video_istr_)
    return;
  vgl_point_2d<double> pt;
  bool valid = this->corr_image_pt(pt);
  if (!valid)
    return;
  unsigned frame_index = video_istr_->frame_number();
  bwm_video_corr_sptr corr = new bwm_video_corr(this, frame_index, pt);
  video_corrs_[corr->id()] = corr;
  this->remove_corr_pt();
  this->display_video_corr(corr, frame_index, CORR_STYLE);
}


void bwm_observer_video::add_match()
{
  if (!video_istr_) return;
  unsigned cur_frame =  video_istr_->frame_number();
  vgl_point_2d<double> pt;
  if (!tracked_corr_){
  unsigned frame = 0, corr_index = 0;
  bwm_soview2D_cross* cross =0;
  if (!this->find_selected_video_corr(frame, corr_index, cross))
  {
    //no corr exists or no previous corr selected
   this->add_video_corr();
   return;
  }
  // if the correspondence is in *this* frame then the match already exists
  // and there is nothing to do
  if (frame == cur_frame)
    return;

  bool valid = this->corr_image_pt(pt);
  if (!valid)
    return;
  this->remove_corr_pt();
  bwm_video_corr_sptr c = video_corrs_[corr_index];
  if (!c) return;
  //if the frame match already exists do nothing
  if (!c->add(cur_frame, pt))
    return;
  //display the new match
  this->display_video_corr(c, cur_frame, CORR_STYLE);
  //display the fact that the match to the prevous frame soview is successful
  this->remove(cross);
  corr_soview_map_[frame].erase(corr_index);
  this->display_video_corr(c, frame, MATCHED_STYLE);
  return;
  }
  bool valid = this->corr_image_pt(pt);
  if (!valid)
    return;
  this->remove_corr_pt();
  //if the frame match already exists do nothing
  if (!tracked_corr_->add(cur_frame, pt))
    return;
  //display the new match
  this->display_video_corr(tracked_corr_, cur_frame, CORR_STYLE);
}

bool bwm_observer_video::is_displayed(bwm_video_corr_sptr const& corr,
                                      unsigned frame_index)
{
  if (!corr)
    return false;
  //find the correspondence in the display map
  vcl_map<unsigned, bwm_soview2D_cross*>::iterator fit;
  fit = corr_soview_map_[frame_index].find(corr->id());
  if (fit == corr_soview_map_[frame_index].end())
    return false;
  return true;
}

//display a single video correspondence
void bwm_observer_video::display_video_corr(bwm_video_corr_sptr const& corr,
                                            unsigned frame_index,
                                            vgui_style_sptr const& style)
{
  //if there is no match for the specified frame, do nothing
  vgl_point_2d<double> pt;
  if (!corr||!corr->match(frame_index, pt))
    return;
  //if already in the display, do nothing
  if (this->is_displayed(corr, frame_index))
    return;
  // add to the display
  bwm_soview2D_cross* cross = new bwm_soview2D_cross(pt.x(), pt.y(), 2.0f);
  cross->set_style(style);
  // if the first corr for frame_index must add a map
  vcl_map<unsigned, vcl_map<unsigned, bwm_soview2D_cross*> >::iterator fit;
  fit = corr_soview_map_.find(frame_index);
  if (fit == corr_soview_map_.end())
    corr_soview_map_[frame_index] = vcl_map<unsigned, bwm_soview2D_cross*>();
  corr_soview_map_[frame_index][corr->id()]=cross;
  //skip over vgui_easy2D, since that tableau overrides the style setting
  vgui_displaylist2D_tableau::add(cross);
  this->post_redraw();
}

void bwm_observer_video::
display_projected_3d_point(bwm_video_corr_sptr const& corr)
{
  vgui_soview2D_point* pview = world_pt_map_[corr->id()];
  if (pview) this->remove(pview);
  vgl_point_3d<double> world_pt = corr->world_pt();
  vpgl_perspective_camera<double>* cam = cam_istr_->current_camera();
  vgl_point_2d<double> image_pt = cam->project(world_pt);

  pview = new vgui_soview2D_point(image_pt.x(), image_pt.y());
  pview->set_style(POINT_3D_STYLE);
  vgui_displaylist2D_tableau::add(pview);
  world_pt_map_[corr->id()]=pview;
}

//display the current set of accepted video correspondences for the current
//frame
void bwm_observer_video::display_video_corrs(unsigned frame_index)
{
  unsigned cur_frame =  video_istr_->frame_number();
  vgui_style_sptr style = CORR_STYLE;
  if (frame_index!=cur_frame){
    style = PREV_STYLE;
  }
  vcl_map<unsigned, bwm_video_corr_sptr>::iterator cit = video_corrs_.begin();
  for (;cit != video_corrs_.end(); ++cit)
  {
    bwm_video_corr_sptr c = (*cit).second;
    this->display_video_corr(c, frame_index, style);
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
  //find the nearest previous frame that has a correspondence match
  if (!video_corrs_.size()) return;
  //display the closest match to the current frame
  vcl_map<unsigned, bwm_video_corr_sptr>::iterator cit = video_corrs_.begin();
  for (; cit != video_corrs_.end(); ++cit)
  {
    bwm_video_corr_sptr c = (*cit).second;
    unsigned nframe=0;
    if (!c->nearest_frame(cur_frame, nframe))
      continue;
    this->display_video_corr(c, nframe, PREV_STYLE);
    if (nframe!=cur_frame)
      this->display_video_corr(c, cur_frame, CORR_STYLE);

    if (cam_istr_&&c->world_pt_valid()&&display_world_pts_)
      this->display_projected_3d_point(c);
  }
}


//clear the entire display map
void bwm_observer_video::clear_display_map()
{
  vcl_map<unsigned, vcl_map<unsigned, bwm_soview2D_cross*> >::iterator fit =
    corr_soview_map_.begin();
  for (; fit != corr_soview_map_.end(); ++fit)
    (*fit).second.clear();
  corr_soview_map_.clear();
}


// clear everything (only temporary for now)
void bwm_observer_video::clear_video_corrs_display()
{
  this->remove_corr_pt();
  this->clear_display_map();
  this->world_pt_map_.clear();
  this->clear();
}

vcl_vector<bwm_video_corr_sptr> bwm_observer_video::corrs()
{
  vcl_vector<bwm_video_corr_sptr> temp;
  vcl_map<unsigned, bwm_video_corr_sptr>::iterator cit = video_corrs_.begin();
  for (; cit != video_corrs_.end(); ++cit)
    temp.push_back((*cit).second);
  return temp;
}

void
bwm_observer_video::set_corrs(vcl_vector<bwm_video_corr_sptr> const& corrs)
{
  for (vcl_vector<bwm_video_corr_sptr>::const_iterator cit = corrs.begin();
       cit != corrs.end(); ++cit)
    video_corrs_[(*cit)->id()] = (*cit);
}
