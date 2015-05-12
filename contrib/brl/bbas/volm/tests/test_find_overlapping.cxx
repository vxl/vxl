#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_2d.h>

#include <volm/volm_satellite_resources.h>
#include <volm/volm_satellite_resources_sptr.h>

#include <vul/vul_file.h>


static void test_find_overlapping()
{
  // resource file created by create_satellite_resources.py, which calls 
  vcl_string resource_file = "/home/sgrichar/mnt/finderdata/p1b_data/satellite_resources_wr3_ver3_all_images_no_ps.bin";
  vcl_string kml_file = "/home/sgrichar/Dropbox (VSI)/satellite_resources/jordan-mafraq_seed_region.kml";

  volm_satellite_resources_sptr res = new volm_satellite_resources();
  vsl_b_ifstream is(resource_file);
  if(!is) { assert(false); }
  res->b_read(is);
  is.close();
  vcl_cout << "there are " << res->resources_size() << " resources in the file!\n";

  vcl_vector<vcl_string> overlapping_res;
  res->highly_overlapping_resources(overlapping_res, res, kml_file, 100.0f);

  for(int i=0; i < overlapping_res.size(); ++i) {
    vcl_cout << overlapping_res[i] << vcl_endl;
  }
  vcl_cout << vcl_endl;
}

TESTMAIN(test_find_overlapping);
