#include <vcl_iostream.h>
#include <vil/vil_memory_image_of.h>
#include <vcl_vector.h>
#include <vdgl/vdgl_intensity_face_sptr.h>
#include <vdgl/vdgl_intensity_face.h>
#include <sdet/sdet_region_proc.h>
#include <vvid/vvid_region_process.h>

vvid_region_process::vvid_region_process(sdet_detector_params & dp)
  : sdet_detector_params(dp)
{
}

vvid_region_process::~vvid_region_process()
{
}

bool vvid_region_process::execute()
{
  if (this->get_N_inputs()!=1)
    {
      vcl_cout << "In vvid_region_process::execute() - not exactly one"
               << " input image \n";
      return false;
    }
  topo_objs_.clear();
  //assume the input images are grey scale (should really check)
  vil_memory_image_of<unsigned char> img(vvid_video_process::get_input_image(0));

  sdet_region_proc_params rpp((*(sdet_detector_params*)this), true, false, 2);
  sdet_region_proc rp(rpp);
  rp.set_image(img);
  rp.extract_regions();
  vcl_vector<vdgl_intensity_face_sptr>& regions = rp.get_regions();
  for (vcl_vector<vdgl_intensity_face_sptr>::iterator fit = regions.begin();
       fit != regions.end(); fit++)
    topo_objs_.push_back((*fit)->cast_to_topology_object());
  output_image_ = 0;
  return true;
}
