//:
// \file
// \author J.L. Mundy
// \date 08/28/15


#include <testlib/testlib_test.h>
#include <vcl_fstream.h>
#include <vul/vul_timer.h>
#include "../boxm2_vecf_cranium_params.h"
#include "../boxm2_vecf_cranium.h"
#include <vgl/vgl_point_3d.h>
//#define BUILD_TEST_CRANIUM
void test_cranium()
{
#ifdef BUILD_TEST_CRANIUM
  vcl_string base_dir = "c:/Users/mundy/VisionSystems/Janus/RelevantPapers/FacialMusclesExpression/skull/";
  vcl_string cranium_path = base_dir  + "skull-top-2x-r-zeroaxis-samp-1.0-r35-norm.txt";
  vcl_string cranium_display_path = base_dir  + "cranium_vrml.wrl";
  vcl_ifstream istr(cranium_path.c_str());
   if(!istr){
     vcl_cout << "Bad cranium file path " << cranium_path << '\n';
     return;
   }
   boxm2_vecf_cranium cran;
   cran.read_cranium(istr);

#if 0
    // display the cranium axis
   vcl_ofstream crstr(cranium_display_path.c_str());
   if(!crstr){
     vcl_cout << "Bad cranium display file path " << cranium_display_path << '\n';
     return;
   }
   cran.display_vrml(crstr);

   vgl_point_3d<double> p(-60.0, 0.0, 0.0);
   vul_timer t;
   unsigned n = 1000;
   double dn = static_cast<double>(n);
   for(unsigned i = 0; i<n; ++i)
     double d = cran.surface_distance(p);
   vcl_cout << "time per surface dist " << t.real()/dn << " msec\n";
#endif
#endif
}
TESTMAIN( test_cranium );
 
