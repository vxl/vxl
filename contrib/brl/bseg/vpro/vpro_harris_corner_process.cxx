// This is brl/bseg/vpro/vpro_harris_corner_process.cxx
#include "vpro_harris_corner_process.h"
//: \file
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vsol/vsol_point_2d.h>
#include <vil1/vil1_memory_image_of.h>
#include <sdet/sdet_harris_detector.h>

vpro_harris_corner_process::vpro_harris_corner_process(sdet_harris_detector_params & hdp)
 : sdet_harris_detector_params(hdp)
{
}

vpro_harris_corner_process::~vpro_harris_corner_process()
{
}

bool vpro_harris_corner_process::execute()
{
  if (this->get_N_input_images() != 1)
  {
    vcl_cout << "In vpro_harris_corner_process::execute() - not exactly one input image\n";
    return false;
  }
  output_spat_objs_.clear();
  //assume the input images are grey scale (should really check)
  vil1_memory_image_of<unsigned char> img(vpro_video_process::get_input_image(0));
  vpro_video_process::clear_input();
  sdet_harris_detector harris_detector(*(static_cast<sdet_harris_detector_params*>(this)));
  harris_detector.set_image(img);
  harris_detector.extract_corners();
  vcl_vector<vsol_point_2d_sptr>& points = harris_detector.get_points();
  int N = points.size();
  if (!N)
    return false;
  for (int i = 0; i<N; i++)
    output_spat_objs_.push_back(points[i]->cast_to_spatial_object_2d());

  output_topo_objs_.clear();
  output_image_ = 0;
  return true;
}
