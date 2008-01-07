#include "vpro_region_process.h"
//:
// \file
#include <vcl_iostream.h>
#include <vil1/vil1_memory_image_of.h>
#include <vcl_vector.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <vtol/vtol_intensity_face.h>
#include <sdet/sdet_region_proc.h>

vpro_region_process::vpro_region_process(sdet_detector_params & dp)
  : sdet_detector_params(dp)
{
}

vpro_region_process::~vpro_region_process()
{
}

bool vpro_region_process::execute()
{
  if (this->get_N_input_images() != 1)
  {
    vcl_cout << "In vpro_region_process::execute() - not exactly one input image\n";
    return false;
  }
  output_topo_objs_.clear();
  //assume the input images are grey scale (should really check)
  vil1_memory_image_of<unsigned char> img(vpro_video_process::get_input_image(0));

  sdet_region_proc_params rpp((*(sdet_detector_params*)this), true, false, 2);
  sdet_region_proc rp(rpp);
  rp.set_image(img);
  rp.extract_regions();
  vcl_vector<vtol_intensity_face_sptr>& regions = rp.get_regions();
  for (vcl_vector<vtol_intensity_face_sptr>::iterator fit = regions.begin();
       fit != regions.end(); fit++)
    output_topo_objs_.push_back((*fit)->cast_to_topology_object());
  output_image_ = 0;
  return true;
}
