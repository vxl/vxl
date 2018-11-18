// This is brl/bpro/core/vil_pro/processes/vil_gaussian_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_gauss_filter.h>
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view_base.h>

//: Constructor
bool vil_gaussian_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr"); // input image
  input_types.emplace_back("float"); // gaussian standard deviation
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr"); // gauss smoothed image(float)
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_gaussian_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    std::cout << "vil_gaussian_process: The input number should be 5" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr input_image = pro.get_input<vil_image_view_base_sptr>(i++);

  //Gaussian standard deviation
  auto sigma = pro.get_input<float>(i++);
  auto half_width = static_cast<unsigned>(3.0f*sigma);

  // convert input image to float
  vil_image_view<float> fimage = *vil_convert_cast(float(), input_image);
  if (input_image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(fimage,1.0/255.0);
  if (input_image->pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
    vil_math_scale_values(fimage,1.0/65536);
  unsigned ni = fimage.ni(), nj = fimage.nj(), np = fimage.nplanes();

  vil_image_view<float> G(ni, nj, np);
  for (unsigned p = 0; p<np; ++p)
  {
    //extract plane
    vil_image_view<float> temp(ni, nj), g(ni,nj);
    for (unsigned j = 0; j<nj; ++j)
      for (unsigned i = 0; i<ni; ++i)
        temp(i,j)=static_cast<float>(fimage(i,j,p));

    vil_gauss_filter_2d(temp, g, sigma, half_width);
    for (unsigned j = 0; j<nj; ++j)
      for (unsigned i = 0; i<ni; ++i)
        G(i,j,p) = g(i,j);
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(G));
  return true;
}
