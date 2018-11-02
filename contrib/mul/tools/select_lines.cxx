//: \file
//  \author Ian Scott
//  \date 20 October 2008
//  \brief Select specified lines from a text file.

#include <iostream>
#include <exception>
#include <fstream>
#include <iterator>
#include <limits>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_index_sort.h>
#include <mbl/mbl_load_text_file.h>

//========================================================================
// Actual main function
//========================================================================
int main2(int argc, char*argv[])
{
  const std::string sep="\n";

  // Parse the program arguments
  vul_arg<std::string> in_fname(nullptr, "input filename (or \"-\" for stdin)");
  vul_arg<std::string> out_fname("-o", "output filename (defaults to stdout)", "-");
  vul_arg<std::vector<unsigned> > lines_arg("-l", "List of line ");
  vul_arg<std::string > lines_fname("-f", "Filename containing list of lines ");
  vul_arg<bool > zero_index("-zero", "Index from zero (defaults to 1-indexed) ");
  vul_arg_parse(argc, argv);


  std::vector<unsigned> lines;
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
    for (unsigned int & line : lines)
    {
      if (line==0)
      {
        std::cerr << "ERROR: Requested line 0, without specifying \"-zero\"\n";
        return 3;
      }
      line--;
    }
  }


  // Load the input data
  std::ifstream in_file;
  std::istream *in_stream=nullptr;
  if (in_fname() == "-")
    in_stream = &std::cin;
  else
  {
    in_file.open(in_fname().c_str());
    in_stream = &in_file;
    if (!in_file) mbl_exception_throw_os_error(in_fname(), "Failed to open for reading");
  }


  std::vector<unsigned> index;
  mbl_index_sort(lines, index);

  const unsigned n = lines.size();
  std::vector<unsigned> ordered_lines(n);

  for (unsigned i=0; i!=n; ++i)
    ordered_lines[i] = lines[index[i]];

// read input into data;
// if ordered_lines[i] == current line number, put current line into data[index[i]]
  std::vector<std::string> data(lines.size());
  unsigned line_no=0;
  unsigned ii=0;
  while (! !*in_stream)
  {
    if (ordered_lines[ii] != line_no)
    {
      in_stream->ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    else
    {
      std::string s;
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
  if (ii != n) std::cerr << "WARNING: input was shorter than requested line numbers\n";


  // write data to output
  std::ostream *out_stream=nullptr;
  std::ofstream out_file;
  if (out_fname() == "-")
    out_stream = &std::cout;
  else
  {
    out_file.open(out_fname().c_str());
    out_stream = &out_file;
    if (!out_file) mbl_exception_throw_os_error(out_fname(), "Failed to open for writing");
  }

  std::copy(data.begin(), data.end(), std::ostream_iterator<std::string>(*out_stream, "\n"));

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
  catch (std::exception& e)
  {
    std::cerr << "ERROR: " << e.what() << '\n';
    return -1;
  }
  catch (...)
  {
    std::cerr << "ERROR: Caught unknown exception\n";
    return -2;
  }

  return 0;
}
