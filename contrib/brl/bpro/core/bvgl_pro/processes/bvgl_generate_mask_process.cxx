// This is brl/bpro/bvgl_pro/bvgl_generate_mask_process.cxx

//:
// \file

#include <vil/vil_image_view.h>
#include <vcl_iostream.h>
#include <bvgl/bvgl_changes.h>
#include <bvgl/bvgl_changes_sptr.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_func_process.h>
#define NI "ni"
#define NJ "nj"
// Constructor
bool bvgl_generate_mask_process_cons(bprb_func_process& pro)
{
  //input
  vcl_vector<vcl_string> input_types, output_types;
  input_types.resize(2);
  input_types[0]= "vcl_string"; //name of the binary file to read bvgl_changes object
  input_types[1]= "vcl_string"; //type of change

  //output

  output_types.resize(2);
  output_types[0]= "vil_image_view_base_sptr"; // vxl_byte mask (with don't care areas)
  output_types[1]= "vil_image_view_base_sptr"; // bool mask (with changes true and other areas false)
  return true;
}



// Execute the process
bool
bvgl_generate_mask_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()) {
    vcl_cerr << "In bvgl_generate_mask_process::execute() - invalid inputs\n";
    return false;
  }

  vcl_string file_name = pro.get_input<vcl_string>(0);

  vcl_string change_type = pro.get_input<vcl_string>(1);


  unsigned ni=0, nj=0; // dummy initialisation, to avoid compiler warning
  if(!pro.parameters()->get_value(NI, ni) || 
     !pro.parameters()->get_value(NJ, nj))
    { 
      vcl_cerr << "In bvgl_generate_mask_process - failed to get parameters\n";
      return false;
    }

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

