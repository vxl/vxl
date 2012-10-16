#include <testlib/testlib_test.h>
#include <boxm2/volm/boxm2_volm_wr3db_index.h>

#define DROP_FOLDER "C:/Users/ozge/Dropbox/projects/FINDER/simple_index/testscene/"

static void test_volm_wr3db_index()
{
  boxm2_volm_wr3db_index ind;
  ind.values_.reserve(10);
  //vcl_cout << 
  
  
  //TEST_NEAR("VRML Orientation ",(rot_axis-res_axis).length(),0.0,1e-3);

}

TESTMAIN(test_volm_wr3db_index);

