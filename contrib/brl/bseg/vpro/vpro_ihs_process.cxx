// This is brl/bseg/vpro/vpro_ihs_process.cxx
#include "vpro_ihs_process.h"
#include <vcl_iostream.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>
#include <brip/brip_vil1_float_ops.h>

vpro_ihs_process::vpro_ihs_process()
{
  failure_ = false;
}

vpro_ihs_process::~vpro_ihs_process()
{
}


bool vpro_ihs_process::execute()
{
  if (failure_)
    return false;
  if (this->get_N_input_images()!=1)
  {
    vcl_cout << "In vpro_ihs_process::execute() -"
             << " not exactly one input image\n";
    failure_ = true;
    return false;
  }
  vil1_memory_image_of<vil1_rgb<unsigned char> > in_image(this->get_input_image(0));
  vil1_memory_image_of<float> I,H,S;
  vil1_memory_image_of<vil1_rgb<unsigned char> > image;
  brip_vil1_float_ops::convert_to_IHS(in_image, I, H, S);
  brip_vil1_float_ops::display_IHS_as_RGB(I, H, S, image);
  output_image_=image;
  input_images_.clear();
  return true;
}

