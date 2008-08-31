// This is brl/bpro/core/vil_pro/vil_load_from_ascii_process.cxx
#include "vil_load_from_ascii_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>

#include <vnl/vnl_matrix.h>

//: Constructor
vil_load_from_ascii_process::vil_load_from_ascii_process()
{
  //this process takes 1 input:
  // input(0): ascii filename
  // input(1): image output filename
  input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0]="vcl_string";
  input_types_[1]="vcl_string";

  //this process has no outputs
  //this process does not save the image because it uses, vil_image_view::set_from_memory
  //where the data goes out of scope the image becomes invalid
  output_data_.resize(0,brdb_value_sptr(0));
  output_types_.resize(0);
}

//: Destructor
vil_load_from_ascii_process::~vil_load_from_ascii_process()
{
}


//: Execute the process
bool
vil_load_from_ascii_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //Retrieve image from input
  brdb_value_t<vcl_string>* input0 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[0].ptr());

  vcl_string ascii_file = input0->value();

  //Retrieve image from input
  brdb_value_t<vcl_string>* input1 =
    static_cast<brdb_value_t<vcl_string>* >(input_data_[1].ptr());

  vcl_string img_file = input1->value();

  //read in ascii file
  vcl_ifstream ifs(ascii_file.c_str());

  vnl_matrix<float> M;
  ifs >> M;

  //create image from memory
  vil_image_view<float> img(M.cols(),M.rows());

  img.set_to_memory(M.data_block(),M.cols(),M.rows(),
    1,1,M.cols(),M.rows()*M.cols());

  vil_save(img, img_file.c_str());

  return true;
}

