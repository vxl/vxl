//:
// \file
// \brief Example of loading an image and applying gradient filter
// \author Tim Cootes

#include <iostream>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vxl_config.h" // for vxl_byte
#include "vil/vil_load.h"
#include "vil/vil_save.h"
#include "vil/vil_image_view.h"
#include <vil/algo/vil_exp_grad_filter_1d.h>
#include "vil/vil_convert.h"

int
main(int argc, char ** argv)
{
  if (argc != 3)
  {
    std::cout << "vil_exp_grad_filter_tool  src_image x_grad_image\n"
              << "Applies exponential gradient filter to src_image, saves to x_grad_image\n";
    return 0;
  }

  vil_image_view<vxl_byte> src_im = vil_load(argv[1]);
  if (src_im.size() == 0)
  {
    std::cout << "Unable to load source image from " << argv[1] << std::endl;
    return 1;
  }

  double k = 0.80; // Kernel smoothing parameter

  vil_image_view<float> grad_x;

  vil_exp_grad_filter_i(src_im, grad_x, double(k));

  // Stretch range to [0,255]
  float min_v, max_v;
  vil_math_value_range(grad_x, min_v, max_v);
  double s = 255 / (max_v - min_v);
  vil_math_scale_and_offset_values(grad_x, s, -s * min_v);
  std::cout << "Range of result: " << min_v << ',' << max_v << std::endl;

  vil_image_view<vxl_byte> grad_x_byte;
  vil_convert_cast(grad_x, grad_x_byte);

  std::cout << "Resulting image is " << grad_x_byte.ni() << " x " << grad_x_byte.nj() << std::endl;

  if (!vil_save(grad_x_byte, argv[2]))
  {
    std::cerr << "Unable to save result image to " << argv[2] << std::endl;
    return 1;
  }

  std::cout << "Saved x-gradient to " << argv[2] << std::endl;

  return 0;
}
