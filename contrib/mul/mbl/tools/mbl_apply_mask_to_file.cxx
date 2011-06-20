/*
 * Tool to apply a mask to a file
 * This is a purely text-based operation
 */

#include <vcl_string.h>
#include <vcl_cstddef.h> // for std::size_t
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vcl_algorithm.h>
#include <vul/vul_arg.h>
#include <vcl_exception.h>
#include <mbl/mbl_mask.h>
#include <mbl/mbl_exception.h>



vcl_string trim(const vcl_string & s)
{
  vcl_size_t start = s.find_first_not_of(" ");
  if (start == vcl_string::npos) return "";
  unsigned end = s.find_last_not_of(" ");
  return s.substr(start, 1+end-start);
}

void split_and_add(vcl_vector<vcl_string> & values, const vcl_string & string, const vcl_string & delim)
{
  vcl_size_t start, next = vcl_string::npos; // == -1
  while (start = next+1, next = string.find_first_of(delim, start), vcl_string::npos != next)
  {
    vcl_string token = string.substr(start, next-start);
    values.push_back(trim(token));
  }
  vcl_string last = trim(string.substr(start));
  if (last.length() != 0) values.push_back(last);
  
}

bool load_vals(vcl_vector<vcl_string> & values, const vcl_string & filename, const vcl_string & delim)
{
  vcl_ifstream fin(filename.c_str());
  if (!fin) return false;

  values.clear();

  vcl_string line;
  while (vcl_getline(fin, line))
  {
    line = trim(line);
    if (line.length() == 0) continue;
    split_and_add(values, line, delim);
  }

  return true;
}

bool load_vals(vcl_vector<vcl_string> & values, const vcl_string & filename)
{
  vcl_ifstream fin(filename.c_str());
  if (!fin) return false;

  values.clear();

  vcl_string line;
  while (vcl_getline(fin, line))
  {
    line = trim(line);
    if (line.length() == 0) continue;
    values.push_back(line);
  }

  return true;
}

void write_vals(const vcl_vector<vcl_string> & values, vcl_ostream & os)
{
  vcl_copy(values.begin(), values.end(), vcl_ostream_iterator<vcl_string>(os, "\n"));
}



int main(int argc, char **argv)
{
  try
  {
    vul_arg<vcl_string> mask_filename(0, "Input mask file");
    vul_arg<vcl_string> values_filename(0, "Input values file");
    vul_arg<vcl_string> output_filename("-out", "Output values file - write to standard out if not set");
    vul_arg<vcl_string> delim("-delim", "Delimiter character for values file - one entry per line if not set");
    vul_arg_parse(argc, argv);

    if (delim.set() && delim().length() != 1)
    {
      vcl_cerr << "ERROR: User-defined delimiter should be one character" << vcl_endl;
      return 1;
    }

    mbl_mask mask;
    mbl_load_mask(mask, mask_filename().c_str());

    vcl_vector<vcl_string> values;
    bool loaded_vals;
    if (delim.set())
      loaded_vals = load_vals(values, values_filename(), delim());
    else
      loaded_vals = load_vals(values, values_filename());

    if (!loaded_vals)
    {
      vcl_cerr << "ERROR: Unable to load input data from " << values_filename() << vcl_endl;
      return 1;
    }

    mbl_apply_mask(mask, values);

    if (output_filename.set())
    {
      vcl_ofstream val_out(output_filename().c_str());
      if (!val_out)
      {
        mbl_exception_throw_os_error(output_filename(),
        "while trying to open for writing");
        return 1;
      }
      write_vals(values, val_out);
      val_out.close();
    }
    else write_vals(values, vcl_cout);

  }
  catch (vcl_exception & e)
  {
    vcl_cerr << "ERROR: " << e.what() << vcl_endl;
    return 1;
  }
  catch (...)
  {
    vcl_cerr << "ERROR: An unknown error occurred while applying the mask." << vcl_endl;
    return 1;
  }


}






