/*
 * Tool to replace 'true' values in B with values taken from A.
 *  Size of A must match 'true' count in B
 */

#include <string>
#include <iostream>
#include <exception>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <mbl/mbl_mask.h>

int main(int argc, char **argv)
{
  vul_arg_base::set_help_description(
    "DESCRIPTION:\n"
    "Input mask B has n elements (of which m are TRUE) and thus transforms a vector X of n elements to a vector Y of m elements.\n"
    "Input mask A has m elements (of which l are TRUE) and thus transforms a vector Y of m elements to a vector Z of l elements.\n"
    "Output mask C has n elements (of which l are TRUE) and thus transforms a vector X of n elements to a vector Z of l elements.\n");
  vul_arg<std::string> maskA_filename(nullptr,"Filename of mask A");
  vul_arg<std::string> maskB_filename(nullptr,"Filename of mask B");
  vul_arg<std::string> maskout_filename(nullptr,"Filename of the output mask");
  vul_arg_parse(argc, argv);

  mbl_mask maskA, maskB;
  mbl_load_mask(maskA, maskA_filename().c_str());
  mbl_load_mask(maskB, maskB_filename().c_str());

  try { mbl_mask_on_mask(maskA, maskB); }
  catch (std::exception & e)
  {
    std::cout << "An error occurred while applying the mask.\n" << e.what() << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cout << "An unknown error occurred while applying the mask." << std::endl;
    return 1;
  }

  mbl_save_mask(maskB, maskout_filename().c_str());
}
