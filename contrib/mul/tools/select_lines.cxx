//: \file
//  \author Ian Scott
//  \date 20 October 2008
//  \brief Select specified lines from a text file.

#include <vcl_exception.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_iterator.h>
#include <vcl_limits.h>
#include <vul/vul_arg.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_index_sort.h>
#include <mbl/mbl_load_text_file.h>

//========================================================================
// Actual main function
//========================================================================
int main2(int argc, char*argv[])
{
  const vcl_string sep="\n";

  // Parse the program arguments
  vul_arg<vcl_string> in_fname(0, "input filename (or \"-\" for stdin)");
  vul_arg<vcl_string> out_fname("-o", "output filename (defaults to stdout)", "-");
  vul_arg<vcl_vector<unsigned> > lines_arg("-l", "List of line ");
  vul_arg<vcl_string > lines_fname("-f", "Filename containing list of lines ");
  vul_arg<bool > zero_index("-zero", "Index from zero (defaults to 1-indexed) ");
  vul_arg_parse(argc, argv);


  vcl_vector<unsigned> lines;
  if (lines_arg.set())
    lines = lines_arg();
  else if (lines_fname.set())
  {
    if (!mbl_load_text_file(lines, lines_fname())) return 2;
  }
  else
    vul_arg_base::display_usage_and_exit("Must specify either \"-l\" of \"-f\".\n");

  if (!zero_index())
  {
    for (vcl_vector<unsigned>::iterator it=lines.begin(), end=lines.end(); it!=end; ++it)
    {
      if (*it==0)
      {
        vcl_cerr << "ERROR: Requested line 0, without specifying \"-zero\"\n";
        return 3;
      }
      (*it)--;
    }
  }


  // Load the input data
  vcl_ifstream in_file;
  vcl_istream *in_stream=0;
  if (in_fname() == "-")
    in_stream = &vcl_cin;
  else
  {
    in_file.open(in_fname().c_str());
    in_stream = &in_file;
    if (!in_file) mbl_exception_throw_os_error(in_fname(), "Failed to open for reading");
  }


  vcl_vector<unsigned> index;
  mbl_index_sort(lines, index);

  const unsigned n = lines.size();
  vcl_vector<unsigned> ordered_lines(n);

  for (unsigned i=0; i!=n; ++i)
    ordered_lines[i] = lines[index[i]];

// read input into data;
// if ordered_lines[i] == current line number, put current line into data[index[i]]
  vcl_vector<vcl_string> data(lines.size());
  unsigned line_no=0;
  unsigned ii=0;
  while (! !*in_stream)
  {
    if (ordered_lines[ii] != line_no)
    {
      in_stream->ignore(vcl_numeric_limits<vcl_streamsize>::max(), '\n');
    }
    else
    {
      vcl_string s;
      std::getline(*in_stream, s);
      if (in_stream->eof()) break;
      while (ii<n && ordered_lines[ii] == line_no)
      {
        data[index[ii]] = s;
        ++ii;
      }
      if (ii == n) break;
    }
    ++line_no;
  }
  if (ii != n) vcl_cerr << "WARNING: input was shorter than requested line numbers\n";


  // write data to output
  vcl_ostream *out_stream=0;
  vcl_ofstream out_file;
  if (out_fname() == "-")
    out_stream = &vcl_cout;
  else
  {
    out_file.open(out_fname().c_str());
    out_stream = &out_file;
    if (!out_file) mbl_exception_throw_os_error(out_fname(), "Failed to open for writing");
  }

  vcl_copy(data.begin(), data.end(), vcl_ostream_iterator<vcl_string>(*out_stream, "\n"));

  return 0;
}


//========================================================================
// Exception-handling wrapper around main function
//========================================================================
int main(int argc, char*argv[])
{
  try
  {
    return main2(argc, argv);
  }
  catch (vcl_exception& e)
  {
    vcl_cerr << "ERROR: " << e.what() << '\n';
    return -1;
  }
  catch (...)
  {
    vcl_cerr << "ERROR: Caught unknown exception\n";
    return -2;
  }

  return 0;
}
