// This is brl/bseg/strk/strk_info_tracker_process.cxx
#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vnl/vnl_matlab_print2.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_face_2d.h>
#include <vil1/vil1_image.h>
#include <strk/strk_tracker.h>
#include <strk/strk_tracking_face_2d.h>
#include <strk/strk_info_tracker_process.h>

strk_info_tracker_process::strk_info_tracker_process(strk_info_tracker_params & tp)
  : tracker_(tp)
{
  n_verts_ = 0;
  start_frame_ = 0;
  write_tracked_faces_=false;
  write_tracked_hist_=false;
  failure_ = false;
  first_frame_ = true;
  //make the color index for display |intensity|gradient|color
  unsigned int ibins = tracker_.intensity_hist_bins_;
  unsigned int gbins = tracker_.gradient_dir_hist_bins_;
  unsigned int cbins = tracker_.color_hist_bins_;
  unsigned int nbins = ibins + gbins + cbins;
  color_index_.resize(nbins);
  for(unsigned int c = 0;c<nbins; c++)
    if(c<ibins)
      color_index_[c]=0;
    else
      if(c>=ibins&&c<(ibins+gbins))
        color_index_[c]=1;
      else
        color_index_[c]=2;
}

strk_info_tracker_process::~strk_info_tracker_process()
{
  tracker_.clear();
}

bool strk_info_tracker_process::execute()
{
  if (failure_)
    {
      vcl_cout << "In strk_info_tracker_process::execute() - process failed\n";
      return false;
    }
  if (this->get_N_input_images()!=1)
    {
      vcl_cout << "In strk_info_tracker_process::execute() -"
               << " not exactly one input image\n";
      failure_ = true;
      return false;
    }
  output_topo_objs_.clear();

  vil1_image img = vpro_video_process::get_input_image(0);
  input_images_.clear();
  if (first_frame_)
    {
      tracker_.set_image_0(img);
      int nto = get_N_input_topo_objs();
      if (!nto)
        {
          vcl_cout << "In strk_info_tracker_process::execute() -"
                   << " no input correlation face\n";
          failure_ = true;
          return false;
        }
      vtol_topology_object_sptr to = input_topo_objs_[0];
      vtol_face_sptr f = to->cast_to_face();
      vtol_face_2d_sptr f2d = f->cast_to_face_2d();
      if (!f2d)
        {
          vcl_cout << "In strk_info_tracker_process::execute() -"
                   << " input is not a vtol_face_2d\n";
          failure_ = true;
          return false;
        }
      vcl_vector<vtol_vertex_sptr> verts;
      f2d->vertices(verts);
      n_verts_ = verts.size();
      start_frame_ = this->frame_index();
      tracked_faces_.clear();
      tracked_hist_.clear();
      tracked_faces_.push_back(f2d);
      tracker_.set_initial_model(f2d);
      tracker_.init();
      if(tracker_.use_background_)
        if(!tracker_.construct_background_faces(f2d, true))
          {
            vcl_cout << "Warning - In strk_info_tracker_process::execute() -"
                     << " could not construct background faces\n";
            failure_ = true;
            return false;
          }
      vcl_vector<vtol_edge_sptr> edges_2d;
      f2d->edges(edges_2d);
      for (vcl_vector<vtol_edge_sptr>::iterator eit = edges_2d.begin();
           eit != edges_2d.end(); eit++)
        {
          vtol_topology_object_sptr to = (*eit)->cast_to_edge();
          output_topo_objs_.push_back(to);
        }
      first_frame_ = false;
      return true;
    }

  tracker_.set_image_i(img);
  tracker_.track();
#if 0
  vcl_vector<vtol_face_2d_sptr> samples;
  tracker_.get_samples(samples);
  for (vcl_vector<vtol_face_2d_sptr>::iterator fit = samples.begin();
       fit != samples.end(); fit++)
    {
      vtol_topology_object_sptr to =
        (vtol_topology_object*)((*fit)->cast_to_face());
      output_topo_objs_.push_back(to);
    }
#endif
#if 1
  //output interior verts
  vcl_vector<vtol_topology_object_sptr> points;
  //  tracker_.get_best_face_points(points);
  for (vcl_vector<vtol_topology_object_sptr>::iterator pit = points.begin();
       pit != points.end(); ++pit)
    output_topo_objs_.push_back(*pit);
  //output face edges
  vtol_face_2d_sptr f = tracker_.get_best_sample();
  tracked_faces_.push_back(f);
  vcl_vector<vtol_edge_sptr> edges;
  f->edges(edges);
  for (vcl_vector<vtol_edge_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
    {
      vtol_topology_object_sptr to = (*eit)->cast_to_edge();
      output_topo_objs_.push_back(to);
    }
  if(tracker_.use_background_)
    {
      vcl_vector<vtol_face_2d_sptr> background_faces;
      if(tracker_.get_background_faces(background_faces))
        for(vcl_vector<vtol_face_2d_sptr>::iterator fit =
              background_faces.begin(); fit != background_faces.end(); fit++)
          {
            vtol_face_2d_sptr fb = *fit;
            vcl_vector<vtol_edge_sptr> bedges;
            fb->edges(bedges);
            for (vcl_vector<vtol_edge_sptr>::iterator eit = bedges.begin();
                 eit != bedges.end(); eit++)
              {
                vtol_topology_object_sptr to = (*eit)->cast_to_edge();
                output_topo_objs_.push_back(to);
              }
          }
    }
  //output the histograms |Intensity|gradient|color|
  vpro_video_process::set_graph(tracker_.histograms());
  tracked_hist_.push_back(tracker_.histograms());
#endif
  return true;
}
bool strk_info_tracker_process::write_track_file()
{
  int n_frames = tracked_faces_.size();
  if (!n_frames)
    return false;
  vnl_matrix<double> cog(n_frames, 2);
  vnl_matrix<double> X(n_frames, n_verts_);
  vnl_matrix<double> Y(n_frames, n_verts_);
  vcl_ofstream track_stream(track_file_.c_str());
  if (!track_stream)
    {
      vcl_cout << "In strk_info_tracker_process::finish() -"
               << " could not open file " << track_file_ << '\n';
      return false;
    }
  track_stream << "START_FRAME: " << start_frame_ << '\n'
               << "N_FRAMES: " << n_frames << '\n'
               << "N_VERTS: " << n_verts_ << '\n';
  vcl_vector<vtol_vertex_sptr> verts;
  for (int i = 0; i<n_frames; i++)
    {
      verts.clear();
      double cog_x = 0, cog_y = 0;
      tracked_faces_[i]->vertices(verts);
      for (int j = 0; j<n_verts_; j++)
        {
          vtol_vertex_2d_sptr v = verts[j]->cast_to_vertex_2d();
          if (!v)
            continue;
          cog_x += v->x(); cog_y += v->y();
          X.put(i,j,v->x());
          Y.put(i,j,v->y());
        }
      cog_x/=n_verts_; cog_y/=n_verts_;
      cog.put(i, 0, cog_x); cog.put(i, 1, cog_y);
    }
  track_stream << "COG:\n";
  vnl_matlab_print(track_stream, cog);
  track_stream << "X:\n";
  vnl_matlab_print(track_stream, X);
  track_stream << "Y:\n";
  vnl_matlab_print(track_stream, Y);
  track_stream.close();
  return true;
}
bool strk_info_tracker_process::write_hist_file()
{
  int n_frames = tracked_hist_.size();
  if (!n_frames||!tracker_.initial_tf())
    return false;
  vcl_ofstream hist_stream(hist_file_.c_str());
  if (!hist_stream)
    {
      vcl_cout << "In strk_info_tracker_process::finish() -"
               << " could not open file " << hist_file_ << '\n';
      return false;
    }
  int nint = tracker_.intensity_hist_bins_, ndir = tracker_.gradient_dir_hist_bins_;
  int ncol = tracker_.color_hist_bins_;
  int nbins = nint+ndir+ncol;
  if(nbins!=tracked_hist_[0].size())
    return false;
  strk_tracking_face_2d_sptr itf = tracker_.initial_tf();
  hist_stream << "START_FRAME: " << start_frame_ << '\n'
              << "N_FRAMES: " << n_frames << '\n'
              << "N_PIXELS: " << itf->face()->Npix() << '\n'
              << "DIAMETER: " << itf->face()->Diameter() << '\n'
              << "ASPECT_RATIO: " << itf->face()->AspectRatio() << '\n'
              << "N_INTENSITY_BINS: " << nint << '\n'
              << "N_GRADIENT_DIR_BINS: " << ndir << '\n'
              << "N_COLOR_BINS: " << ncol << '\n'
              << "HISTOGRAMS: " << '\n';
  vnl_matrix<float> temp(n_frames, nbins);
  for(unsigned int i = 0;i<n_frames; ++i)
    for(unsigned int j = 0; j<nbins; ++j)
        temp[i][j]=tracked_hist_[i][j];
    hist_stream << temp << '\n';
  hist_stream.close();
  return true;
}
bool strk_info_tracker_process::finish()
{
  if (write_tracked_faces_)
    if(!write_track_file())
      return false;
  if (write_tracked_hist_)
    if(!write_hist_file())
      return false;
  return true;
}
bool strk_info_tracker_process::set_track_output_file(vcl_string const& file_name)
{
  write_tracked_faces_ = true;
  track_file_ = file_name;
  vcl_ofstream track_stream(track_file_.c_str());
  if (!track_stream)
    {
      vcl_cout << "In strk_info_tracker_process::set_output_file() -"
               << " could not open file " << track_file_ << '\n';
      return false;
    }
  track_stream.close();
  return true;
}

bool strk_info_tracker_process::set_hist_output_file(vcl_string const& file_name)
{
  write_tracked_hist_ = true;
  hist_file_ = file_name;
  vcl_ofstream hist_stream(hist_file_.c_str());
  if (!hist_stream)
    {
      vcl_cout << "In strk_info_tracker_process::set_output_file() -"
               << " could not open file " << hist_file_ << '\n';
      return false;
    }
  hist_stream.close();
  return true;
}

 
