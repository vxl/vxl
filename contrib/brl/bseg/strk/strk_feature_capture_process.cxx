// This is brl/bseg/strk/strk_feature_capture_process.cxx
#include "strk_feature_capture_process.h"
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_face_2d.h>
#include <strk/strk_io.h>
#include <strk/strk_tracking_face_2d.h>
strk_feature_capture_process::strk_feature_capture_process(strk_info_tracker_params & tp)
  : tracker_(tp)
{
  failure_ = false;
  first_frame_ = true;
  face_index_ =0;
}

strk_feature_capture_process::~strk_feature_capture_process()
{
}

bool strk_feature_capture_process::execute()
{
  if (failure_)
    return false;
  if (this->get_N_input_images()!=1)
    {
      vcl_cout << "In strk_feature_capture_process::execute() -"
               << " not exactly one input image\n";
      failure_ = true;
      return false;
    }
  vil1_image img = vpro_video_process::get_input_image(0);
  input_images_.clear();
  static int n_faces = 0;
  if (first_frame_)
    {
      face_index_ = 0;
      tracker_.set_image_0(img);
      n_faces = tracked_faces_.size();
      if (!n_faces)
        {
          vcl_cout << "In strk_feature_capture_process::execute() -"
                   << " no faces found in track file\n";
          failure_ = true;
        }
      vtol_face_2d_sptr f = tracked_faces_[face_index_];
      tracker_.set_initial_model(f);
      tracker_.init();
      first_frame_ = false;
      tracked_hist_.push_back(tracker_.capture_histograms(true));
      return true;
    }
  face_index_++;
  tracker_.set_image_i(img);
  vtol_face_2d_sptr f = tracked_faces_[face_index_];
  tracker_.set_capture_face(f);
  tracked_hist_.push_back(tracker_.capture_histograms());
  vcl_vector<vtol_edge_sptr> edges;
  f->edges(edges);
  output_topo_objs_.clear();
  for (vcl_vector<vtol_edge_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
  {
    vtol_topology_object_sptr to = (*eit)->cast_to_edge();
    output_topo_objs_.push_back(to);
  }
  return true;
}

bool strk_feature_capture_process::finish()
{
  first_frame_ = true;
  face_index_ = 0;
  failure_ = false;
  vcl_ofstream strm(hist_file_.c_str());
  if (!strm)
  {
    vcl_cout << "In strk_feature_capture_process::set_input_file() -"
             << " could not open file " << hist_file_ << '\n';
    return false;
  }
  strk_tracking_face_2d_sptr tf = tracker_.capture_tf();
  if(!tf)
    return false;
  int n_pix = tf->face()->Npix();
  float dia = tf->face()->Diameter();
  float r = tf->face()->AspectRatio();
  vcl_vector<vcl_vector<float> > junk;
  if(!strk_io::write_histogram_data(start_frame_, n_pix, dia, r, 
                                    tracker_.intensity_hist_bins_,
                                    tracker_.gradient_dir_hist_bins_,
                                    tracker_.color_hist_bins_,
                                    tracked_hist_,
                                    strm))
    return false;
  return true;
}

bool strk_feature_capture_process::set_input_file(vcl_string const& file_name)
{
  start_frame_ = 0;
  track_file_ = file_name;
  if (track_file_=="")
    return false;
  vcl_ifstream str(track_file_.c_str());
  if (!str)
    {
      vcl_cout << "In strk_feature_capture_process::set_input_file() -"
               << " could not open file " << track_file_ << '\n';
      return false;
    }
  vcl_vector<vgl_point_2d<double> > tracked_cogs;
  unsigned int n_frames = 0;
  if(!strk_io::read_track_data(str, start_frame_, n_frames,
                               tracked_cogs, tracked_faces_))
    {
      str.close();
      return false;
    }
  end_frame_ = start_frame_+n_frames -1;
  str.close();
  return true;
}

bool strk_feature_capture_process::set_output_file(vcl_string const& file_name)
{
  hist_file_ = file_name;
  return true;
}
