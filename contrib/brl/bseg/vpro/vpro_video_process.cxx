#include "vpro_video_process.h"
#include <vcl_iostream.h>
vpro_video_process::vpro_video_process()
{
  frame_index_ = 0;
  n_frames_ = 0;
  start_frame_ =0;
  end_frame_ = 0;
  graph_flag_ = false;
}
vil1_image vpro_video_process::get_input_image(unsigned int i)
{
  if (i > input_images_.size())
  {
    vcl_cout << "In vpro_video_process::get_input_image(..) - index "
             << i << " out of bounds\n";
    return vil1_image();
  }
  return input_images_[i];
}

void vpro_video_process::clear_input()
{
  input_images_.clear();
  input_spat_objs_.clear();
  input_topo_objs_.clear();
}

void vpro_video_process::clear_output()
{
  //output_images_.clear();
  output_spat_objs_.clear();
  output_topo_objs_.clear();
}

void vpro_video_process::add_input_spatial_object(vsol_spatial_object_2d_sptr const& so)
{
  input_spat_objs_.push_back(so);
}

void vpro_video_process::add_input_spatial_objects(vcl_vector<vsol_spatial_object_2d_sptr> const& spat_objs)
{
  for (vcl_vector<vsol_spatial_object_2d_sptr>::const_iterator sit = spat_objs.begin(); sit != spat_objs.end(); sit++)
    input_spat_objs_.push_back(*sit);
}

void vpro_video_process::add_input_topology_object(vtol_topology_object_sptr const& to)
{
  input_topo_objs_.push_back(to);
}

void vpro_video_process::add_input_topology(vcl_vector<vtol_topology_object_sptr> const& topo_objs)
{
  for (vcl_vector<vtol_topology_object_sptr>::const_iterator toit = topo_objs.begin(); toit != topo_objs.end(); toit++)
    input_topo_objs_.push_back(*toit);
}
