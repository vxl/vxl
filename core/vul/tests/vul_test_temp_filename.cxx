// Amitha Perera <perera@cs.rpi.edu>

#include <vul/vul_temp_filename.h>
#include <vul/vul_test.h>
#include <vpl/vpl.h>  // for vpl_unlink and vpl_sleep
#include <vcl_fstream.h>

int main()
{
  vul_test_start("Temporary filename");

  {
    vul_test_begin("Writing to temporary filename");

    vcl_string filename = vul_temp_filename();

    vcl_ofstream ostr( filename.c_str() );
    bool status_good = ostr.good();
    // Under Windows, an open file cannot be unlinked.
    ostr.close();
    if( vpl_unlink( filename.c_str() ) == -1) {
      vcl_cerr << "Unlink failed!" << vcl_endl;
      status_good=false;
    }
    
    vul_test_perform( status_good );
  }

  {
    vul_test_begin("Testing multiple calls");

    vcl_string filename1 = vul_temp_filename();
    vcl_string filename2 = vul_temp_filename();

    vul_test_perform( filename1 != filename2 );
  }

  return vul_test_summary();
}
