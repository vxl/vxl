#include <testlib/testlib_test.h>
#include <volm/desc/volm_desc_ex_matcher.h>

static void test_ex_matcher()
{
#if 0
  vcl_string dm_file = "z:/projects/FINDER/test1/p1a_test1_20/p1a_test1_20.vsl";
  depth_map_scene_sptr dm = new depth_map_scene;
  vsl_b_ifstream dis(dm_file.c_str());
  dm->b_read(dis);
  dis.close();
  // define the radius
  vcl_vector<double> radius;
  radius.push_back(100);  radius.push_back(50);  radius.push_back(200);

  volm_desc_matcher_sptr ex_matcher = new volm_desc_ex_matcher(dm, radius, "Visibility");

  vcl_cout << " matcher is " << ex_matcher->name() << vcl_endl;

  volm_desc_sptr query = ex_matcher->create_query_desc();

  query->print();
  query->visualize("./p1a_test1_20_ex_desc.svg", 2);

  vsl_b_ifstream is("./volm_desc_ex_dms.bin");
  volm_desc_sptr desc_in = new volm_desc_ex();
  desc_in->b_read(is);
  is.close();
#endif
}

TESTMAIN( test_ex_matcher );
