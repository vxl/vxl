//----*-c++-*----tells emacs to use C++ mode----------
// This is brl/vpro/vpro_track_display_process.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_face_2d.h>
#include <vpro/vpro_track_display_process.h>

vpro_track_display_process::vpro_track_display_process()
{
  failure_ = false;
  first_frame_ = true;
}

vpro_track_display_process::~vpro_track_display_process()
{
}

bool vpro_track_display_process::input_tracked_faces(vcl_ifstream & str)
{
  int n_frames = 0;
  int n_verts =0;
  vcl_string sf, nf, nv;
  str >> sf;
  if(sf!="START_FRAME:")
    return false;
  str >> start_frame_;
  str >> nf;
  if(nf!="N_FRAMES:")
    return false;
  str >> n_frames;
  str >> nv;
  if(nv!="N_VERTS:")
    return false;
  str >> n_verts;
  vnl_matrix<double> X(n_frames, n_verts);
  vnl_matrix<double> Y(n_frames, n_verts);
  vcl_string x, y;
  str >> x;
  if(x!="X:")
    return false;
  str >> X;
  str >> y;
  if(y!="Y:")
    return false;
  str >> Y;
  vcl_vector<vtol_vertex_sptr> verts;
  for(int i = 0; i<n_frames; i++)
    {
      verts.clear();
      for(int j = 0; j<n_verts; j++)
        {
          vtol_vertex_2d* v2d  = new vtol_vertex_2d(X[i][j], Y[i][j]);
          verts.push_back((vtol_vertex*)v2d);
        }
      tracked_faces_.push_back(new vtol_face_2d(verts));
    }
  return true;
}
bool vpro_track_display_process::execute()
{
  if(failure_)
    return false;
  if (this->get_N_input_images()!=1)
    {
      vcl_cout << "In vpro_track_display_process::execute() - not exactly one"
               << " input image \n";
      failure_ = true;
      return false;
    }
  output_topo_objs_.clear();
  input_images_.clear();
  
  if(first_frame_)
    {
      first_frame_ = false;

      vcl_ifstream str = vcl_ifstream(track_file_.c_str());
      if(!str.is_open())
        {
          vcl_cout << "In vpro_track_display_process::execute() -"
                   << " could not open file " << track_file_ << "\n";
          failure_ = true;
          return false;
        }
        
      this->input_tracked_faces(str);
      if(!tracked_faces_.size())
        {
          vcl_cout << "In vpro_track_display_process::execute() -"
                   << " no faces found in track file\n";
          failure_ = true;
        }
    }
  int frame_index = this->frame_index();
  int offset = frame_index-start_frame_;
  if(offset<0)
    return true;
  vtol_face_2d_sptr f = tracked_faces_[offset];
  vtol_topology_object_sptr to = 
    (vtol_topology_object*)(f->cast_to_face());
  output_topo_objs_.push_back(to);
  return true;
}
          
bool vpro_track_display_process::set_input_file(vcl_string const& file_name)
{
  track_file_ = file_name;
  vcl_ifstream str = vcl_ifstream(track_file_.c_str());
  if(!str.is_open())
    {
      vcl_cout << "In vpro_track_display_process::set_input_file() -"
               << " could not open file " << track_file_ << "\n";
      return false;
    }
  str.close();
  return true;
}
