#include <testlib/testlib_test.h>
#include <volm/desc/volm_desc_ex.h>
#include <volm/desc/volm_desc_ex_2d.h>
#include <volm/desc/volm_desc_land.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_buffered_index.h>
#include <vnl/vnl_random.h>
#include <vul/vul_timer.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>

static void test_volm_desc_land()
{
  volm_desc_sptr land = new volm_desc_land(31);
  land->print();
  land->visualize("test_volm_desc_land.svg", 2);
  // load from a file
  std::string filename = "./location_category.txt";
  std::ofstream ofs(filename.c_str());
  ofs << "Developed/Low_Intensity";
  ofs.close();
  volm_desc_sptr land_cat = new volm_desc_land(filename);
  land_cat->print();
  TEST("NLCD land type 3 is located in correct bin", (*land)[6], 1);
  TEST("NLCD land type Developed/Low_Intensity is located in bin 09", (*land_cat)[4], 1);
}

static void test_volm_desc_ex_2d()
{
  // define the radius
  std::vector<double> radius;
  radius.push_back(100);  radius.push_back(50);  radius.push_back(200);
  // define heading intervals
  double h_width = 45.0;  double h_inc = 30.0;
  unsigned nland = 2;
  volm_desc_ex_2d* desc = new volm_desc_ex_2d(radius, h_width, h_inc, nland, 2);
  desc->print();
  // set two object into histogram
  unsigned land_id;  double distance, heading;
  land_id = 1;  distance = 222;  heading = 40;
  desc->set_count(distance, land_id, heading, (unsigned char)1);
  land_id = 1;  distance = 230;  heading = 70;
  desc->set_count(distance, land_id, heading, (unsigned char)1);
  desc->print();
}

static void test_volm_desc_ex()
{
  // create a depth_map_scene
  depth_map_scene_sptr depth_scene = new depth_map_scene;
  unsigned ni = 1280;
  unsigned nj = 720;
  depth_scene->set_image_size(ni, nj);
  // add a ground plane object
  vsol_point_2d_sptr p0= new vsol_point_2d(0.0, 720.0);
  vsol_point_2d_sptr p1= new vsol_point_2d(1280.0, 720.0);
  vsol_point_2d_sptr p2= new vsol_point_2d(1280.0, 500.0);
  vsol_point_2d_sptr p3= new vsol_point_2d(0.0, 500.0);
  std::vector<vsol_point_2d_sptr> verts;
  verts.push_back(p0);   verts.push_back(p1);
  verts.push_back(p2);   verts.push_back(p3);
  vsol_polygon_2d_sptr gp = new vsol_polygon_2d(verts);
  depth_scene->add_ground(gp, 0.0, 0.0, 0, "beach", 6);
  // add an object
  vsol_point_2d_sptr pb0 = new vsol_point_2d(640.0, 400.0);
  vsol_point_2d_sptr pb1 = new vsol_point_2d(940.0, 600.0);
  vsol_point_2d_sptr pb2 = new vsol_point_2d(340.0, 200.0);
  std::vector<vsol_point_2d_sptr> verts_bd;
  verts_bd.push_back(pb0);  verts_bd.push_back(pb1);  verts_bd.push_back(pb2);
  vsol_polygon_2d_sptr bp = new vsol_polygon_2d(verts_bd);
  vgl_vector_3d<double> np(1.0, 1.0, 0.0);
  depth_scene->add_region(bp, np, 100.0, 1000.0, "hotel", depth_map_region::FRONT_PARALLEL, 1, 15);
  std::string dms_bin_file = "./depth_map_scene.bin";
  vsl_b_ofstream ofs_dms(dms_bin_file);
  depth_scene->b_write(ofs_dms);
  ofs_dms.close();

  // define the radius
  std::vector<double> radius;
  radius.push_back(100);  radius.push_back(50);  radius.push_back(200);
  volm_desc_sptr desc = new volm_desc_ex(depth_scene, radius);
  desc->print();

  // create a existence histogram from index
  std::vector<unsigned char> values_dst(1176);
  std::vector<unsigned char> values_combine(1176);
  float solid_angle = 2.0f, vmin = 1.0f, dmax = 3000.0f;
  volm_spherical_container_sptr sph = new volm_spherical_container(solid_angle, vmin, dmax);
  std::map<double, unsigned char>& depth_interval_map = sph->get_depth_interval_map();
  std::vector<double> depth_interval;
  auto iter = depth_interval_map.begin();
  for (; iter != depth_interval_map.end(); ++iter)
    depth_interval.push_back(iter->first);
  vnl_random rand(9667566);
  for (unsigned i = 0; i < 1176; i++) {
    values_dst.push_back((unsigned char)rand.lrand32(254));
    values_combine.push_back((unsigned char)rand.lrand32(254));
  }

  // binary IO test
  desc->visualize("./test_desc_ex_dms.svg", 2);
  vsl_b_ofstream os("./volm_desc_ex_dms.bin");
  desc->b_write(os);
  os.close();

  vsl_b_ifstream is("./volm_desc_ex_dms.bin");
  volm_desc_sptr desc_in = new volm_desc_ex();
  desc_in->b_read(is);
  is.close();
  desc_in->print();
  desc_in->visualize("./test_desc_ex_dms_in.svg", 2);

  bool is_same = desc_in->name() == desc->name();
  is_same = is_same && desc_in->nbins() == desc->nbins();
  float score = desc->similarity(desc_in);
  std::cout << " for two same histogram, similarity score is " << score << std::endl;


  TEST ("beach bin should exist", desc->count(6), 1);
  TEST ("binary io is correct", is_same, true);
  TEST ("total area of the histogram", desc->get_area(), 2);
  TEST ("similarity of the histogram", score, 1.0f);

  vul_timer time;
  unsigned N = 1000;
  for (unsigned i = 0; i < N; i++) {
    volm_desc_sptr desc_index = new volm_desc_ex(values_dst, values_combine, depth_interval, radius);
    /*desc_index->print();
    desc_index->visualize("./test_desc_ex_index.svg", 2);*/
  }
  std::cout << N << " location, visibility intersection takes " << time.all()/1000.0 << " seconds. " << std::endl;

}

#if 0
static void params_io()
{
  std::string filename = "./ex_params.params";
  volm_buffered_index_params params;
  std::vector<double> radius;  radius.push_back(2.30);  radius.push_back(2.318);  radius.push_back(421.342);  radius.push_back(89342.1);
  unsigned nlands = 31;
  unsigned norients = 3;
  params.layer_size = 321;
  params.radius = radius;
  params.nlands = nlands;
  params.norients = norients;
  params.write_ex_param_file(filename);
  volm_buffered_index_params params_in;
  params_in.read_ex_param_file(filename);
  std::cout << " layer_size = " << params_in.layer_size << std::endl;
  std::cout << " nlands = " << params_in.nlands << std::endl;
  std::cout << " norient = " << params_in.norients << std::endl;
  std::cout << " radius = ";
  for (std::vector<double>::iterator vit = params_in.radius.begin(); vit != params_in.radius.end(); ++vit)
    std::cout << *vit << ' ';
  std::cout << std::endl;
}
#endif

static void test_volm_descriptor()
{


  std::cout << "======================== test the land category histogram ================== " << std::endl;
  test_volm_desc_land();
  std::cout << "============================================================================ " << std::endl;

  std::cout << "======================== test the existence histogram ====================== " << std::endl;
  test_volm_desc_ex();
  std::cout << "============================================================================ " << std::endl;

  std::cout << "======================== test the ex_2d histogram with heading intervals === " << std::endl;
  test_volm_desc_ex_2d();
  std::cout << "============================================================================ " << std::endl;

#if 0
  std::vector<unsigned char> values;
  for (unsigned i = 0; i < 14; i++)
    values.push_back(i);
  volm_desc_sptr desc_base = new volm_desc(values);
  desc_base->print();
#endif
}

TESTMAIN( test_volm_descriptor );
