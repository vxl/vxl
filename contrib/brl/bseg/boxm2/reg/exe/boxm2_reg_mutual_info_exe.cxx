//:
// \file
// \brief  First attempt at multi gpu render
// \author Andy Miller
// \date 13-Oct-2011
#include <boxm2/reg/boxm2_reg_mutual_info.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <vgl/vgl_vector_3d.h>
#include <vul/vul_arg.h>
#include <vul/vul_timer.h>
#include <vcl_algorithm.h>

struct ltstr
{
  bool operator()(const vcl_pair<int,int> & s1, const vcl_pair<int,int> & s2) const
  {
    if (s1.first < s2.first)
      return true;

    if (s1.first == s2.first)
    {
      if (s1.second < s2.second )
        return true;
    }
    return false;
  }
};

int main(int argc,  char** argv)
{
  //init vgui (should choose/determine toolkit)
  vul_arg<vcl_string> sceneA_file("-sceneA", "sceneA filename", "");
  vul_arg<vcl_string> sceneB_file("-sceneB", "sceneB filename", "");
  vul_arg_parse(argc, argv);

  //create scene
  boxm2_scene_sptr sceneA = new boxm2_scene(sceneA_file());
  boxm2_scene_sptr sceneB = new boxm2_scene(sceneB_file());
  boxm2_lru_cache::create(sceneA);
  boxm2_cache_sptr cacheA = boxm2_cache::instance();
 // boxm2_cache_sptr cacheB = new boxm2_lru_cache(sceneB);

  vcl_vector<float> MIs;
  vcl_map<vcl_pair<int,int>, float,ltstr > Ts;
  vul_timer t;
  for (int i=0; i<1;) {
    for (int j=0; j<1; ) {
      vgl_vector_3d<double> trans(i,j,0);
      t.mark();
      float MI = register_world(cacheA, sceneB, trans);
      vcl_cout<<"Time Taken is "<<t.all()<<vcl_endl;
      Ts[vcl_pair<int,int>(i,j)] = MI;
      vcl_cout<<"Trans: "<<trans<<' '<<MI<<' '<<vcl_endl;
      j=j+2;
    }
    i=i+2;
  }
  for (int i=0; i<1; ) {
    for (int j=0; j<1; ) {
      vcl_cout<<Ts[vcl_pair<int,int> (i,j)]<<' ';
      j=j+2;
    }
    i=i+2;
    vcl_cout<<vcl_endl;
  }
  return 0;
}
