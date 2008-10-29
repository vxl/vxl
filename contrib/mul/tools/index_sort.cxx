//: \file
//  \author Kevin de Souza
//  \date 20 October 2008
//  \brief Perform an index sort on a list of scalar numeric data


#include <vcl_exception.h>
#include <vcl_iostream.h>
#include <vcl_iterator.h>
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
  const vcl_string sep="\n";

  // Parse the program arguments
  vul_arg<vcl_string> in_file(0, "input filename (contains whitespace-separated scalar numeric values");
  vul_arg<vcl_string> out_file("-o", "output filename (contains zero-based indices into input list.", "");
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
    vcl_cerr << "ERROR: invalid input filename" << vcl_endl;
    return 1;
  }

  // Load the input data
  vcl_ifstream ifs(in_file().c_str());
  MBL_LOG(DEBUG, logger(), "Opened input file: " << in_file());
  vcl_vector<double> data;
  data.assign(vcl_istream_iterator<double>(ifs), vcl_istream_iterator<double>());
  if (data.empty())
  {
    vcl_cerr << "ERROR: Could not parse input data file." << vcl_endl;
    return 2;
  }
  ifs.close();
  MBL_LOG(INFO, logger(), "Loaded " << data.size() << " values.");
  
  // Perform the index sort - default is ascending order
  vcl_vector<unsigned> indices;
  mbl_index_sort(data, indices);

  // Reverse the order of the index list if descending order was requested
  if (reverse())
  {
    MBL_LOG(DEBUG, logger(), "Reversing order of index list");
    vcl_reverse(indices.begin(), indices.end());
  }

  // Print the results to stdout or save to file
  if (out_file.set())
  {
    if (out_file().empty())
    {
      vcl_cerr << "ERROR: invalid output filename" << vcl_endl;
      return 3;
    }
    vcl_ofstream ofs(out_file().c_str());
    for (vcl_vector<unsigned>::const_iterator it=indices.begin(), end=indices.end(); it!=end; ++it)
    {
      ofs << *it << sep;
    }
    ofs.close();
    MBL_LOG(INFO, logger(), "Saved sorted index list to file: " << out_file());
  }
  else
  {
    for (vcl_vector<unsigned>::const_iterator it=indices.begin(), end=indices.end(); it!=end; ++it)
    {
      vcl_cout << *it << sep;
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
  catch (vcl_exception& e)
  {
    vcl_cerr << "Caught exception " << e.what() << vcl_endl;
    return -1;
  }
  catch (...)
  {
    vcl_cerr << "Caught unknown exception " << vcl_endl;
    return -2;
  }

  return 0;
}
