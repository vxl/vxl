// This is brl/bpro/core/vil_pro/processes/vil_two_planes_composite_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>

//: Constructor
bool vil_two_planes_composite_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types(4);
  input_types[0] = "vil_image_view_base_sptr"; //base image
  input_types[1] = "vil_image_view_base_sptr"; //colorizing image
  input_types[2] = "bool"; //use complement
  input_types[3] = "float"; //scale factor
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //this process has one output
  std::vector<std::string> output_types(1);
  output_types[0] = "vil_image_view_base_sptr";
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_two_planes_composite_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()) {
    std::cout << "vil_two_planes_composite_process: bad inputs" << std::endl;
    return false;
  }

  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr img_1 = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr img_2 = pro.get_input<vil_image_view_base_sptr>(i++);
  bool comp = pro.get_input<bool>(2);
  auto scale = pro.get_input<float>(3);

  // convert input images to float
  vil_image_view<float> fimage1 = *vil_convert_cast(float(), img_1);
  if (img_1->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(fimage1,1.0/255.0);

  vil_image_view<float> fimage2 = *vil_convert_cast(float(), img_2);
  if (img_2->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(fimage2,1.0/255.0);

#if 0
  vil_image_view<vxl_byte> bimage1 = vil_convert_cast(vxl_byte(), img_1);
  vil_image_view<vxl_byte> bimage2 = vil_convert_cast(vxl_byte(), img_2);
#endif
  if (!(img_1->ni()==img_2->ni() && img_1->nj()==img_2->nj()))
  {
    std::cout<<"The images have different dimensions"<<std::endl;
    return false;
  }

  auto *out_img=new vil_image_view<float>(img_1->ni(),img_1->nj(),3);
  unsigned nplanes1 = img_1->nplanes(), nplanes2 = img_2->nplanes();
  if ((nplanes1!=1 && nplanes1!=3) ||
      (nplanes2!=1 && nplanes2!=3))
    return false;

  for (unsigned i=0;i<img_1->ni();i++)
    for (unsigned j=0;j<img_1->nj();j++)
    {
      if (nplanes1==1&&nplanes2==1) {
        float v = fimage2(i,j);
        if (comp) v = 1.0f-v;
        (*out_img)(i,j,0)=fimage1(i,j);
        (*out_img)(i,j,1)=scale*v;
        (*out_img)(i,j,2)=0;
      }
      if (nplanes1==1&&nplanes2==3) {
        float v = fimage1(i,j);
        if (comp) v = 1.0f-v;
        (*out_img)(i,j,0)=static_cast<float>((fimage2(i,j,0)+ scale*v)/2.0);
        (*out_img)(i,j,1)=fimage2(i,j,1);
        (*out_img)(i,j,2)=fimage2(i,j,2);
      }
      if (nplanes1==3&&nplanes2==1) {
        float v = fimage2(i,j);
        if (comp) v = 1.0f-v;
        (*out_img)(i,j,0)=static_cast<float>((scale*v+ fimage1(i,j,0))/2.0);
        (*out_img)(i,j,1)=fimage1(i,j,1);
        (*out_img)(i,j,2)=fimage1(i,j,2);
      }
      if (nplanes1==3&&nplanes2==3) {
        float v0 = fimage2(i,j,0), v1 = fimage2(i,j,1), v2 = fimage2(i,j,2);
        if (comp) { v0 = 1.0f-v0; v1 = 1.0f-v1; v2 = 1.0f-v2; }
        (*out_img)(i,j,0)=static_cast<float>((scale*v0+ fimage1(i,j,0))/2.0);
        (*out_img)(i,j,1)=static_cast<float>((scale*v1+ fimage1(i,j,1))/2.0);
        (*out_img)(i,j,2)=static_cast<float>((scale*v2+ fimage1(i,j,2))/2.0);
      }
    }

  vil_image_view_base_sptr out_img_ptr=out_img;

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_ptr);
  return true;
}
