//----*-c++-*----tells emacs to use C++ mode----------
// This is brl/vpro/vpro_info_tracker_process.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vnl/vnl_matlab_print2.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_face_2d.h>
#include <vil1/vil1_memory_image_of.h>
#include <sdet/sdet_tracker.h>
#include <vpro/vpro_info_tracker_process.h>

vpro_info_tracker_process::vpro_info_tracker_process(sdet_info_tracker_params & tp)
 : tracker_(tp) 
{
  n_verts_ = 0;
  start_frame_ = 0;
  write_tracked_faces_=false;
  failure_ = false;
  first_frame_ = true;
}

vpro_info_tracker_process::~vpro_info_tracker_process()
{
  tracker_.clear();
}

bool vpro_info_tracker_process::execute()
{
  if(failure_)
    {
      vcl_cout << "In vpro_info_tracker_process::execute() - process failed\n";
      return false;
    }
  if (this->get_N_input_images()!=1)
    {
      vcl_cout << "In vpro_info_tracker_process::execute() - not exactly one"
               << " input image \n";
      failure_ = true;
      return false;
    }
  output_topo_objs_.clear();
  //assume the input images are grey scale (should really check)
  vil1_memory_image_of<unsigned char> img(vpro_video_process::get_input_image(0));
  input_images_.clear();
  if(first_frame_)
    {
      tracker_.set_image_0(img);
      if(!get_N_input_topo_objs())
        {
          vcl_cout << "In vpro_info_tracker_process::execute() - no input"
                   << "correlation face  \n";
          failure_ = true;
          return false;
        }
      vtol_topology_object_sptr to = input_topo_objs_[0];
      vtol_face_sptr f = to->cast_to_face();
      vtol_face_2d_sptr f2d = f->cast_to_face_2d();
      if(!f2d)
        {
          vcl_cout << "In vpro_info_tracker_process::execute() - input"
                   << "is not a vtol_face_2d  \n";
          failure_ = true;
          return false;
        }
      vcl_vector<vtol_vertex_sptr> verts;
      f2d->vertices(verts);
      n_verts_ = verts.size();
      start_frame_ = this->frame_index();
      tracked_faces_.push_back(f2d);
      tracker_.set_initial_model(f2d);
      tracker_.init();
      output_topo_objs_.push_back((vtol_topology_object*)f2d->cast_to_face());
      first_frame_ = false;
      return true;
    }
  
  tracker_.set_image_i(img);
  tracker_.track();
  //  vcl_vector<vtol_face_2d_sptr> samples;
  //  tracker_.get_samples(samples);
  //   for(vcl_vector<vtol_face_2d_sptr>::iterator fit = samples.begin();
  //       fit != samples.end(); fit++)
  //     {
  //       vtol_topology_object_sptr to = 
  //         (vtol_topology_object*)((*fit)->cast_to_face());
  //       output_topo_objs_.push_back(to);
  //     }
  vtol_face_2d_sptr f = tracker_.get_best_sample();
  tracked_faces_.push_back(f);
  vtol_topology_object_sptr to = 
    (vtol_topology_object*)(f->cast_to_face());
  output_topo_objs_.push_back(to);
  return true;
}

bool vpro_info_tracker_process::finish()
{
  if(write_tracked_faces_)
    {
      int n_frames = tracked_faces_.size();
      if(!n_frames)
        return false;
      vnl_matrix<double> X(n_frames, n_verts_);
      vnl_matrix<double> Y(n_frames, n_verts_);
      vcl_ofstream track_stream(track_file_.c_str());
      if(!track_stream.is_open())
        {
          vcl_cout << "In vpro_info_tracker_process::finish() -"
                   << " could not open file " << track_file_ << "\n";
          return false;
        }
      track_stream << "START_FRAME: " << start_frame_ << "\n";
      track_stream << "N_FRAMES: " << n_frames << "\n";
      track_stream << "N_VERTS: " << n_verts_ << "\n";
      vcl_vector<vtol_vertex_sptr> verts;
      for(int i = 0; i<n_frames; i++)
        {
          verts.clear();
          tracked_faces_[i]->vertices(verts);
          for(int j = 0; j<n_verts_; j++)
            {
              vtol_vertex_2d_sptr v = verts[j]->cast_to_vertex_2d();
              if(!v)
                continue;
              X.put(i,j,v->x());
              Y.put(i,j,v->y());
            }
        }
      track_stream << "X: \n";
      vnl_matlab_print(track_stream, X);
      track_stream << "Y: \n";
      vnl_matlab_print(track_stream, Y);
      track_stream.close();
    }
  return true;
}
          
bool vpro_info_tracker_process::set_output_file(vcl_string const& file_name)
{
  write_tracked_faces_ = true;
  track_file_ = file_name;
  vcl_ofstream track_stream(track_file_.c_str());
  if(!track_stream.is_open())
    {
      vcl_cout << "In vpro_info_tracker_process::set_output_file() -"
               << " could not open file " << track_file_ << "\n";
      return false;
    }
  track_stream.close();
  return true;
}

