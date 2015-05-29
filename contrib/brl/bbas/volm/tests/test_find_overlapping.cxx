#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_2d.h>

#include <volm/volm_satellite_resources.h>
#include <volm/volm_satellite_resources_sptr.h>

#include <vul/vul_file.h>


static void test_find_overlapping()
{
  // resource file created by create_satellite_resources.py, which calls 
  vcl_string resource_file = "/mnt/finderdata/p1b_data/satellite_resources_wr3_ver3_all_images_no_ps.bin";
  vcl_string kml_file = "/mnt/finderdata/p1b_data/satellite_resources/jordan-mafraq_seed_region.kml";

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

static void compute_intersection()
{
  // resource file created by create_satellite_resources.py, which calls 
  vcl_string resource_file = "/home/sgrichar/mnt/finderdata/p1b_data/satellite_resources_wr3_ver3_all_images_no_ps.bin";
  vcl_string kml_file = "/home/sgrichar/mnt/finderdata/p1b_data/satellite_resources/jordan-mafraq_seed_region.kml";

  volm_satellite_resources_sptr res = new volm_satellite_resources();
  vsl_b_ifstream is(resource_file);
  if(!is) { assert(false); }
  res->b_read(is);
  is.close();
  vcl_cout << "there are " << res->resources_size() << " resources in the file!\n";

  vcl_vector<vgl_polygon<double> > footprints;
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>(  6.96578484, -14.32022812)); 
    footprint.push_back(vgl_point_2d<double>(-0.56180803, -0.40059161)); 
    footprint.push_back(vgl_point_2d<double>(-27.60466641,  -6.32010066)); 
    footprint.push_back(vgl_point_2d<double>(-20.07707354, -20.23973717)); 
    footprint.push_back(vgl_point_2d<double>(  6.96578484, -14.32022812));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>(  9.86031084,  17.30602255)); 
    footprint.push_back(vgl_point_2d<double>(-16.65467243,  23.37292325)); 
    footprint.push_back(vgl_point_2d<double>(-38.62524118,  10.88856858)); 
    footprint.push_back(vgl_point_2d<double>(-12.11025791,   4.82166788)); 
    footprint.push_back(vgl_point_2d<double>(  9.86031084,  17.30602255));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>(-24.47583861,   0.79007923)); 
    footprint.push_back(vgl_point_2d<double>( -4.52928017, -18.30042274)); 
    footprint.push_back(vgl_point_2d<double>( 12.73037319,   0.69530667)); 
    footprint.push_back(vgl_point_2d<double>( -7.21618526,  19.78580864)); 
    footprint.push_back(vgl_point_2d<double>(-24.47583861,   0.79007923));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>(-10.4571369 , -18.17991572)); 
    footprint.push_back(vgl_point_2d<double>(-14.54043578, -11.46874204)); 
    footprint.push_back(vgl_point_2d<double>( -2.01418419,  10.16162632)); 
    footprint.push_back(vgl_point_2d<double>( 2.06911469,  3.45045264)); 
    footprint.push_back(vgl_point_2d<double>(-10.4571369 , -18.17991572));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>( 12.64561638,  -4.16966793)); 
    footprint.push_back(vgl_point_2d<double>( 10.00981543, -22.189241 )); 
    footprint.push_back(vgl_point_2d<double>( -7.10074225, -23.04330731)); 
    footprint.push_back(vgl_point_2d<double>(-4.4649413 , -5.02373424)); 
    footprint.push_back(vgl_point_2d<double>( 12.64561638,  -4.16966793));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>(  0.85833915,  35.76693877)); 
    footprint.push_back(vgl_point_2d<double>( 7.31103026,  9.67003435)); 
    footprint.push_back(vgl_point_2d<double>( 27.07958284, -17.26309788)); 
    footprint.push_back(vgl_point_2d<double>( 20.62689172,   8.83380654)); 
    footprint.push_back(vgl_point_2d<double>(  0.85833915,  35.76693877));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>(-17.54710261,  -7.46710868)); 
    footprint.push_back(vgl_point_2d<double>(-29.27027646,   3.58687291)); 
    footprint.push_back(vgl_point_2d<double>(-5.1306033 ,  7.17772329)); 
    footprint.push_back(vgl_point_2d<double>( 6.59257055, -3.8762583)); 
    footprint.push_back(vgl_point_2d<double>(-17.54710261,  -7.46710868));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>( 20.59928871,  12.71430972)); 
    footprint.push_back(vgl_point_2d<double>(-7.33740002, -1.75889139)); 
    footprint.push_back(vgl_point_2d<double>(-31.89662415, -11.51022845)); 
    footprint.push_back(vgl_point_2d<double>(-3.95993541,  2.96297266)); 
    footprint.push_back(vgl_point_2d<double>( 20.59928871,  12.71430972));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>( 2.32929353,  6.01126638)); 
    footprint.push_back(vgl_point_2d<double>(  5.13191029,  34.18634405)); 
    footprint.push_back(vgl_point_2d<double>( 24.04064296,  17.67837464)); 
    footprint.push_back(vgl_point_2d<double>( 21.2380262 , -10.49670302)); 
    footprint.push_back(vgl_point_2d<double>( 2.32929353,  6.01126638));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>(-3.92395365, -2.01537927)); 
    footprint.push_back(vgl_point_2d<double>(  1.95004508, -26.70582914)); 
    footprint.push_back(vgl_point_2d<double>(-23.36240902,  -8.46480089)); 
    footprint.push_back(vgl_point_2d<double>(-29.23640775,  16.22564899)); 
    footprint.push_back(vgl_point_2d<double>(-3.92395365, -2.01537927));
    footprints.push_back(footprint);
  }

  vcl_vector<unsigned> overlapping_ids;
  res->highly_intersecting_resources(overlapping_ids, footprints, 2, 5);

  for(int i=0; i < overlapping_ids.size(); ++i) {
    vcl_cout << overlapping_ids[i] << ",";
  }
  vcl_cout << vcl_endl;
}

TESTMAIN(test_find_overlapping);

TESTMAIN(compute_intersection);
