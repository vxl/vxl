//:
// \file
// \brief Tool to apply mask-based merge operation between two value files

#include <vcl_string.h>
#include <vul/vul_arg.h>
#include <vcl_exception.h>
#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <mbl/mbl_mask.h>
#include <vcl_fstream.h>

// IO helpers - see below for definition
bool load_values(const vcl_string & filename, vcl_vector<vcl_string> & values, bool ignore_blanks = true);
void write_vals(const vcl_vector<vcl_string> & values, vcl_ostream & os);

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

  vul_arg<vcl_string> valueA_filename(0,"Filename of values file A");
  vul_arg<vcl_string> valueB_filename(0,"Filename of values file B");
  vul_arg<vcl_string> mask_filename(0,"Filename of mask");
  vul_arg<vcl_string> output_filename("-out","Filename for output - sent to standard out if not set");
  vul_arg<bool> use_blanks("-b", "Keep blank lines in values files (they are stripped by default)");
  vul_arg_parse(argc, argv);

  mbl_mask mask;
  mbl_load_mask(mask, mask_filename().c_str());

  vcl_vector<vcl_string> valuesA, valuesB;
  if (!load_values(valueA_filename(), valuesA, !use_blanks.set()))
  {
    vcl_cout << "Unable to load values from file " << valueA_filename() << vcl_endl;
    return 1;
  }
  if (!load_values(valueB_filename(), valuesB, !use_blanks.set()))
  {
    vcl_cout << "Unable to load values from file " << valueB_filename() << vcl_endl;
    return 1;
  }

  try {
    mbl_mask_merge_values(mask, valuesA.begin(), valuesA.end(), valuesB.begin(), valuesB.end(), valuesB.begin());
  }
  catch (vcl_exception & e)
  {
    vcl_cout << "An error occurred while merging the data.\n" << e.what() << vcl_endl;
    return 1;
  }
  catch (...)
  {
    vcl_cout << "An unknown error occurred while merging the data." << vcl_endl;
    return 1;
  }

  if (output_filename.set())
  {
    vcl_ofstream val_out(output_filename().c_str());
    if (!val_out)
    {
      vcl_cout << "Unable to save output data to " << output_filename() << vcl_endl;
      return 1;
    }
    write_vals(valuesB, val_out);
    val_out.close();
  }
  else
    write_vals(valuesB, vcl_cout);
}


// io helpers

bool load_values(const vcl_string & filename, vcl_vector<vcl_string> & values, bool ignore_blanks)
{
  vcl_ifstream fin(filename.c_str());
  if (!fin) return false;

  values.clear();

  vcl_string line;
  while (vcl_getline(fin, line))
  {
    if (line.empty() && ignore_blanks) continue;
    values.push_back(line);
  }
  return true;
}

void write_vals(const vcl_vector<vcl_string> & values, vcl_ostream & os)
{
  vcl_copy(values.begin(), values.end(), vcl_ostream_iterator<vcl_string>(os, "\n"));
}
