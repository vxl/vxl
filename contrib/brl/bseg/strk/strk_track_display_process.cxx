// This is brl/bseg/strk/strk_track_display_process.cxx
#include "strk_track_display_process.h"
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_face_2d.h>
#include <strk/strk_io.h>
strk_track_display_process::strk_track_display_process()
{
  failure_ = false;
  first_frame_ = true;
  start_frame_ = 0;
}

strk_track_display_process::~strk_track_display_process()
{
}

bool strk_track_display_process::execute()
{
  if (failure_)
    return false;
  if (this->get_N_input_images()!=1)
  {
    vcl_cout << "In strk_track_display_process::execute() -"
             << " not exactly one input image\n";
    failure_ = true;
    return false;
  }
  output_topo_objs_.clear();
  input_images_.clear();
  static int n_faces = 0;
  if (first_frame_)
  {
    first_frame_ = false;
    n_faces = tracked_faces_.size();
    if (!n_faces)
    {
      vcl_cout << "In strk_track_display_process::execute() -"
               << " no faces found in track file\n";
      failure_ = true;
    }
  }
  int frame_index = this->frame_index();
  int offset = frame_index-start_frame_;
  if (offset<0||offset>=n_faces)
    return false;
  vtol_face_2d_sptr f = tracked_faces_[offset];
  vcl_vector<vtol_edge_sptr> edges;
  f->edges(edges);
  //cog
  vgl_point_2d<double> p = tracked_cogs_[offset];
  vtol_topology_object_sptr to = new vtol_vertex_2d(p.x(), p.y());
  output_topo_objs_.push_back(to);
  //edges
  for (vcl_vector<vtol_edge_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
  {
    to = (*eit)->cast_to_edge();
    output_topo_objs_.push_back(to);
  }
  return true;
}

bool strk_track_display_process::set_input_file(vcl_string const& file_name)
{
  start_frame_ = 0;
  track_file_ = file_name;
  if (track_file_=="")
    return false;
  vcl_ifstream str(track_file_.c_str());
  if (!str)
  {
    vcl_cout << "In strk_track_display_process::set_input_file() -"
             << " could not open file " << track_file_ << '\n';
    return false;
  }
  unsigned int n_frames = 0;
  unsigned int n_verts =0;
  if(!strk_io::read_track_data(str, start_frame_, n_frames,
                           tracked_cogs_, tracked_faces_))
    {
      str.close();
      return false;
    }
  end_frame_ = start_frame_+n_frames -1;
  str.close();
  return true;
}
