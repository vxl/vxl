// This is brl/bpro/core/sdet_pro/processes/sdet_segment_image_process.cxx
#include "sdet_segment_image_process.h"
//:
// \file

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <sdet/sdet_graph_img_seg.h>

bool sdet_segment_image_process_cons(bprb_func_process& pro)
{
  using namespace sdet_segment_image_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "int";  // margin around the image, pass 0 if not needed
  input_types_[2] = "int";  // neighborhood, 4 or 8
  input_types_[3] = "float";  // weight threshold, depends on dissimilarity metric,
  input_types_[4] = "float";  // smoothing sigma, in pixels, usually 1
  input_types_[5] = "int";  // min segment size, e.g. 50
  if (!pro.set_input_types(input_types_))
    return false;

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

bool sdet_segment_image_process(bprb_func_process& pro)
{
 using namespace sdet_segment_image_process_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get inputs
  // image
  vil_image_view_base_sptr input_image_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  int margin = pro.get_input<int>(1);
  int neigh = pro.get_input<int>(2);
  auto thres = pro.get_input<float>(3);
  auto sigma = pro.get_input<float>(4);
  int min_size = pro.get_input<int>(5);

  //check input validity
  if (!input_image_sptr) {
    std::cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }

  vil_image_view<vil_rgb<vxl_byte> > out_img;

  if (input_image_sptr->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {

    std::cout << "pixel format is BYTE, using weight_threshold: " << thres << std::endl;
    vil_image_view<vxl_byte> input_image(input_image_sptr);
    sdet_segment_img<vxl_byte>(input_image, margin, neigh, thres, sigma, min_size, out_img);

  } else if (input_image_sptr->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {

    std::cout << "pixel format is FLOAT (assumes image is in [0,1]), using weight_threshold: " << thres << std::endl;
    vil_image_view<float> input_image(input_image_sptr);

    //sdet_segment_img<float>(input_image, 10, 8, 0.1, 1, 50, out_img);
    sdet_segment_img<float>(input_image, margin, neigh, thres, sigma, min_size, out_img);
  }
  // return the output edge image
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vil_rgb<vxl_byte> >(out_img));

  return true;
}

bool sdet_segment_image_using_edge_map_process_cons(bprb_func_process& pro)
{
  using namespace sdet_segment_image_using_edge_map_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vil_image_view_base_sptr";  // edge image, float in [0,1]
  input_types_[2] = "int";  // margin around the image, pass 0 if not needed
  input_types_[3] = "int";  // neighborhood, 4 or 8
  input_types_[4] = "float";  // weight threshold, depends on dissimilarity metric,
  input_types_[5] = "float";  // smoothing sigma, in pixels, usually 1
  input_types_[6] = "int";  // min segment size, e.g. 50
  if (!pro.set_input_types(input_types_))
    return false;

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

bool sdet_segment_image_using_edge_map_process(bprb_func_process& pro)
{
 using namespace sdet_segment_image_using_edge_map_process_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get inputs
  // image
  vil_image_view_base_sptr input_image_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vil_image_view_base_sptr input_edge_image_sptr = pro.get_input<vil_image_view_base_sptr>(1);
  int margin = pro.get_input<int>(2);
  int neigh = pro.get_input<int>(3);
  auto thres = pro.get_input<float>(4);
  auto sigma = pro.get_input<float>(5);
  int min_size = pro.get_input<int>(6);

  //check input validity
  if (!input_image_sptr) {
    std::cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }

  vil_image_view<float> edge_img(input_edge_image_sptr);

  vil_image_view<vil_rgb<vxl_byte> > out_img;

  if (input_image_sptr->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {

    std::cout << "pixel format is BYTE, using weight_threshold: " << thres << std::endl;
    vil_image_view<vxl_byte> input_image(input_image_sptr);
    sdet_segment_img_using_edges<vxl_byte>(input_image, edge_img, margin, neigh, thres, sigma, min_size, out_img);

  } else if (input_image_sptr->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {

    std::cout << "pixel format is FLOAT (assumes image is in [0,1]), using weight_threshold: " << thres << std::endl;
    vil_image_view<float> input_image(input_image_sptr);

    //sdet_segment_img<float>(input_image, 10, 8, 0.1, 1, 50, out_img);
    sdet_segment_img_using_edges<float>(input_image, edge_img, margin, neigh, thres, sigma, min_size, out_img);
  }
  // return the output edge image
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vil_rgb<vxl_byte> >(out_img));

  return true;
}


bool sdet_segment_image_using_height_map_process_cons(bprb_func_process& pro)
{
  using namespace sdet_segment_image_using_height_map_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vil_image_view_base_sptr";  // height image, vxl_byte, will normalize to [0,1]
  input_types_[2] = "int";  // margin around the image, pass 0 if not needed
  input_types_[3] = "int";  // neighborhood, 4 or 8
  input_types_[4] = "float";  // weight threshold, depends on dissimilarity metric,
  input_types_[5] = "float";  // smoothing sigma, in pixels, usually 1
  input_types_[6] = "int";  // min segment size, e.g. 50
  if (!pro.set_input_types(input_types_))
    return false;

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

bool sdet_segment_image_using_height_map_process(bprb_func_process& pro)
{
 using namespace sdet_segment_image_using_height_map_process_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get inputs
  // image
  vil_image_view_base_sptr input_image_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vil_image_view_base_sptr input_height_image_sptr = pro.get_input<vil_image_view_base_sptr>(1);
  int margin = pro.get_input<int>(2);
  int neigh = pro.get_input<int>(3);
  auto thres = pro.get_input<float>(4);
  auto sigma = pro.get_input<float>(5);
  int min_size = pro.get_input<int>(6);

  //check input validity
  if (!input_image_sptr) {
    std::cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }

  std::cout << "!!!!!!!!!!! input height img ni: " << input_height_image_sptr->ni() << "  nj: " << input_height_image_sptr->nj() << std::endl;
  vil_image_view<float> height_image = vil_convert_cast(float(), input_height_image_sptr);

  vil_image_view<vil_rgb<vxl_byte> > out_img;

  if (input_image_sptr->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    std::cerr << "Input image needs to be float and normalized to [0,1]\n";
    return false;

  } else if (input_image_sptr->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {

    std::cout << "pixel format is FLOAT (assumes image is in [0,1]), using weight_threshold: " << thres << std::endl;
    vil_image_view<float> input_image(input_image_sptr);

    //sdet_segment_img<float>(input_image, 10, 8, 0.1, 1, 50, out_img);
    sdet_segment_img2(input_image, height_image, margin, neigh, thres, sigma, sigma, min_size, out_img);
  }
  // return the output edge image
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vil_rgb<vxl_byte> >(out_img));

  return true;
}


bool sdet_segment_image_using_height_map_process2_cons(bprb_func_process& pro)
{
  using namespace sdet_segment_image_using_height_map_process2_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vil_image_view_base_sptr";  // height image, vxl_byte, will normalize to [0,1]
  input_types_[2] = "vil_image_view_base_sptr";  // edge image, float image in [0,1]
  input_types_[3] = "int";  // margin around the image, pass 0 if not needed
  input_types_[4] = "int";  // neighborhood, 4 or 8
  input_types_[5] = "float";  // weight threshold, depends on dissimilarity metric,
  input_types_[6] = "float";  // smoothing sigma, in pixels, usually 1
  input_types_[7] = "int";  // min segment size, e.g. 50
  if (!pro.set_input_types(input_types_))
    return false;

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

bool sdet_segment_image_using_height_map_process2(bprb_func_process& pro)
{
 using namespace sdet_segment_image_using_height_map_process2_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get inputs
  // image
  vil_image_view_base_sptr input_image_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vil_image_view_base_sptr input_height_image_sptr = pro.get_input<vil_image_view_base_sptr>(1);
  vil_image_view_base_sptr input_edge_image_sptr = pro.get_input<vil_image_view_base_sptr>(2);
  int margin = pro.get_input<int>(3);
  int neigh = pro.get_input<int>(4);
  auto thres = pro.get_input<float>(5);
  auto sigma = pro.get_input<float>(6);
  int min_size = pro.get_input<int>(7);

  //check input validity
  if (!input_image_sptr) {
    std::cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }

  std::cout << "!!!!!!!!!!! input height img ni: " << input_height_image_sptr->ni() << "  nj: " << input_height_image_sptr->nj() << std::endl;
  vil_image_view<float> height_image = vil_convert_cast(float(), input_height_image_sptr);

  vil_image_view<float> edge_img(input_edge_image_sptr);
  vil_math_truncate_range(edge_img, 0.05f, 1.0f);
  float min_h, max_h;
  vil_math_value_range(edge_img, min_h, max_h);
  std::cout << "edge image value range, min: " << min_h << " max: " << max_h << " normalizing to [0,1]!\n";
  vil_math_scale_and_offset_values(edge_img, 1.0,-min_h);
  vil_math_scale_and_offset_values(edge_img, 1.0/(max_h-min_h),0);

  vil_image_view<vil_rgb<vxl_byte> > out_img;

  if (input_image_sptr->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    std::cerr << "Input image needs to be float and normalized to [0,1]\n";
    return false;

  } else if (input_image_sptr->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {

    std::cout << "pixel format is FLOAT (assumes image is in [0,1]), using weight_threshold: " << thres << std::endl;
    vil_image_view<float> input_image(input_image_sptr);

    sdet_segment_img2_using_edges(input_image, height_image, edge_img, margin, neigh, thres, sigma, 0.0f, min_size, out_img);
  }
  // return the output edge image
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vil_rgb<vxl_byte> >(out_img));

  return true;
}
