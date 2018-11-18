//:
// \file
// \brief Convert a list of indices to a mask.

#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <stdexcept>
#include <iterator>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <mbl/mbl_log.h>
#include <mbl/mbl_mask.h>
#include <mbl/mbl_exception.h>


//=========================================================================
// Static function to create a static logger when first required
//=========================================================================
static mbl_logger& logger()
{
  static mbl_logger l("mul.mbl.tools.convert_indices_to_mask");
  return l;
}


//=========================================================================
// Load a vector of unsigned from named file
//=========================================================================
static void load_indices(std::vector<unsigned>& indices,
                         const std::string& path)
{
  std::ifstream ifs(path.c_str());
  if (!ifs)
    mbl_exception_throw_os_error(path, "load_indices() could not open file");
  indices.assign(std::istream_iterator<unsigned>(ifs), std::istream_iterator<unsigned>());

  if (indices.empty())
    throw mbl_exception_parse_error("Could not parse indices file.");
}


//=========================================================================
// Main function
//=========================================================================
int main2(int argc, char *argv[])
{
  int ret_val=0;

  vul_arg_base::set_help_precis("Convert a list of zero-based indices to a boolean mask.");
  vul_arg_base::set_help_description(
    "Convert a list of zero-based indices to a boolean mask.\n"
    "Mask will be false everywhere except at the specified indices.\n"
    "By default, the mask length will be defined by the highest index present,\n"
    "unless you specify the -n option."
  );

  // Parse command line arguments
  vul_arg<std::string> inds_file(nullptr, "INPUT indices file");
  vul_arg<unsigned> n("-n", "Length of mask; default is to use highest index in indices file");
  vul_arg<std::string> mask_file(nullptr, "OUTPUT mask file");
  vul_arg_parse(argc, argv);

  std::vector<unsigned> inds;
  load_indices(inds, inds_file());

  unsigned num=0;
  if (n.set())
  {
    num = n();
  }
  else
  {
    num = 1 + *std::max_element(inds.begin(), inds.end());
  }

  mbl_mask mask;
  mbl_indices_to_mask(inds, num, mask);

  mbl_save_mask(mask, mask_file());

  return ret_val;
}


//=========================================================================
// Main function with exception-handling wrapper and logging
//=========================================================================
int main(int argc, char *argv[])
{
  int retcode = 0;

  try
  {
    mbl_logger::root().load_log_config_file();
    retcode = main2(argc, argv);
  }
  catch (const std::runtime_error &e)
  {
    std::cout << "\n";
    std::cout << "====================================\n";
    std::cout << "Caught std::runtime_error: " << e.what() << "\n";
    std::cout << "Ending program.\n";
    std::cout << "====================================\n" << std::endl;
    MBL_LOG(ERR, logger(), "Caught exception: " << e.what());
    retcode = 1;
  }
  catch (...)
  {
    std::cout << "\n";
    std::cout << "====================================\n";
    std::cout << "Caught unknown exception.\n";
    std::cout << "Ending program.\n";
    std::cout << "====================================\n" << std::endl;
    MBL_LOG(ERR, logger(), "Caught unknown exception");
    retcode = 2;
  }

  return retcode;
}
