// This is brl/bpro/core/brip_pro/processes/brip_extrema_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <brip/brip_vil_float_ops.h>
#include <bprb/bprb_parameters.h>
#include <vil/vil_new.h>
#include <vil/vil_math.h>
#include <vil/vil_image_view_base.h>

//: Constructor
bool brip_extrema_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr"); // input image
  input_types.push_back("float");  // lambda0
  input_types.push_back("float");  // lambda1
  input_types.push_back("float");  // theta
  input_types.push_back("bool");   // bright = true, dark = false
  input_types.push_back("bool");   // use fast algorithm = true
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");  // point response
  output_types.push_back("vil_image_view_base_sptr");  // kernel domain response
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}


//: Execute the process
bool brip_extrema_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 6) {
    vcl_cout << "brip_extrema_process: The input number should be 6" << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr image_ptr = pro.get_input<vil_image_view_base_sptr>(i++);
  float lambda0 = pro.get_input<float>(i++);
  float lambda1 = pro.get_input<float>(i++);
  float theta = pro.get_input<float>(i++);
  bool bright = pro.get_input<bool>(i++);   //bright vs. dark
  bool fast = pro.get_input<bool>(i++);     //fast or composed

  //Retrieve image from input
  vil_image_resource_sptr resc = vil_new_image_resource_of_view(*image_ptr);

  // convert input image to float on range 0-1
  vil_image_view<float> fimage = brip_vil_float_ops::convert_to_float(resc);
  if (image_ptr->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(fimage,1.0/255.0);
  unsigned ni = fimage.ni(), nj = fimage.nj(), np = fimage.nplanes();
  vil_image_view<float> gimage;
  if (np>1){
    gimage.set_size(ni,nj);
    for (unsigned j = 0; j<nj; ++j)
      for (unsigned i = 0; i<ni; ++i)
      {
        float v = 0;
        for (unsigned p = 0; p<np; ++p)
          v += fimage(i,j,p);
        gimage(i,j) = v/(float)np;
      }
  }
  else
    gimage = fimage;

  vil_image_view<float> out;
  if (fast)
    out = brip_vil_float_ops::fast_extrema(gimage, lambda0, lambda1,
                                           theta, bright, false, true);
  else
    out = brip_vil_float_ops::extrema(gimage, lambda0, lambda1,
                                      theta, bright, true);
  vil_image_view<float>* point = new vil_image_view<float>(ni, nj);
  vil_image_view<float>* mask = new vil_image_view<float>(ni, nj);

  for (unsigned j = 0; j<nj; ++j) {
    for (unsigned i = 0; i<ni; ++i)
    {
      (*point)(i,j) = out(i,j,0);
      (*mask)(i,j) = out(i,j,1);
    }
  }

  i=0;
  pro.set_output_val<vil_image_view_base_sptr>(i++, point);
  pro.set_output_val<vil_image_view_base_sptr>(i++, mask);
  return true;
}

