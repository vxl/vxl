//:
// \file
// \brief Convert a mask file to a list of indices file.

#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vul/vul_arg.h>
#include <mbl/mbl_log.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_mask.h>


//=========================================================================
// Static function to create a static logger when first required
//=========================================================================
static mbl_logger& logger()
{
  static mbl_logger l("mul.mbl.tools.convert_mask_to_indices");
  return l;
}


//=======================================================================
// Write a vector of unsigned values to a stream with a specified delimiter.
//=======================================================================
static void save_indices(const vcl_vector<unsigned>& indices,
                         const vcl_string& path,
                         const vcl_string& delim="\n")
{
  vcl_ofstream afs(path.c_str());
  if (!afs)
    mbl_exception_throw_os_error(path, "save_indices() could not open file");
  for (vcl_vector<unsigned>::const_iterator it=indices.begin(), end=indices.end(); it!=end; ++it)
  {
    afs << *it << delim;
  }
  afs.close();
}


//=========================================================================
// Main function
//=========================================================================
int main2(int argc, char *argv[])
{
  int ret_val=0;
  
  vul_arg_base::set_help_precis("Convert a boolean mask to a list of zero-based indices.");
  vul_arg_base::set_help_description(
    "Convert a boolean mask to a list of zero-based indices.\n"
    "Indices i will be recorded wherever mask[i] is true.\n"
  );

  // Parse command line arguments
  vul_arg<vcl_string> mask_file(0, "INPUT mask file");
  vul_arg<vcl_string> inds_file(0, "OUTPUT indices file");
  vul_arg_parse(argc, argv);

  mbl_mask mask;
  mbl_load_mask(mask, mask_file());

  vcl_vector<unsigned> inds;
  mbl_mask_to_indices(mask, inds);
  save_indices(inds, inds_file());

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
  catch (const vcl_runtime_error &e)
  {
    vcl_cout << "\n";
    vcl_cout << "====================================\n";
    vcl_cout << "Caught vcl_runtime_error: " << e.what() << "\n";
    vcl_cout << "Ending program.\n";
    vcl_cout << "====================================\n" << vcl_endl;
    MBL_LOG(ERR, logger(), "Caught exception: " << e.what());
    retcode = 1;
  }
  catch (...)
  {
    vcl_cout << "\n";
    vcl_cout << "====================================\n";
    vcl_cout << "Caught unknown exception.\n";
    vcl_cout << "Ending program.\n";
    vcl_cout << "====================================\n" << vcl_endl;
    MBL_LOG(ERR, logger(), "Caught unknown exception");
    retcode = 2;
  }

  return retcode;
}

