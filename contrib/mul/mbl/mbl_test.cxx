#include <cerrno>
#include <string>
#include <fstream>
#include <ctime>
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include "mbl_test.h"
//:
// \file
// \brief A place for useful things associated with testing.
// \author iscott
// \date  Aug 2005

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <vul/vul_expand_path.h>
#include <mbl/mbl_config.h>


//: replace instances of 'from' in 's' with 'to'
static unsigned replace(char from, char to, std::string &s)
{
  unsigned c = 0;
  for (char & i : s)
    if (i == from)
    {
      c++;
      i = to;
    }
    return c;
}


std::string timestamp()
{
  char tmpbuf[128];
  std::time_t ltime;
  struct std::tm *today;

  // Get UNIX-style time and display as number and string.
  std::time( &ltime );

  // Convert to time structure and adjust for PM if necessary.
  today = std::localtime( &ltime );

  // Use strftime to build a customized time string.
  std::strftime( tmpbuf, 128, "%Y-%m-%d %H:%M:%S", today );
  return std::string(tmpbuf);
}


//: A historical measurement recording framework.
// Currently the function will append the measurement to the file specified
// by ${MBL_TEST_SAVE_MEASUREMENT_PATH}/measurement_path, and exports it to Dart.
// In the longer term it may save the value via Dart2.
void mbl_test_save_measurement( const std::string &measurement_path, double value)
{
  std::cout << "<DartMeasurement name=\"" <<
    vul_file::strip_directory(measurement_path) <<
    "\" type=\"numeric/float\">"<<value<<"</DartMeasurement>" << std::endl;

  char * cpath = std::getenv("MBL_TEST_SAVE_MEASUREMENT_ROOT");
  std::string path(cpath?cpath:"");
  if (path.empty())
    path = MBL_CONFIG_TEST_SAVE_MEASUREMENT_ROOT;
  if (path.empty()) // Nobody wants the measurements stored this way.
    return;

  std::string config = MBL_CONFIG_BUILD_NAME;
  if (config.empty()) config="DEFAULT_CONFIG";

  path += '/' + measurement_path + ".txt";
  replace('\\', '/', path); // replace Windows-style "\" with Unix-style "/"
  path = vul_expand_path(path); // removes trailing or repeated "/"
  vul_file::make_directory_path(vul_file::dirname(path));
  std::ofstream file(path.c_str(), std::ios::app | std::ios::out);

  if (!file)
    std::cerr << "ERROR: mbl_test_save_measurement: Unable to open file " << path.c_str()
             << "ERRNO: " << errno << '\n';
  else
    file << timestamp() << ' ' << MBL_CONFIG_BUILD_NAME << ' ' << std::setprecision(15) << value << std::endl;
}
