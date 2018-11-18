// This is brl/bpro/core/bvgl_pro/processes/bvgl_generate_mask_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
//:
// \file

#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bvgl/bvgl_changes.h>
#include <bvgl/bvgl_changes_sptr.h>
#define NI "ni"
#define NJ "nj"

// Constructor
bool bvgl_generate_mask_process_cons(bprb_func_process& pro)
{
  //input
  std::vector<std::string> input_types, output_types;
  input_types.resize(2);
  input_types[0]= "vcl_string"; //name of the binary file to read bvgl_changes object
  input_types[1]= "vcl_string"; //type of change
  if (pro.set_input_types(input_types)) {
    //output
    output_types.resize(2);
    output_types[0]= "vil_image_view_base_sptr"; // vxl_byte mask (with don't care areas)
    output_types[1]= "vil_image_view_base_sptr"; // bool mask (with changes true and other areas false)
    return pro.set_output_types(output_types);
  } else
    return false;
}


// Execute the process
bool
bvgl_generate_mask_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "In bvgl_generate_mask_process::execute() - invalid inputs\n";
    return false;
  }

  std::string file_name = pro.get_input<std::string>(0);
  std::string change_type = pro.get_input<std::string>(1);

  unsigned ni=0, nj=0; // dummy initialisation, to avoid compiler warning
  if (!pro.parameters()->get_value(NI, ni) ||
      !pro.parameters()->get_value(NJ, nj))
  {
      std::cerr << "In bvgl_generate_mask_process - failed to get parameters\n";
      return false;
  }

  // read the object
  vsl_b_ifstream bif(file_name);
  bvgl_changes_sptr objs = new bvgl_changes();
  objs->b_read(bif);
  bif.close();
  std::cout << "In bvgl_generate_mask_process(), the size of the change objects is: " << objs->size() << std::endl;

  vil_image_view_base_sptr out = objs->create_mask_from_objs(ni, nj, change_type);

  vil_image_view<vxl_byte> out_view(out);
  vil_image_view<bool> out_b(ni, nj, 1);
  out_b.fill(false);

  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++)
      out_b(i,j) = (out_view(i,j) == 255 ? true : false);


  pro.set_output_val<vil_image_view_base_sptr>(0, out);
  pro.set_output_val<vil_image_view_base_sptr>(1, new vil_image_view<bool>(out_b));

  return true;
}

// Constructor
bool bvgl_set_change_type_process_cons(bprb_func_process& pro)
{
  //input
  std::vector<std::string> input_types, output_types;
  input_types.resize(3);
  input_types[0]= "vcl_string"; //name of the binary file to read bvgl_changes object
  input_types[1]= "vcl_string"; //type of change
  input_types[2]= "vcl_string"; //name of the output binary file to write new bvgl_changes object
  if (pro.set_input_types(input_types)) {
    //output
    output_types.resize(0);
    return pro.set_output_types(output_types);
  } else
    return false;
}


// Execute the process
bool bvgl_set_change_type_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "In bvgl_set_change_type_mask_process::execute() - invalid inputs\n";
    return false;
  }

  std::string file_name = pro.get_input<std::string>(0);
  std::string change_type = pro.get_input<std::string>(1);
  std::string output_file_name = pro.get_input<std::string>(2);

  // read the object
  vsl_b_ifstream bif(file_name);
  bvgl_changes_sptr objs = new bvgl_changes();
  objs->b_read(bif);
  bif.close();
  std::cout << "In bvgl_set_change_type_mask_process(), the size of the change objects is: " << objs->size() << std::endl;

  bvgl_changes_sptr objs_new = new bvgl_changes();
  for (unsigned int i = 0; i < objs->size(); i++) {
    bvgl_change_obj_sptr obj = objs->obj(i);
    bvgl_change_obj_sptr new_obj = new bvgl_change_obj(obj->poly(), change_type);
    objs_new->add_obj(new_obj);
  }
  std::cout << "In bvgl_set_change_type_mask_process(), the size of the output change objects is: " << objs_new->size() << std::endl;

  vsl_b_ofstream bof(output_file_name);
  objs_new->b_write(bof);
  bof.close();
  return true;
}


bool bvgl_generate_mask_process2_cons(bprb_func_process& pro)
{
  //input
  std::vector<std::string> input_types, output_types;
  input_types.resize(4);
  input_types[0]= "vcl_string"; //name of the binary file to read bvgl_changes object
  input_types[1]= "vcl_string"; //type of change
  input_types[2]= "unsigned"; // ni for output image
  input_types[3]= "unsigned"; // nj for output image
  if (pro.set_input_types(input_types)) {
    //output
    output_types.resize(2);
    output_types[0]= "vil_image_view_base_sptr"; // vxl_byte mask (with don't care areas)
    output_types[1]= "vil_image_view_base_sptr"; // bool mask (with changes true and other areas false)
    return pro.set_output_types(output_types);
  } else
    return false;
}



// Execute the process
bool
bvgl_generate_mask_process2(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()) {
    std::cerr << "In bvgl_generate_mask_process::execute() - invalid inputs\n";
    return false;
  }

  std::string file_name = pro.get_input<std::string>(0);
  std::string change_type = pro.get_input<std::string>(1);

  unsigned ni=0, nj=0; // dummy initialisation, to avoid compiler warning
  ni = pro.get_input<unsigned>(2);
  nj = pro.get_input<unsigned>(3);

  // read the object
  vsl_b_ifstream bif(file_name);
  bvgl_changes_sptr objs = new bvgl_changes();
  objs->b_read(bif);
  bif.close();

  vil_image_view_base_sptr out = objs->create_mask_from_objs(ni, nj, change_type);

  vil_image_view<vxl_byte> out_view(out);
  vil_image_view<bool> out_b(ni, nj, 1);
  out_b.fill(false);

  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++)
      out_b(i,j) = (out_view(i,j) == 255 ? true : false);


  pro.set_output_val<vil_image_view_base_sptr>(0, out);
  pro.set_output_val<vil_image_view_base_sptr>(1, new vil_image_view<bool>(out_b));

  return true;
}
