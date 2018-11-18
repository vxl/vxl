// This is brl/bpro/core/vil_pro/processes/vil_load_from_ascii_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>

#include <vnl/vnl_matrix.h>

//: Constructor
// this process takes 2 inputs:
// * input(0): ascii filename
// * input(1): image output filename
//
// this process has no outputs
// this process does not save the image because it uses vil_image_view::set_from_memory
// where the data goes out of scope the image becomes invalid
bool vil_load_from_ascii_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  input_types.emplace_back("vcl_string");
  return pro.set_input_types(input_types);
}


//: Execute the process
bool vil_load_from_ascii_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    std::cout << "vil_load_from_ascii_process: The number of inputs should be 2" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //Retrieve image from input
  std::string ascii_file = pro.get_input<std::string>(i++);
  std::string img_file = pro.get_input<std::string>(i++);

  //read in ascii file
  std::ifstream ifs(ascii_file.c_str());

  vnl_matrix<float> M;
  ifs >> M;

  //create image from memory
  vil_image_view<float> img(M.cols(),M.rows());

  img.set_to_memory(M.data_block(),M.cols(),M.rows(),
                    1,1,M.cols(),M.rows()*M.cols());

  vil_save(img, img_file.c_str());

  return true;
}
