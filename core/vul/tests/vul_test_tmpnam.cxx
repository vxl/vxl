// Amitha Perera <perera@cs.rpi.edu>

#include <vul/vul_tmpnam.h>
#include <vul/vul_test.h>
#include <vpl/vpl.h>  // for vpl_unlink and vpl_sleep
#include <vcl_fstream.h>
#include <vcl_cstdio.h> // for P_tmpdir

int main()
{
  vul_test_start("Temporary filename");

  {
    vul_test_begin("Writing to temporary filename");

    vcl_string filename = P_tmpdir; // defined in stdio.h
    filename += "testtmpXXX";
    unsigned int count = 0;
    while( count < 10 ) {
      if( tmpnam( filename ) ) {
        break;
      }
      vpl_sleep( 1 );
      ++count;
    }
    vcl_ofstream ostr( filename.c_str() );

    vul_test_perform( count<10 && bool(ostr) );

    vpl_unlink( filename.c_str() );
  }

  {
    vul_test_begin("Testing multiple calls");
    vcl_string filename1 = P_tmpdir; // defined in stdio.h
    filename1 += "testtmpXXX";
    vcl_string filename2 = filename1;
    unsigned int count1 = 0;
    while( count1 < 10 ) {
      if( tmpnam( filename1 ) ) {
        break;
      }
      vpl_sleep( 1 );
      ++count1;
    }
    unsigned int count2 = 0;
    while( count2 < 10 ) {
      if( tmpnam( filename2 ) ) {
        break;
      }
      vpl_sleep( 1 );
      ++count2;
    }

    vul_test_perform( count1<10 && count2<10 && filename1 != filename2 );
  }

  return vul_test_summary();
}
