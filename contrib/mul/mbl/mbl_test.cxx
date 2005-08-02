//:
// \file
// \brief A place for useful things associated with testing.
// \author iscott
// \date  Aug 2005

#include <vcl_sstream.h>
#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_ctime.h>
#include <vcl_cstdlib.h>
#include <vpl/vpl.h>
#include <vul/vul_file.h>
#include <mbl/mbl_config.h>


vcl_string timestamp()
{
    char tmpbuf[128];
    vcl_time_t ltime;
    struct vcl_tm *today;

    /* Get UNIX-style time and display as number and string. */
    vcl_time( &ltime );

    /* Convert to time structure and adjust for PM if necessary. */
    today = vcl_localtime( &ltime );

    /* Use strftime to build a customized time string. */
    vcl_strftime( tmpbuf, 128,
      "%Y-%m-%d %H:%M:%S", today );
  return vcl_string(tmpbuf);
}



//: A historical measurement recording framework.
// Currently the function will append the measurement to the file specified
// by ${MBL_TEST_SAVE_MEASUREMENT_PATH}/measurement_path.
// In the longer term it may save the value via Dart2.
void mbl_test_save_measurement( const vcl_string &measurement_path, double value)
{
  char * cpath = vcl_getenv("MBL_TEST_SAVE_MEASUREMENT_ROOT");
  vcl_string path(cpath?cpath:"");
  if (path.empty())
    path = MBL_CONFIG_TEST_SAVE_MEASUREMENT_ROOT;
  if (path.empty()) // Nobody wants the measurements
    return;
  
  vcl_string config = MBL_CONFIG_BUILD_NAME;
  if (config.empty()) config="DEFAULT_CONFIG";
  
  path += '/' + measurement_path + ".txt";
  vul_file::make_directory_path(vul_file::dirname(path));
  vcl_ofstream file(path.c_str(), vcl_ios_app | vcl_ios_out);
  file << timestamp() << " " << MBL_CONFIG_BUILD_NAME << " " << value << vcl_endl;
}



