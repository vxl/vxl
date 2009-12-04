/*
 * Tool to apply any logical operations on pairs of masks
 * using a general 'truth table' rule
 */

#include <vcl_string.h>
#include <vcl_exception.h>
#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <mbl/mbl_mask.h>

int main(int argc, char **argv)
{
  vul_arg_base::set_help_description(
    "Apply a logical operation on a pair of mask files.\n"
    "--------------------------------------------------\n"
    "Uses an 'operation' string that defines the 'rule' that is applied between the two masks\n"
    "The operation must consist of 4 characters, each either 0 or 1\n"
    "This will be applied to each possible combination of A and B values\n"
    "For example, the truth table below performs an AND operation:\n"
    "\n"
    "        |-A-|-B-|-OUT-|\n"
    "        | 0 | 0 |  0  |\n"
    "        | 0 | 1 |  0  |\n"
    "        | 1 | 0 |  0  |\n"
    "        | 1 | 1 |  1  |\n"
    "\n"
    "The rule in this case would be '0001'\n"
    "\n"
    "Some of the more common logical operations:\n"
    "   A AND B  = 0001\n"
    "   A OR B   = 0111\n"
    "   NOT A    = 1100 (Note: Mask B must still exist despite not being used in NOT A\n"
    "   NOT B    = 0011        and vice versa)\n"
    "   A XOR B  = 0110\n"
    "   A NOR B  = 1000\n"
    "   A XNOR B = 1001\n"
    "   A NAND B = 1110\n"
    "\n"
  );

  vul_arg<vcl_string> maskA_filename(0,"Filename of mask A");
  vul_arg<vcl_string> maskB_filename(0,"Filename of mask B");
  vul_arg<vcl_string> operation(0,"Operation to apply - see help text for explanation");
  vul_arg<vcl_string> maskout_filename(0,"Filename of the output mask");
  vul_arg_parse(argc, argv);

  mbl_mask maskA, maskB;
  mbl_load_mask(maskA, maskA_filename().c_str());
  mbl_load_mask(maskB, maskB_filename().c_str());

  try { mbl_mask_logic(maskA, maskB, operation()); }
  catch (vcl_exception & e)
  {
    vcl_cout << "An error occurred while carrying out the logic operation.\n" << e.what() << vcl_endl;
    return 1;
  }
  catch (...)
  {
    vcl_cout << "An unknown error occurred while carrying out the logic operation." << vcl_endl;
    return 1;
  }
  
  mbl_save_mask(maskB, maskout_filename().c_str());
}
