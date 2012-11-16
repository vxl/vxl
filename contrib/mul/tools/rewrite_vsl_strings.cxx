//: \file
//  \author Ian Scott
//  \date 10 April 2012
//  \brief Search and Replace on vsl-created files.

#include <vcl_exception.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_iterator.h>
#include <vcl_ios.h>
#include <vcl_streambuf.h>
#include <vcl_algorithm.h>
#include <vcl_sstream.h>
#include <vul/vul_arg.h>
#include <vsl/vsl_binary_io.h>
#include <mbl/mbl_exception.h>

//========================================================================
// Actual main function
//========================================================================
int main2(int argc, char*argv[])
{
  const vcl_string sep="\n";

  // Parse the program arguments
  vul_arg<vcl_string> find_str(0, "find string");
  vul_arg<vcl_string> replace_str(0, "replace string");
  vul_arg<vcl_string> in_fname(0, "input filename");
  vul_arg<vcl_string> out_fname(0, "output filename");
  vul_arg<vcl_vector<unsigned> > selection_list("-s", "Only replace the specified matches");
  vul_arg_parse(argc, argv);

  // Find vsl's encoding of find and replace strings.
  vcl_vector<char> test, replace;
  {
    vcl_ostringstream ss;
    vsl_b_ostream bfs(&ss);

    unsigned pos1 = ss.str().size();
    vsl_b_write(bfs, find_str());
    vcl_string str = ss.str();
    test.assign(str.begin() + pos1, str.end());
  }
  {
    vcl_ostringstream ss;
    vsl_b_ostream bfs(&ss);

    unsigned pos1 = ss.str().size();
    vsl_b_write(bfs, replace_str());
    vcl_string str = ss.str();
    replace.assign(str.begin() + pos1, str.end());
  }
  vcl_vector<char>::const_iterator test_begin = test.begin(), test_it=test_begin, test_end=test.end();

  vcl_vector<unsigned> selection(selection_list());
  vcl_sort(selection.begin(), selection.end());

  // Load the input data
  vcl_ifstream in_file(in_fname().c_str(), vcl_ios_binary);
  if (!in_file) mbl_exception_throw_os_error(in_fname(), "Failed to open for reading");
  vcl_ofstream out_file(out_fname().c_str(), vcl_ios_trunc | vcl_ios_binary);
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
        if (selection.empty() || vcl_binary_search(selection.begin(), selection.end(), match_count))
          vcl_copy(replace.begin(), replace.end(), out_it); // write out replacement.
        else
          vcl_copy(test_begin, test_it, out_it); // skip this match - write any stuff already found -
        test_it=test_begin;
      }
      ++file_it;
    }
    else
    {
      // not found - assume no potential overlaps.
      vcl_copy(test_begin, test_it, out_it); // write any stuff already found

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
