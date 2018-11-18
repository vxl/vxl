// This is brl/bpro/core/vil_pro/processes/vil_EO_IR_combine_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>

//: Constructor
bool vil_EO_IR_combine_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types(2);
  input_types[0] = "vil_image_view_base_sptr"; //color image
  input_types[1] = "vil_image_view_base_sptr"; //IR image
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //this process has one output
  std::vector<std::string> output_types(1);
  output_types[0] = "vil_image_view_base_sptr"; // 3- channel image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_EO_IR_combine_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()) {
    std::cout << "vil_EO_IR_combine_process: bad inputs" << std::endl;
    return false;
  }

  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr img_eo = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr img_ir = pro.get_input<vil_image_view_base_sptr>(i++);

  if (img_eo->nplanes() < 3 || img_ir->nplanes() !=1 ||
      img_eo->ni() != img_ir->ni() || img_eo->nj() !=img_ir->nj())
  {
    std::cout<<"# of planes  "<<img_eo->nplanes()<<' '<<img_ir->nplanes()<<'\n'
            <<"Error in the input image"<<std::endl;
    return false;
  }
  vil_image_view_base_sptr n_planes = vil_convert_to_n_planes(4, img_eo);
  vil_image_view_base_sptr comp_image = vil_convert_to_component_order(n_planes);
  auto* eo_rgba_view = new vil_image_view<vil_rgba<vxl_byte> >(comp_image);

  auto* ir_view    = dynamic_cast<vil_image_view<vxl_byte>* >(img_ir.ptr());
  auto *out_img=new vil_image_view<vil_rgb<vxl_byte> >(img_ir->ni(),img_ir->nj());


    //make sure all alpha values are set to 255 (1)
  vil_image_view<vil_rgba<vxl_byte> >::iterator eo_iter= eo_rgba_view->begin();
  vil_image_view<vxl_byte >::iterator ir_iter=ir_view->begin();
  vil_image_view<vil_rgb<vxl_byte> >::iterator out_iter= out_img->begin();
  for (; eo_iter != eo_rgba_view->end(); ++eo_iter,++ir_iter,++out_iter) {
    (*out_iter) = vil_rgb<vxl_byte>((unsigned char)(0.75*(float)eo_iter->R()+0.25*(float)(*ir_iter)),eo_iter->G(),  (unsigned char)std::fabs(0.75*(float)eo_iter->G()-0.25*(float)(*ir_iter)));
  }

  vil_image_view_base_sptr out_img_ptr=out_img;

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_ptr);
  return true;
}
