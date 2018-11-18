// This is brl/bpro/core/vil_pro/processes/vil_gradient_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view_base.h>

//: Constructor
bool vil_gradient_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr"); // input image
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr"); // dI/dx
  output_types.emplace_back("vil_image_view_base_sptr"); // dI/dy
  output_types.emplace_back("vil_image_view_base_sptr"); // Sqrt((dI/dx)^2 + (dI/dy)^2)
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_gradient_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    std::cout << "vil_gradient_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr input_image = pro.get_input<vil_image_view_base_sptr>(i++);

  // convert image to float
  vil_image_view<float> fimage = *vil_convert_cast(float(), input_image);
  if (input_image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(fimage,1.0/255.0);
  unsigned ni = fimage.ni(), nj = fimage.nj(), np = fimage.nplanes();
  vil_image_view<float> Ix(ni, nj, np);
  vil_image_view<float> Iy(ni, nj, np);
  vil_image_view<float> mag(ni, nj, np);
  for (unsigned p = 0; p<np; ++p)
  {
    //extract plane
    vil_image_view<float> temp(ni, nj), gx(ni, nj), gy(ni,nj);
    for (unsigned j = 0; j<nj; ++j)
      for (unsigned i = 0; i<ni; ++i)
        temp(i,j)=static_cast<float>(fimage(i,j,p));

    // vil_sobel_3x3(temp, gx, gy);
    float scale = 1.0f/6.0f;
    for (unsigned int j = 0; j<nj; ++j)
      for (unsigned int i = 0; i<ni; ++i) {
        // boundary: make sure to never access i-1 or j-1 when i==0 or j==0,
        //           nor access i+1 or j+1 when i==ni-1 or j==nj-1:
        if (i==0||j==0||i+1==ni||j+1==nj){
          Ix(i,j,p) = 0.0f; Iy(i,j,p) = 0.0f; mag(i,j,p) = 0.0f;
          continue;
        }
        // at this point, unsigned i-1, i+1, j-1, and j+1 may be used safely:

        float gx = temp(i+1,j-1) +temp(i+1,j) +temp(i+1,j-1)
                  -temp(i-1,j-1) -temp(i-1,j) -temp(i-1,j-1);

        float gy = temp(i+1,j+1) +temp(i,j+1) +temp(i-1,j+1)
                  -temp(i+1,j-1) -temp(i,j-1) -temp(i-1,j-1);

        Ix(i,j,p) = gx*scale;
        Iy(i,j,p) = gy*scale;
        mag(i,j,p) = std::sqrt(gx*gx+gy*gy)*scale;
      }
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(Ix));
  pro.set_output_val<vil_image_view_base_sptr>(1, new vil_image_view<float>(Iy));
  pro.set_output_val<vil_image_view_base_sptr>(2, new vil_image_view<float>(mag));

  return true;
}
