//:
// \file
// \author Andy Miller
// \date 26-Oct-2010
#include <boxm2/boxm2_scene.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vgl/vgl_point_3d.h>

void test_scene()
{
  
  //test xml file 
  vcl_string root_dir  = testlib_root_dir();
  vcl_string test_file = root_dir + "/contrib/brl/bseg/boxm2/tests/test.xml"; 

  boxm2_scene scene(test_file);
  vcl_cout<<scene<<vcl_endl;
  
  vgl_point_3d<int> a(0, 1, 2);
  vcl_cout<<a<<vcl_endl;
  
}


TESTMAIN(test_scene);
