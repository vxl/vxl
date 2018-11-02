//: \file
//  \author Ian Scott
//  \date 10 April 2012
//  \brief Search and Replace on vsl-created files.

#include <exception>
#include <iostream>
#include <fstream>
#include <iterator>
#include <ios>
#include <streambuf>
#include <algorithm>
#include <sstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vsl/vsl_binary_io.h>
#include <mbl/mbl_exception.h>

//========================================================================
// Actual main function
//========================================================================
int main2(int argc, char*argv[])
{
  const std::string sep="\n";

  // Parse the program arguments
  vul_arg<std::string> find_str(nullptr, "find string");
  vul_arg<std::string> replace_str(nullptr, "replace string");
  vul_arg<std::string> in_fname(nullptr, "input filename");
  vul_arg<std::string> out_fname(nullptr, "output filename");
  vul_arg<std::vector<unsigned> > selection_list("-s", "Only replace the specified matches");
  vul_arg_parse(argc, argv);

  // Find vsl's encoding of find and replace strings.
  std::vector<char> test, replace;
  {
    std::ostringstream ss;
    vsl_b_ostream bfs(&ss);

    unsigned pos1 = ss.str().size();
    vsl_b_write(bfs, find_str());
    std::string str = ss.str();
    test.assign(str.begin() + pos1, str.end());
  }
  {
    std::ostringstream ss;
    vsl_b_ostream bfs(&ss);

    unsigned pos1 = ss.str().size();
    vsl_b_write(bfs, replace_str());
    std::string str = ss.str();
    replace.assign(str.begin() + pos1, str.end());
  }
  std::vector<char>::const_iterator test_begin = test.begin(), test_it=test_begin, test_end=test.end();

  std::vector<unsigned> selection(selection_list());
  std::sort(selection.begin(), selection.end());

  // Load the input data
  std::ifstream in_file(in_fname().c_str(), std::ios::binary);
  if (!in_file) mbl_exception_throw_os_error(in_fname(), "Failed to open for reading");
  std::ofstream out_file(out_fname().c_str(), std::ios::trunc | std::ios::binary);
  if (!out_file) mbl_exception_throw_os_error(out_fname(), "Failed to open for writing");

  std::istreambuf_iterator<char> file_it(in_file), eof;
  std::ostreambuf_iterator<char> out_it(out_file);

  unsigned match_count=0;

  while (file_it != eof)
  {
    if (*file_it == *test_it)
    {
      ++test_it;
      if (test_it == test_end) // found whole string.
      {
        match_count++;
        if (selection.empty() || std::binary_search(selection.begin(), selection.end(), match_count))
          std::copy(replace.begin(), replace.end(), out_it); // write out replacement.
        else
          std::copy(test_begin, test_it, out_it); // skip this match - write any stuff already found -
        test_it=test_begin;
      }
      ++file_it;
    }
    else
    {
      // not found - assume no potential overlaps.
      std::copy(test_begin, test_it, out_it); // write any stuff already found

      test_it=test_begin;

      *out_it++ = *file_it++;
    }
  }

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
