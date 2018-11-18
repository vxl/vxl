//:
// \file
// \author J.L. Mundy
// \date 08/28/15


#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timer.h>
#include "../boxm2_vecf_cranium_params.h"
#include "../boxm2_vecf_cranium.h"
#include "../boxm2_vecf_cranium_scene.h"
#include <vgl/vgl_point_3d.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>

//#define BUILD_TEST_CRANIUM
void test_cranium()
{
#ifdef BUILD_TEST_CRANIUM
  std::string base_dir = "c:/Users/mundy/VisionSystems/Janus/RelevantPapers/FacialMusclesExpression/skull/";
  std::string scene_dir = "c:/Users/mundy/VisionSystems/Janus/experiments/vector_flow/cranium/";
  //std::string cranium_geo_path = base_dir  + "skull-top-2x-r-zeroaxis-samp-1.0-r35-norm.txt";
  std::string cranium_display_path = base_dir  + "cranium_vrml.wrl";
#if 0
  std::ifstream istr(cranium_geo_path.c_str());
   if(!istr){
     std::cout << "Bad cranium file path " << cranium_path << '\n';
     return;
   }
#endif
  std::string cranium_scene_path = scene_dir + "cranium_8.xml";
  if(!vul_file::exists(cranium_scene_path))
    {
      std::cout<<"cranium scene file) does not exist"<<std::endl;
      return;
    }

  std::string cranium_geo_path = scene_dir + "cranium_geo.txt";
  if(!vul_file::exists(cranium_geo_path))
    {
      std::cout<<"cranium geometry file) does not exist"<<std::endl;
      return;
    }

  //  boxm2_vecf_cranium_scene crscene(cranium_scene_path, cranium_geo_path);
  //const boxm2_vecf_cranium& crand = crscene.cranium_geo();
  boxm2_vecf_cranium_scene crscene(cranium_scene_path);
  boxm2_scene_sptr scene = crscene.scene();
  if(!scene)
    return;
  std::string target_scene_path = scene_dir + "target_cranium_8.xml";

  if(!vul_file::exists(target_scene_path))
    {
      std::cout<<"target scene file) does not exist"<<std::endl;
      return;
    }
  boxm2_scene_sptr target = new boxm2_scene(target_scene_path);
  if(!target)
    return;
  crscene.map_to_target(target);

  boxm2_cache::instance()->write_to_disk();

#if 0
   boxm2_vecf_cranium cran;
   cran.read_cranium(istr);
    // display the cranium axis
   std::ofstream crstr(cranium_display_path.c_str());
   if(!crstr){
     std::cout << "Bad cranium display file path " << cranium_display_path << '\n';
     return;
   }
   cran.display_vrml(crstr);

   vgl_point_3d<double> p(-60.0, 0.0, 0.0);
   vul_timer t;
   unsigned n = 1000;
   double dn = static_cast<double>(n);
   for(unsigned i = 0; i<n; ++i)
     double d = cran.surface_distance(p);
   std::cout << "time per surface dist " << t.real()/dn << " msec\n";
#endif
#endif
}
TESTMAIN( test_cranium );
