// This is core/tests/test_build_info.cxx
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <ctime>
#include "test_build_info.h"
//:
// \file
// Based on ITK, Testing/Code/Common/itkSystemInformationTest.cxx

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "testlib/testlib_test.h"

// Construct the name of the notes file.
#define vxl_BUILD_INFO_NOTES                                                                                           \
  vxl_BUILD_DIR "/Testing/HTML/TestingResults/Sites/" vxl_SITE "/" vxl_BUILD_NAME "/BuildNameNotes.xml"

static std::string
get_current_date_time(const char * format)
{
  char        buf[1024];
  std::time_t t;
  std::time(&t);
  std::strftime(buf, sizeof(buf), format, std::localtime(&t));
  return buf;
}

static void
system_information_print_file(const char * name, std::ostream & os, bool note = false)
{
  if (!note)
    os << "================================\n";

  std::ifstream fin(name, std::ios::in);

  if (fin)
  {
    if (!note)
      os << "Contents of \"" << name << "\":\n"
         << "--------------------------------\n";

    // Writing out character by character is potentially slow, but
    // that's the easiest way to XMLize the output. This is not
    // performance critical code, so it's probably okay. Besides,
    // the output stream is buffered, which should help.

    std::string buffer;

    // Assume the string encoding is ASCII
    std::map<char, char const *> mapping;
    mapping['&'] = "&amp;";
    mapping['<'] = "&lt;";
    mapping['>'] = "&gt;";

    // Range of printable characters
    char const lo(32);
    char const hi(126);
    char const cr('\r');
    char const nl('\n');

    while (fin)
    {
      std::getline(fin, buffer);
      for (unsigned i = 0; i < buffer.size(); ++i)
      {
        char const & c = buffer[i];
        // Assume a "\r" at the end or a "\n" at the beginning is due to
        // mixed line-ending conversions not being handled by the
        // stream library, and so suppress them.
        if ((c == nl && i == 0) || (c == cr && i == buffer.size() - 1))
          ; // do nothing
        else if (mapping.find(c) != mapping.end())
          os << mapping[c];
        else if (buffer[i] < lo || buffer[i] > hi)
          os << "<strong>&lt;" << unsigned(buffer[i]) << "&gt;</strong>";
        else
          os << buffer[i];
      }
      os << '\n'; // the "\n" is not stored by std::getline
    }
    os.flush();
  }
  else
    os << "Could not open \"" << name << "\" for reading.\n";
}

static void
test_build_info()
{
  const char * files[] = { // Note.txt is meant to be created by hand if desired for a build
                           vxl_BUILD_DIR "/Note.txt",
                           // files below all may be generated by CMake
                           vxl_BUILD_DIR "/CMakeCache.txt",
                           vxl_BUILD_DIR "/CMakeError.log",
                           vxl_BUILD_DIR "/core/vxl_config.h",
                           vxl_BUILD_DIR "/vcl/vcl_compiler.h",
                           vxl_BUILD_DIR "/core/vil/vil_config.h",
                           vxl_BUILD_DIR "/core/vnl/vnl_config.h",
                           vxl_BUILD_DIR "/v3p/dcmtk/osconfig.h",
                           vxl_BUILD_DIR "/v3p/mpeg2/include/config.h",
                           nullptr
  };

  for (const char ** f = files; *f; f++)
    system_information_print_file(*f, std::cout);

  std::ofstream outf(vxl_BUILD_INFO_NOTES, std::ios::out);
  if (outf)
  {
    std::cout << "Also writing this information to file " << vxl_BUILD_INFO_NOTES << '\n';

    outf << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         << "<Site BuildName=\"" << vxl_BUILD_NAME << "\"  Name=\"" << vxl_SITE << "\">\n"
         << "<BuildNameNotes>\n";

    for (const char ** f = files; *f; ++f)
    {
      outf << "<Note Name=\"" << *f << "\">\n"
           << "<DateTime>" << get_current_date_time("%a %b %d %Y %H:%M:%S %Z") << "</DateTime>\n"
           << "<Text>\n";

      system_information_print_file(*f, outf, true);

      outf << "</Text>\n"
           << "</Note>\n";
    }

    outf << "</BuildNameNotes>\n"
         << "</Site>\n";
  }
  else
  {
    std::cout << "Error writing this information to file " << vxl_BUILD_INFO_NOTES << '\n';
    TEST("ofstream", true, false);
  }
}

TESTMAIN(test_build_info);
