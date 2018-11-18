/*
 * Tool to apply a mask to a file
 * This is a purely text-based operation
 */

#include <string>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <exception>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <mbl/mbl_mask.h>
#include <mbl/mbl_exception.h>



std::string trim(const std::string & s)
{
  std::size_t start = s.find_first_not_of(' ');
  if (start == std::string::npos) return "";
  unsigned end = s.find_last_not_of(' ');
  return s.substr(start, 1+end-start);
}

void split_and_add(std::vector<std::string> & values, const std::string & string, const std::string & delim)
{
  std::size_t start, next = std::string::npos; // == -1
  while (start = next+1, next = string.find_first_of(delim, start), std::string::npos != next)
  {
    std::string token = string.substr(start, next-start);
    values.push_back(trim(token));
  }
  std::string last = trim(string.substr(start));
  if (last.length() != 0) values.push_back(last);

}

bool load_vals(std::vector<std::string> & values, const std::string & filename, const std::string & delim)
{
  std::ifstream fin(filename.c_str());
  if (!fin) return false;

  values.clear();

  std::string line;
  while (std::getline(fin, line))
  {
    line = trim(line);
    if (line.length() == 0) continue;
    split_and_add(values, line, delim);
  }

  return true;
}

bool load_vals(std::vector<std::string> & values, const std::string & filename)
{
  std::ifstream fin(filename.c_str());
  if (!fin) return false;

  values.clear();

  std::string line;
  while (std::getline(fin, line))
  {
    line = trim(line);
    if (line.length() == 0) continue;
    values.push_back(line);
  }

  return true;
}

void write_vals(const std::vector<std::string> & values, std::ostream & os)
{
  std::copy(values.begin(), values.end(), std::ostream_iterator<std::string>(os, "\n"));
}



int main(int argc, char **argv)
{
  try
  {
    vul_arg<std::string> mask_filename(nullptr, "Input mask file");
    vul_arg<std::string> values_filename(nullptr, "Input values file");
    vul_arg<std::string> output_filename("-out", "Output values file - write to standard out if not set");
    vul_arg<std::string> delim("-delim", "Delimiter character for values file - one entry per line if not set");
    vul_arg_parse(argc, argv);

    if (delim.set() && delim().length() != 1)
    {
      std::cerr << "ERROR: User-defined delimiter should be one character" << std::endl;
      return 1;
    }

    mbl_mask mask;
    mbl_load_mask(mask, mask_filename().c_str());

    std::vector<std::string> values;
    bool loaded_vals;
    if (delim.set())
      loaded_vals = load_vals(values, values_filename(), delim());
    else
      loaded_vals = load_vals(values, values_filename());

    if (!loaded_vals)
    {
      std::cerr << "ERROR: Unable to load input data from " << values_filename() << std::endl;
      return 1;
    }

    mbl_apply_mask(mask, values);

    if (output_filename.set())
    {
      std::ofstream val_out(output_filename().c_str());
      if (!val_out)
      {
        mbl_exception_throw_os_error(output_filename(),
        "while trying to open for writing");
        return 1;
      }
      write_vals(values, val_out);
      val_out.close();
    }
    else write_vals(values, std::cout);

  }
  catch (std::exception & e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cerr << "ERROR: An unknown error occurred while applying the mask." << std::endl;
    return 1;
  }


}
