#include <testlib/testlib_test.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_stream_scene_cache.h>

#define DROP_FOLDER "C:/Users/ozge/Dropbox/projects/FINDER/simple_index/testscene/"

static void test_volm_wr3db_index()
{
  //: load a test scene
  vcl_stringstream name; name << DROP_FOLDER; name << "scene_rgb.xml";
  boxm2_scene_sptr scene = new boxm2_scene(name.str());
  vcl_cout << "# of blocks in scene: " << scene->num_blocks() << vcl_endl;
  vgl_box_3d<double> bbox = scene->bounding_box();
  vcl_cout << "global scene bbox: " << bbox << vcl_endl;
  
  vcl_vector<vcl_string> data_types;
	vcl_vector<vcl_string> identifiers;
	data_types.push_back("alpha");
	identifiers.push_back("");
  boxm2_stream_scene_cache_sptr cache = new boxm2_stream_scene_cache( scene, data_types,identifiers);
  vcl_cout << "stream cache, tot bytes per block: " << cache->total_bytes_per_block_ << " tot bytes per data: " << cache->total_bytes_per_data_["alpha"] << vcl_endl;
  vcl_cout << "tot floats per data: " << cache->total_bytes_per_data_["alpha"]/4.0 << vcl_endl;
   
  // now print out alpha's
  float * buf = (float *)cache->data_buffers_["alpha"];
  for (int i = 0; i < (int)(cache->total_bytes_per_data_["alpha"]/4.0); i++)
  {
    vcl_cout << buf[i] << " ";
  }
  vcl_cout << "\n";
  
  
  //TEST_NEAR("VRML Orientation ",(rot_axis-res_axis).length(),0.0,1e-3);

}

TESTMAIN(test_volm_wr3db_index);

