#include <vcl_iostream.h>
#include <vvid/vvid_video_process.h>

vvid_video_process::vvid_video_process()
{
}

vvid_video_process::~vvid_video_process()
{
}

vil_image vvid_video_process::get_input_image(unsigned int i) const
{
  if (i>input_images_.size())
    {
      vcl_cout << "In vvid_video_process::get_input_image(..) - index out of"
               << " bounds\n";
      return vil_image();
    }
  return input_images_[i];
}

vcl_vector<vtol_topology_object_sptr> const & 
vvid_video_process::get_output_topology()
{
  return output_topo_objs_;
}

vcl_vector<vsol_spatial_object_2d_sptr> const & 
vvid_video_process::get_output_spatial_objects()
{
  return output_spat_objs_;
}

void vvid_video_process::clear_input()
{
  input_images_.clear();
  input_spat_objs_.clear();
  input_topo_objs_.clear();
}

void vvid_video_process::clear_output()
{
  //output_images_.clear();
  output_spat_objs_.clear();
  output_topo_objs_.clear();
}

void vvid_video_process::add_input_spatial_objects(vcl_vector<vsol_spatial_object_2d_sptr> const& spat_objs)
{
  for (vcl_vector<vsol_spatial_object_2d_sptr>::const_iterator sit = spat_objs.begin(); sit != spat_objs.end(); sit++)
    input_spat_objs_.push_back(*sit);
}

void vvid_video_process::add_input_topology(vcl_vector<vtol_topology_object_sptr> const& topo_objs)
{
  for (vcl_vector<vtol_topology_object_sptr>::const_iterator toit = topo_objs.begin(); toit != topo_objs.end(); toit++)
    input_topo_objs_.push_back(*toit);
}
