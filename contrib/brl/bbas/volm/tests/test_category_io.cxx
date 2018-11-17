#include <testlib/testlib_test.h>
#include <volm/volm_category_io.h>
#include <vcl_where_root_dir.h>

static void write_osm_samples()
{
  // input file sample
  // transformation from open_street_map to
  std::string osm_land_file = "./osm_to_volm_land.txt";
  std::ofstream ofs_land(osm_land_file.c_str());
  // header
  ofs_land << "Key Value id name level\n";
  // contents
  ofs_land << "landuse reservoir 1 Open_Water 0 0\n";
  ofs_land << "landuse greenhouse_horticulture 13 Cultivated_Crops 0 0\n";
  ofs_land << "man_made monitoring_station 15 building 4 1.5\n";
  ofs_land << "emergency ambulance_station 48 hospital 2 2\n";
  ofs_land << "tourism viewpoint 141 tourism_viewpoint 4 3.5\n";
  ofs_land << "natural shadow 0 invalid 0 1.2\n";
  ofs_land.close();

  // road width file
  std::string road_width_file = "./osm_road_width.txt";
  std::ofstream ofs_road(road_width_file.c_str());
  // header
  ofs_road << "Key Value Width(0 means the categroy is not a road)\n";
  // contents
  ofs_road << "abutters commercial 1.00\n";
  ofs_road << "amenity kindergarten 0.00\n";
  ofs_road << "barrier city_wall 12.00\n";
  ofs_road << "highway living_street 2.50\n";
  ofs_road.close();
}

static void test_category_io_read_land_category(const std::string& filename)
{
  std::map<std::pair<std::string, std::string>, volm_land_layer> osm_land_table;
  TEST("land_category_io", volm_osm_category_io::load_category_table(filename, osm_land_table), true);
  auto mit = osm_land_table.begin();
  for (; mit != osm_land_table.end(); ++mit) {
    std::cout << " tag = " << mit->first.first << ", value = " << mit->first.second << ", ---> id = ";
    mit->second.print();
    std::cout << std::endl;
  }
}

static void test_category_io_load_road_width(std::string const& filename)
{
  std::map<std::pair<std::string, std::string>, float> volm_road_width;
  TEST("road_width_io", volm_osm_category_io::load_road_width_table(filename, volm_road_width), true);
  auto mit = volm_road_width.begin();
  for (; mit != volm_road_width.end(); ++mit)
    std::cout << " tag = " << mit->first.first << ", value = " << mit->first.second << " ---> width = "
             << volm_road_width[std::pair<std::string, std::string>(mit->first.first, mit->first.second)] << std::endl;
}

static void test_category_io()
{
  // create sample files
  write_osm_samples();

  // test the method of loading land category
  std::string osm_to_volm_file;
  osm_to_volm_file = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/volm/osm_to_volm_labels.txt";
  std::map<std::pair<std::string, std::string>, volm_land_layer> osm_land_table;
  test_category_io_read_land_category(osm_to_volm_file);

  // output the hard coded ncld table
  for (auto & mit : volm_osm_category_io::nlcd_land_table) {
    std::cout << " nlcd id = " << mit.first << " ----> ";  mit.second.print();  std::cout << '\n';
  }

  // output the hard coded geo_cover table
  for (auto & mit : volm_osm_category_io::geo_land_table) {
    std::cout << " geo_cover id = " << mit.first << " ----> ";  mit.second.print();  std::cout << '\n';
  }

  // print out the road junction table
  for (auto & mit : volm_osm_category_io::road_junction_table) {
    std::cout << " id1 = " << mit.first.first
             << " id2 = " << mit.first.second
             << " ---> id = " << (unsigned)mit.second.id_
             << " name = " << mit.second.name_
             << " level = " << mit.second.level_
             << " width = " << mit.second.width_
             << " color = " << mit.second.color_
             << std::endl;
  }

  // output the volm_land_table
  for (auto & mit : volm_osm_category_io::volm_land_table) {
    std::cout << " id = " << mit.first << " --> ";  mit.second.print(); std::cout << '\n';
  }

  for (auto & vit : volm_osm_category_io::volm_category_name_table) {
    unsigned id = volm_osm_category_io::volm_land_table_name[vit].id_;
    std::cout << " id = " << id << " ---> name = " << vit << std::endl;
  }
}

TESTMAIN(test_category_io);
