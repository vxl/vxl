//This is brl/bpro/core/sdet_pro/processes/sdet_detect_edge_tangent_interp_process.cxx
#include "sdet_detect_edge_tangent_interp_process.h"
//:
// \file

#include <sdet/sdet_img_edge.h>

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <sdet/sdet_detector.h>
#include <sdet/sdet_fit_lines.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

//: initialize input and output types
bool sdet_detect_edge_tangent_interp_process_cons(bprb_func_process& pro)
{
  using namespace sdet_detect_edge_tangent_interp_process_globals;
  // process takes 2 inputs
  //input[0]: input grayscale image
  //input[1]: string indicating the output format
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vcl_string";

  // process has 1 output image with 3 bands:
  // output[0]: output edge image with 3 planes
  // case pos_dir
  // plane 0 - sub-pixel column position of the edge.
  //           Negative value indicates no edge is present
  // plane 1 - sub-pixel row position of the edge.
  //           Negative value indicates no edge is present
  // plane 2 - Orientation of local edge tangent direction in radians
  // range is [0, 2pi).
  // case line_2d
  // plane 0 - line coefficient a --
  //                                |-- components of line normal vector
  // plane 1 - line coefficient b --
  //
  // plane 2 - line coefficient c

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  return pro.set_input_types(input_types_)
      && pro.set_output_types(output_types_);
}

//: generates the edge map
bool sdet_detect_edge_tangent_interp_process(bprb_func_process& pro)
{
 using namespace sdet_detect_edge_tangent_interp_process_globals;

  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " Invalid inputs" << std::endl;
    return false;
  }

  // get inputs
  // image
  vil_image_view_base_sptr input_image_sptr = pro.get_input<vil_image_view_base_sptr>(0);

  //check input validity
  if (!input_image_sptr) {
    std::cout << pro.name() <<" :-- null input image\n";
    return false;
  }

  vil_image_view<vxl_byte> input_image =
    *vil_convert_cast(vxl_byte(), input_image_sptr);

  std::string out_type = pro.get_input<std::string>(1);
  // get parameters
  double noise_multiplier=1.5, smooth=1.5;
  bool automatic_threshold=false, junctionp=false, aggressive_junction_closure=false;

  pro.parameters()->get_value(param_noise_multiplier_, noise_multiplier);
  pro.parameters()->get_value(param_smooth_, smooth);
  pro.parameters()->get_value(param_automatic_threshold_, automatic_threshold);
  pro.parameters()->get_value(param_junctionp_, junctionp);
  pro.parameters()->get_value(param_aggressive_junction_closure_, aggressive_junction_closure);
#if 0
  std::cout << "Edge detection parameters\n";
  pro.parameters()->print_all(std::cout);
#endif
  vil_image_view<float> edge_image =
    sdet_img_edge::detect_edge_tangent_interpolated(input_image,
                                                    noise_multiplier,
                                                    smooth,
                                                    automatic_threshold,
                                                    junctionp,
                                                    aggressive_junction_closure);

  // return the output edge image in pos_dir format
  if (out_type=="pos_dir") {
    pro.set_output_val<vil_image_view_base_sptr>(0,new vil_image_view<float>(edge_image));
    return true;
  }
  //else convert to line format
  if (out_type == "line_2d") {
    vil_image_view<float> line_image(edge_image.ni(), edge_image.nj(), 3);
    sdet_img_edge::convert_edge_image_to_line_image(edge_image, line_image);
    pro.set_output_val<vil_image_view_base_sptr>(0,new vil_image_view<float>(line_image));
    return true;
  }
  return false;
}

//: a process that fits lines to detected canny edges and samples those lines to generate sub-pixel edges
bool sdet_detect_edge_line_fitted_process_cons(bprb_func_process& pro)
{
  // process takes 2 inputs:
  //input[0]: input grayscale image
  //input[1]: string indicating the output format
  std::vector<std::string> input_types_(2);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vcl_string";
  if (!pro.set_input_types(input_types_))
    return false;

  std::vector<std::string> output_types_(1);
  output_types_[0] = "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

bool sdet_detect_edge_line_fitted_process(bprb_func_process& pro)
{
  using namespace sdet_detect_edge_line_fitted_process_globals;
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " Invalid inputs" << std::endl;
    return false;
  }

  // get inputs
  // image
  vil_image_view_base_sptr input_image_sptr = pro.get_input<vil_image_view_base_sptr>(0);

  //check input validity
  if (!input_image_sptr) {
    std::cout << pro.name() <<" :-- null input image\n";
    return false;
  }

  vil_image_view<vxl_byte> input_image =
    *vil_convert_cast(vxl_byte(), input_image_sptr);

  std::string out_type = pro.get_input<std::string>(1);
  // get parameters
  double noise_multiplier=1.5, smooth=1.5;
  bool automatic_threshold=false, junctionp=false, aggressive_junction_closure=false;
  int min_fit_length=3; double rms_distance=0.1;

  pro.parameters()->get_value(param_noise_multiplier_, noise_multiplier);
  pro.parameters()->get_value(param_smooth_, smooth);
  pro.parameters()->get_value(param_automatic_threshold_, automatic_threshold);
  pro.parameters()->get_value(param_junctionp_, junctionp);
  pro.parameters()->get_value(param_aggressive_junction_closure_, aggressive_junction_closure);
  pro.parameters()->get_value(param_min_fit_length_, min_fit_length);
  pro.parameters()->get_value(param_rms_distance_, rms_distance);

  vil_image_view<float> edge_image =
    sdet_img_edge::detect_edge_line_fitted(input_image,
                                           noise_multiplier,
                                           smooth,
                                           automatic_threshold,
                                           junctionp,
                                           aggressive_junction_closure, min_fit_length, rms_distance);

  // return the output edge image in pos_dir format
  if (out_type=="pos_dir") {
    pro.set_output_val<vil_image_view_base_sptr>(0,new vil_image_view<float>(edge_image));
    return true;
  }
  //else convert to line format
  if (out_type == "line_2d") {
    vil_image_view<float> line_image(edge_image.ni(), edge_image.nj(), 3);
    sdet_img_edge::convert_edge_image_to_line_image(edge_image, line_image);
    pro.set_output_val<vil_image_view_base_sptr>(0,new vil_image_view<float>(line_image));
    return true;
  }
  return false;
}


//: a helper process that prints out subpixel edge_image into a text file
bool sdet_write_edge_file_process_cons(bprb_func_process& pro)
{
  // process has 1 input image with 3 bands:
  // output[0]: output edge image with 3 planes
  // case pos_dir
  // plane 0 - sub-pixel column position of the edge.
  //           Negative value indicates no edge is present
  // plane 1 - sub-pixel row position of the edge.
  //           Negative value indicates no edge is present
  // plane 2 - Orientation of local edge tangent direction in radians
  // range is [0, 2pi).

  std::vector<std::string> input_types_(2);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vcl_string";  // name of output file, extention should be .edg
  if (!pro.set_input_types(input_types_))
    return false;

  std::vector<std::string> output_types_(0);
  return pro.set_output_types(output_types_);
}

bool sdet_write_edge_file_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " Invalid inputs" << std::endl;
    return false;
  }

  // get inputs
  // image
  vil_image_view_base_sptr input_image_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  std::string filename = pro.get_input<std::string>(1);

  //check input validity
  if (!input_image_sptr || input_image_sptr->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
    std::cout << pro.name() <<" :-- null input image or wrong image type\n";
    return false;
  }

  vil_image_view<float> edge_image(input_image_sptr);
  unsigned ni = edge_image.ni();
  unsigned nj = edge_image.nj();

  unsigned cnt = 0;
  //: make a pass to find edge count
  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 0; j < nj; j++) {
      if (edge_image(i,j) < 0)
        continue;
      cnt++;
    }
  }

  //1) If file open fails, return.
  std::ofstream outfp(filename.c_str(), std::ios::out);

  if (!outfp){
    std::cout << " Error opening file  " << filename.c_str() << std::endl;
    return false;
  }

  //2) write out the header block
  outfp << "# EDGE_MAP v3.0\n\n"
        << "# Format :  [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Strength Uncer\n\n"
        << "WIDTH=" << ni << '\n'
        << "HEIGHT=" << nj << '\n'
        << "EDGE_COUNT=" << cnt  << '\n' << std::endl;

  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 0; j < nj; j++) {
      if (edge_image(i,j) < 0)
        continue;

        double x = edge_image(i,j,0);
        double y = edge_image(i,j,1);

        unsigned ix = i;//col; //preserve the original pixel assignment
        unsigned iy = j;//row;

        double idir = edge_image(i,j,2), iconf = 0.0;
        double dir= edge_image(i,j,2), conf= 0.0, uncer=0.0;

        outfp << "[" << ix << ", " << iy << "]    " << idir << " " << iconf << "   [" << x << ", " << y << "]   " << dir << " " << conf << " " << uncer << std::endl;
    }
  }

  outfp.close();

  return true;
}
