#include <vcl_iostream.h>
#include <vvid/vvid_video_process.h>

vvid_video_process::vvid_video_process()
{
}

vvid_video_process::~vvid_video_process()
{
}

vil_image  vvid_video_process::get_input_image(int i)
{
  if (i<0||i>(int)input_images_.size())
    {
      vcl_cout << "In vvid_video_process::get_input_image(..) - index out of"
               << " bounds \n";
      return vil_image();
    }
  return input_images_[i];
}

vcl_vector<vtol_topology_object_sptr> const & 
vvid_video_process::get_segmentation()
{
  return topo_objs_;
}

vcl_vector<vsol_spatial_object_2d_sptr> const & 
vvid_video_process::get_spatial_objects()
{
  return spat_objs_;
}
