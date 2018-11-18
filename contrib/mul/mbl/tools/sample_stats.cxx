//////////////////////////////////////////////////////////////////////////
//
// Calculate various statistics on a sample of 1D data.
//
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iterator>
#include <fstream>
#include <string>
#include <exception>
#include <map>
#include <typeinfo>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vul/vul_sprintf.h>
#include <mbl/mbl_log.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_sample_stats_1d.h>


//! Identify which format to use for output.
enum OutputFormat
{
  LIST,   //!< Write each statistic on a separate line, including its title
  TABLE   //!< Write all statistics on one line, arranged in columns
};


//=========================================================================
// Static function to create a static logger when first required
//=========================================================================
static mbl_logger& logger()
{
  static mbl_logger l("mul.mbl.tools.sample_stats");
  return l;
}


//=========================================================================
// Report an error message in several ways, then throw an exception.
//=========================================================================
static void do_error(const std::string& msg)
{
  MBL_LOG(ERR, logger(), msg);
  throw mbl_exception_abort(msg);
}


//=========================================================================
// Actual main function (apart from exception-handling wrapper)
//=========================================================================
int main2(int argc, char *argv[])
{
  vul_arg_base::set_help_precis("Compute various statistics of 1D data");
  vul_arg_base::set_help_description("DETAILED INFORMATION\n"
    "1. By default, data is read from stdin, unless the -i option is used to specify an input filename.\n\n"
    "2. By default, output is written to stdout, unless the -o option is used to specify an output filename.\n"
    "   When writing to an output file, results are *appended* to the specified file.\n"
    "   This permits the program to be run multiple times *sequentially* on different data, building up a list of comparable statistics.\n"
    "   Results are undefined if this program is run multiple times *simultaneously* with the -o option specifying the same file.\n\n"
    "3. A choice of 2 output formats is provided with the -fmt option: \"table\" and \"list\". The default is \"list\".\n"
    "   list: write each statistic on a separate line, including its title.\n"
    "   table: write all statistics on one line, arranged in columns, with optional column headers.\n"
    "     In \"table\" format, column headers are printed by default, unless the -h option is specified.\n"
    "4. The desired statistics may be specified in any order, but the output is printed in alphabetical order.\n"
  );

  // Parse the program arguments

  // These options are I/O and format-related
  vul_arg<std::string> in_file("-i", "input file containing scalar values (whitespace-separated); otherwise uses stdin", "");
  vul_arg<std::string> out_file("-o", "output file to append statistics; otherwise write to stdout", "");
  vul_arg<std::string> label("-label","Adds this label to each line outputting a statistic - useful for later grep");
  vul_arg<std::string> format("-fmt","Specify the output format, e.g. \"table\", \"list\" (default is list). See help for more details.");
  vul_arg<bool> nohead("-h","Specify this to SUPPRESS column headers in tabular format", false);
  vul_arg<std::string> sep("-sep", "String to use as a separator between columns in tabular format, e.g. \", \" or \"  \" (default=TAB)", "\t");
  // These options are statistical measures:
  vul_arg<bool> n("-n", "Specify this to record the number of samples", false);
  vul_arg<bool> mean("-mean", "Specify this to record the mean", false);
  vul_arg<bool> variance("-var", "Specify this to record the variance", false);
  vul_arg<bool> sd("-sd", "Specify this to record the standard deviation", false);
  vul_arg<bool> se("-se", "Specify this to record the standard error", false);
  vul_arg<bool> med("-med", "Specify this to record the median", false);
  vul_arg<bool> min("-min", "Specify this to record the minimum", false);
  vul_arg<bool> max("-max", "Specify this to record the maximum", false);
  vul_arg<std::vector<int> > pc("-pc", "Specify this switch with 1 or more comma-separated integers (e.g. 5,50,95) to record percentile(s)");
  vul_arg<std::vector<double> > quant("-q", "Specify this switch with 1 or more comma-separated floats (e.g. 0.05,0.50,0.95) to record quantile(s)");
  vul_arg<bool> sum("-sum", "Specify this to record the sum", false);
  vul_arg<bool> sum_squares("-ssq", "Specify this to record the sum of squares", false);
  vul_arg<bool> rms("-rms", "Specify this to record the rms", false);
  vul_arg<bool> mean_of_absolutes("-moa", "Specify this to record the mean_of_absolutes", false);
  vul_arg<bool> skewness("-skew", "Specify this to record the skewness", false);
  vul_arg<bool> kurtosis("-kurt", "Specify this to record the kurtosis", false);
  vul_arg<bool> absolute("-absolute", "Calculate statistics of absolute sample values", false);
  vul_arg_parse(argc, argv);

  // Try to open the input file if specified or use stdin
  std::istream* is=nullptr;
  if (!in_file().empty())
  {
    is = new std::ifstream(in_file().c_str());
    if (!is || !is->good())
      do_error(std::string("Failed to open input file ") + in_file().c_str());
    MBL_LOG(DEBUG, logger(), "Opened input file: " << in_file().c_str());
  }
  else
  {
    is = &std::cin;
  }
  MBL_LOG(INFO, logger(), "in_file: " << (in_file.set() ? in_file() : std::string("stdin")));

  // Load the data from stream until end
  std::vector<double> data_vec;
  data_vec.assign(std::istream_iterator<double>(*is), std::istream_iterator<double>());
  if (data_vec.empty())
    do_error("Could not parse data file.");
  MBL_LOG(DEBUG, logger(), "data file contained " << data_vec.size() << " values.");

  if (absolute())
  {
    for (double & i : data_vec) i=std::abs(i);
  }

  // Clean up if input was from a file
  {
    auto* ifs = dynamic_cast<std::ifstream*>(is);
    if (ifs)
    {
      ifs->close();
      delete ifs;
    }
    is = nullptr;
  }

  // Calculate the requested statistics
  mbl_sample_stats_1d data(data_vec);
  std::map<std::string, double> stats;
  if (n.set())                 stats["n"]=data.n_samples();
  if (mean.set())              stats["mean"]=data.mean();
  if (variance.set())          stats["var"]=data.variance();
  if (sd.set())                stats["sd"]=data.sd();
  if (se.set())                stats["se"]=data.stdError();
  if (med.set())               stats["med"]=data.median();
  if (min.set())               stats["min"]=data.min();
  if (max.set())               stats["max"]=data.max();
  if (sum.set())               stats["sum"]=data.sum();
  if (sum_squares.set())       stats["ssq"]=data.sum_squares();
  if (rms.set())               stats["rms"]=data.rms();
  if (mean_of_absolutes.set()) stats["moa"]=data.mean_of_absolutes();
  if (skewness.set())          stats["skew"]=data.skewness();
  if (kurtosis.set())          stats["kurt"]=data.kurtosis();
  if (pc.set())
  {
    for (std::vector<int>::const_iterator it=pc().begin(); it!=pc().end(); ++it)
    {
      std::string name = vul_sprintf("pc%02u", *it);
      stats[name] = data.nth_percentile(*it);
    }
  }
  if (quant.set())
  {
    for (std::vector<double>::const_iterator it=quant().begin(); it!=quant().end(); ++it)
    {
      std::string name = vul_sprintf("q%f", *it);
      stats[name] = data.quantile(*it);
    }
  }

  // Open output file if requested, otherwise use stdout
  std::ostream* os=nullptr;
  if (!out_file().empty())
  {
    os = new std::ofstream(out_file().c_str(), std::ios::app);
    if (!os || !os->good())
      do_error(std::string("Failed to open outout file ") + out_file().c_str());
    MBL_LOG(DEBUG, logger(), "Opened output file: " << out_file().c_str());
  }
  else
  {
    os = &std::cout;
  }


  // Use provided label if specified, otherwise use input filename (or empty string).
  std::string my_label = label.set() ? label() : in_file();

  // Write statistics in 1 of multiple formats
  enum OutputFormat output_format = format()=="table" ? TABLE : LIST;
  switch (output_format)
  {
  case TABLE: // Tabular format
    {
      // Write a line of column headers unless suppressed
      if (!nohead())
      {
        if (os && os->good()) *os << '#' << sep();
        for (std::map<std::string,double>::const_iterator it=stats.begin(); it!=stats.end(); ++it)
        {
          if (os && os->good()) *os << it->first << sep();
        }
        if (os && os->good()) *os << '\n';
      }

      // Write all statistics on one line arranged in columns
      if (os && os->good()) *os << my_label << sep();
      for (std::map<std::string,double>::const_iterator it=stats.begin(); it!=stats.end(); ++it)
      {
        if (os && os->good())
          *os << it->second << sep();
      }
      if (os && os->good()) *os << std::endl;
    }
    break;

  default:
    {
      // List format
      for (std::map<std::string,double>::const_iterator it=stats.begin(); it!=stats.end(); ++it)
      {
        if (os && os->good())
        {
          *os <<  my_label << " " << it->first << ": " << it->second << "\n";
        }
      }
    }
    break;
  }

  // Clean up if output was to a file
  {
    auto* ofs = dynamic_cast<std::ofstream*>(os);
    if (ofs)
    {
      ofs->close();
      delete ofs;
    }
    os = nullptr;
  }

  return 0;
}


//=========================================================================
// main() function with exception-handling wrapper
//=========================================================================
int main(int argc, char *argv[])
{
  int errcode = 0;

  // Initialize the logger
  mbl_logger::root().load_log_config_file();

  try
  {
    errcode = main2(argc, argv);
  }
  catch (const std::exception & e)
  {
    std::cerr << "ERROR: " << typeid(e).name() << '\n' <<
      e.what() << std::endl;
    errcode = 4;
  }

  return errcode;
}
