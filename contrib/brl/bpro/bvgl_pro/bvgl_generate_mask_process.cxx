// This is brl/bpro/core/vil_pro/bvgl_generate_mask_process.cxx

//:
// \file


#include <vil/vil_math.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vcl_iostream.h>
#include "bvgl_generate_mask_process.h"
#include <bvgl/bvgl_changes.h>
#include <bvgl/bvgl_changes_sptr.h>

//: Constructor
bvgl_generate_mask_process::bvgl_generate_mask_process()
{
  //input  
  input_data_.resize(3,brdb_value_sptr(0));
  input_types_.resize(3);
  input_types_[0]= "vcl_string"; //name of the binary file to read bvgl_changes object
  input_types_[1]= "unsigned"; // ni for output image
  input_types_[2]= "unsigned"; // nj for output image
  
  //output 
  output_data_.resize(2, brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0]= "vil_image_view_base_sptr"; // vxl_byte mask (with don't care areas)
  output_types_[1]= "vil_image_view_base_sptr"; // bool mask (with changes true and other areas false)
}


//: Destructor
bvgl_generate_mask_process::~bvgl_generate_mask_process()
{
}


//: Execute the process
bool
bvgl_generate_mask_process::execute()
{
  // Sanity check
  if(!this->verify_inputs()){
    vcl_cerr << "In bvgl_generate_mask_process::execute() -"
             << " invalid inputs\n";
    return false;
  }
  
  brdb_value_t<vcl_string>* input0 = 
    static_cast<brdb_value_t<vcl_string>* >(input_data_[0].ptr());
  vcl_string file_name = input0->value();

  brdb_value_t<unsigned>* input1 = 
    static_cast<brdb_value_t<unsigned>* >(input_data_[1].ptr());
  unsigned ni = input1->value();

  brdb_value_t<unsigned>* input2 = 
    static_cast<brdb_value_t<unsigned>* >(input_data_[2].ptr());
  unsigned nj = input2->value();

  //vil_image_view<bool> result(;
  //: read the object
  vsl_b_ifstream bif(file_name);
  bvgl_changes_sptr objs = new bvgl_changes();
  objs->b_read(bif);
  bif.close();

  vil_image_view_base_sptr out = objs->create_mask_from_objs(ni, nj);

  vil_image_view<vxl_byte> out_view(out);
  vil_image_view<bool> out_b(ni, nj, 1);
  out_b.fill(false);

  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++)
      out_b(i,j) = (out_view(i,j) == 255 ? true : false);

  
  brdb_value_sptr output0 = 
    new brdb_value_t<vil_image_view_base_sptr>(out);
  output_data_[0] = output0;

  brdb_value_sptr output1 = 
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<bool>(out_b));
  output_data_[1] = output1;

  return true;
}




