/*
 * Tool to replace 'true' values in B with values taken from A.
 *  Size of A must match 'true' count in B
 */

#include <vcl_string.h>
#include <vcl_exception.h>
#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <mbl/mbl_mask.h>

int main(int argc, char **argv)
{
  vul_arg_base::set_help_description(
    "DESCRIPTION:\n"
    "Input mask B has n elements (of which m are TRUE) and thus transforms a vector X of n elements to a vector Y of m elements.\n"
    "Input mask A has m elements (of which l are TRUE) and thus transforms a vector Y of m elements to a vector Z of l elements.\n"
    "Output mask C has n elements (of which l are TRUE) and thus transforms a vector X of n elements to a vector Z of l elements.\n");
  vul_arg<vcl_string> maskA_filename(0,"Filename of mask A");
  vul_arg<vcl_string> maskB_filename(0,"Filename of mask B");
  vul_arg<vcl_string> maskout_filename(0,"Filename of the output mask");
  vul_arg_parse(argc, argv);

  mbl_mask maskA, maskB;
  mbl_load_mask(maskA, maskA_filename().c_str());
  mbl_load_mask(maskB, maskB_filename().c_str());
  
  try { mbl_mask_on_mask(maskA, maskB); }
  catch (vcl_exception & e)
  {
    vcl_cout << "An error occurred while applying the mask.\n" << e.what() << vcl_endl;
    return 1;
  }
  catch (...)
  {
    vcl_cout << "An unknown error occurred while applying the mask." << vcl_endl;
    return 1;
  }

  mbl_save_mask(maskB, maskout_filename().c_str());
}
