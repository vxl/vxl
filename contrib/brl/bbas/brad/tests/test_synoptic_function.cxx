#include <testlib/testlib_test.h>
#include <brad/brad_synoptic_function_1d.h>
#include <vcl_cstdlib.h> // for rand()
#include <vcl_fstream.h>

static void test_synoptic_function()
{
  START("synoptic function test");
  brad_synoptic_function_1d sf;
  if(!sf.load_samples("E:/mundy/CVGroup/Nibbler/Notes/Experiments/intensities_and_viewing_direction.txt"))
    return;
  unsigned n = sf.size();
  for (unsigned i = 0; i<n; ++i){
    double s = sf.arc_length(i);

    vcl_cout << s << ' ' << sf.intensity(i) 
             << ' ' << sf.cubic_interp_inten(s)<< ' ' << sf.vis(i) <<  '\n';
  }
  vcl_cout << "Fit Error " << vcl_sqrt(sf.fit_error()) <<'\n';;
}

TESTMAIN( test_synoptic_function );
