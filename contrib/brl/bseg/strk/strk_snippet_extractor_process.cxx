// This is brl/bseg/strk/strk_snippet_extractor_process.cxx
#include "strk_snippet_extractor_process.h"
#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vidl_vil1/vidl_vil1_movie.h>
#include <vidl_vil1/vidl_vil1_clip.h>
#include <vidl_vil1/vidl_vil1_io.h>
#include <vsol/vsol_box_2d.h>
#include <vtol/vtol_face_2d.h>
#include <bsol/bsol_algs.h>
#include <brip/brip_roi.h>
#include <brip/brip_vil1_float_ops.h>
#include <strk/strk_io.h>
strk_snippet_extractor_process::strk_snippet_extractor_process()
{
  failure_ = false;
  first_frame_ = true;
  margin_frac_ = 0.1;
  track_file_ = "";
  snippet_directory_= "";
}

strk_snippet_extractor_process::~strk_snippet_extractor_process()
{
}

bool strk_snippet_extractor_process::execute()
{
  if (failure_)
    return false;
  if (this->get_N_input_images()!=1)
  {
    vcl_cout << "In strk_snippet_extractor_process::execute() -"
             << " not exactly one input image\n";
    failure_ = true;
    return false;
  }
  static unsigned int n_frames = 0;
  vil1_image img = vpro_video_process::get_input_image(0);
  input_images_.clear();
  if (first_frame_)
  {
    n_frames = tracked_rois_.size();
    unsigned int w = img.width(), h = img.height();
    for(unsigned int i=0; i<n_frames; ++i)
      tracked_rois_[i]->set_image_bounds(w, h);
    first_frame_ = false;
  }
  int frame_index = this->frame_index();
  int offset = frame_index-start_frame_;
  if (offset<0||offset>=n_frames)
    {
      failure_ = true;      
      return false;
    }
  brip_vil1_float_ops::chip(img, tracked_rois_[offset], output_image_);
  tracked_snippets_.push_back(output_image_);
  return true;
}

bool strk_snippet_extractor_process::
extract_rois(vcl_vector<vtol_face_2d_sptr> const & tracked_faces)
{
  for(vcl_vector<vtol_face_2d_sptr>::const_iterator fit = tracked_faces.begin();fit != tracked_faces.end(); ++fit)
    {
      vsol_box_2d_sptr bb = (*fit)->get_bounding_box(), expanded_box;
      double diameter = (bb->width()+bb->height())/2.0;

      if(!bsol_algs::box_with_margin(bb, margin_frac_*diameter, expanded_box))
		  return false;
      brip_roi_sptr roi = new brip_roi();
      roi->add_region(expanded_box);
      tracked_rois_.push_back(roi);
    }
  return true;
}

bool strk_snippet_extractor_process::
set_input_file(vcl_string const& track_file_name)
{
  start_frame_ = 0;
  track_file_ = track_file_name;
  if (track_file_=="")
    return false;
  vcl_ifstream str(track_file_.c_str());
  if (!str)
  {
    vcl_cout << "In strk_snippet_extractor_process::set_input_file() -"
             << " could not open file " << track_file_ << '\n';
    return false;
  }
  unsigned int n_frames = 0;
  vcl_vector<vgl_point_2d<double> > cogs;
  vcl_vector<vtol_face_2d_sptr> tracked_faces;
  if(!strk_io::read_track_data(str, start_frame_, n_frames,
                           cogs, tracked_faces))
    {
      str.close();
      return false;
    }
  end_frame_ = start_frame_+n_frames -1;
  str.close();
  if(!extract_rois(tracked_faces))
    return false;
  return true;
}
void 
strk_snippet_extractor_process::set_snippet_directory(vcl_string const& snip_dir)
{
  snippet_directory_ = snip_dir;
}
// write out the collected snippets.
bool strk_snippet_extractor_process::finish()
{
  if (!tracked_snippets_.size())
    return false;
  vidl_vil1_clip_sptr clip = new vidl_vil1_clip(tracked_snippets_);
  vidl_vil1_movie_sptr mov= new vidl_vil1_movie();
  mov->add_clip(clip);
  if (!vidl_vil1_io::save(mov.ptr(), snippet_directory_.c_str(), "tiff")){
    vcl_cout << "In strk_snippet_extractor_process::finish()"
             << " - failed to save video" << vcl_endl;
    return false;
  }
  tracked_snippets_.clear();
  return true;
}
