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
  //this process takes two inputs:
  // input(0): first  vil_image_view_base_sptr
  // input(1): second image
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");
  input_types.push_back("vil_image_view_base_sptr");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //this process has no outputs
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_two_planes_composite_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    vcl_cout << "vil_two_planes_composite_process: The input number should be 2" << vcl_endl;
    return false;
  }

  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr img_1 = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr img_2 = pro.get_input<vil_image_view_base_sptr>(i++);

  vil_image_view<vxl_byte> bimage1 = vil_convert_cast(vxl_byte(), img_1);
  vil_image_view<vxl_byte> bimage2 = vil_convert_cast(vxl_byte(), img_2);

  if (!(img_1->ni()==img_2->ni() && img_1->nj()==img_2->nj()))
  {
    vcl_cout<<"The images have different dimensions"<<vcl_endl;
    return false;
  }
  vil_image_view<unsigned char> *out_img=new vil_image_view<unsigned char>(img_1->ni(),img_1->nj(),3);

  for (unsigned i=0;i<img_1->ni();i++)
  {
    for (unsigned j=0;j<img_1->nj();j++)
    {
      (*out_img)(i,j,0)=bimage1(i,j);
      (*out_img)(i,j,1)=bimage2(i,j);
      (*out_img)(i,j,2)=0.0;
    }
  }

  vil_image_view_base_sptr out_img_ptr=out_img;

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_ptr);
  return true;
}

