//:
// \file
// \brief Tool to apply mask-based merge operation between two value files

#include <string>
#include <exception>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vul/vul_arg.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_mask.h>

// IO helpers - see below for definition
bool load_values(const std::string & filename, std::vector<std::string> & values, bool ignore_blanks = true);
void write_vals(const std::vector<std::string> & values, std::ostream & os);

int main(int argc, char **argv)
{
  vul_arg_base::set_help_description(
    "Merge two files according to a mask.\n"
    "------------------------------------\n"
    "The two value files and the mask must be the same length N\n"
    "The value files will be merged as follows:\n\n"
    "  For every entry index i in the file (0..N-1)\n"
    "    If i'th mask value is false use i'th entry from first value file\n"
    "    Else use i'th entry from second value file\n\n"
    "The output will also be of length N\n\n"
    "Note: blank lines in the values files will be ignored unless -b flag is set\n"
    "      blank lines in the mask are always ignored\n"
    "\n"
  );

  vul_arg<std::string> valueA_filename(nullptr,"Filename of values file A");
  vul_arg<std::string> valueB_filename(nullptr,"Filename of values file B");
  vul_arg<std::string> mask_filename(nullptr,"Filename of mask");
  vul_arg<std::string> output_filename("-out","Filename for output - sent to standard out if not set");
  vul_arg<bool> use_blanks("-b", "Keep blank lines in values files (they are stripped by default)");
  vul_arg_parse(argc, argv);

  mbl_mask mask;
  mbl_load_mask(mask, mask_filename().c_str());

  std::vector<std::string> valuesA, valuesB;
  if (!load_values(valueA_filename(), valuesA, !use_blanks.set()))
  {
    std::cout << "Unable to load values from file " << valueA_filename() << std::endl;
    return 1;
  }
  if (!load_values(valueB_filename(), valuesB, !use_blanks.set()))
  {
    std::cout << "Unable to load values from file " << valueB_filename() << std::endl;
    return 1;
  }

  try {
    mbl_mask_merge_values(mask, valuesA.begin(), valuesA.end(), valuesB.begin(), valuesB.end(), valuesB.begin());
  }
  catch (std::exception & e)
  {
    std::cout << "An error occurred while merging the data.\n" << e.what() << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cout << "An unknown error occurred while merging the data." << std::endl;
    return 1;
  }

  if (output_filename.set())
  {
    std::ofstream val_out(output_filename().c_str());
    if (!val_out)
    {
      std::cout << "Unable to save output data to " << output_filename() << std::endl;
      return 1;
    }
    write_vals(valuesB, val_out);
    val_out.close();
  }
  else
    write_vals(valuesB, std::cout);
}


// io helpers

bool load_values(const std::string & filename, std::vector<std::string> & values, bool ignore_blanks)
{
  std::ifstream fin(filename.c_str());
  if (!fin) return false;

  values.clear();

  std::string line;
  while (std::getline(fin, line))
  {
    if (line.empty() && ignore_blanks) continue;
    values.push_back(line);
  }
  return true;
}

void write_vals(const std::vector<std::string> & values, std::ostream & os)
{
  std::copy(values.begin(), values.end(), std::ostream_iterator<std::string>(os, "\n"));
}
