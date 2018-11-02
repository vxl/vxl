//: \file
//  \author Kevin de Souza
//  \date 20 October 2008
//  \brief Perform an index sort on a list of scalar numeric data


#include <iostream>
#include <exception>
#include <iterator>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <mbl/mbl_log.h>
#include <mbl/mbl_index_sort.h>


//=========================================================================
// Static function to create a static logger when first required
//=========================================================================
static mbl_logger& logger()
{
  static mbl_logger l("mul.tools.index_sort");
  return l;
}


//========================================================================
// Actual main function
//========================================================================
int main2(int argc, char*argv[])
{
  const std::string sep="\n";

  // Parse the program arguments
  vul_arg<std::string> in_file(nullptr, "input filename (contains whitespace-separated scalar numeric values");
  vul_arg<std::string> out_file("-o", "output filename (contains zero-based indices into input list.", "");
  vul_arg<bool> reverse("-r", "Sort into descending order (default is ascending order)", false);
  vul_arg_parse(argc, argv);

  // Log the program arguments
  MBL_LOG(INFO, logger(), "Program arguments: ");
  MBL_LOG(INFO, logger(), "  in_file: " << in_file());
  if (out_file.set())
    MBL_LOG(INFO, logger(), "  out_file: " << out_file());
  if (reverse.set())
    MBL_LOG(INFO, logger(), "  reverse: " << reverse());

  // Validate the program arguments
  if (in_file().empty() || !vul_file::exists(in_file()))
  {
    std::cerr << "ERROR: invalid input filename" << std::endl;
    return 1;
  }

  // Load the input data
  std::ifstream ifs(in_file().c_str());
  MBL_LOG(DEBUG, logger(), "Opened input file: " << in_file());
  std::vector<double> data;
  data.assign(std::istream_iterator<double>(ifs), std::istream_iterator<double>());
  if (data.empty())
  {
    std::cerr << "ERROR: Could not parse input data file." << std::endl;
    return 2;
  }
  ifs.close();
  MBL_LOG(INFO, logger(), "Loaded " << data.size() << " values.");

  // Perform the index sort - default is ascending order
  std::vector<unsigned> indices;
  mbl_index_sort(data, indices);

  // Reverse the order of the index list if descending order was requested
  if (reverse())
  {
    MBL_LOG(DEBUG, logger(), "Reversing order of index list");
    std::reverse(indices.begin(), indices.end());
  }

  // Print the results to stdout or save to file
  if (out_file.set())
  {
    if (out_file().empty())
    {
      std::cerr << "ERROR: invalid output filename" << std::endl;
      return 3;
    }
    std::ofstream ofs(out_file().c_str());
    for (std::vector<unsigned>::const_iterator it=indices.begin(), end=indices.end(); it!=end; ++it)
    {
      ofs << *it << sep;
    }
    ofs.close();
    MBL_LOG(INFO, logger(), "Saved sorted index list to file: " << out_file());
  }
  else
  {
    for (std::vector<unsigned>::const_iterator it=indices.begin(), end=indices.end(); it!=end; ++it)
    {
      std::cout << *it << sep;
    }
  }

  return 0;
}


//========================================================================
// Exception-handling wrapper around main function
//========================================================================
int main(int argc, char*argv[])
{
  // Initialize the logger
  mbl_logger::root().load_log_config_file();

  try
  {
    main2(argc, argv);
  }
  catch (std::exception& e)
  {
    std::cerr << "Caught exception " << e.what() << std::endl;
    return -1;
  }
  catch (...)
  {
    std::cerr << "Caught unknown exception " << std::endl;
    return -2;
  }

  return 0;
}
