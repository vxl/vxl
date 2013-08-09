#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vsl/vsl_binary_io.h>
#include <vul/vul_timer.h>
#include <bbas/volm/volm_tile.h>

static void test_tile()
{
  vcl_vector<volm_tile> tiles = volm_tile::generate_p1_wr1_tiles(); 
  TEST("tile 1", tiles[0].lat_, 37);
  
  double lat, lon, elev;
  tiles[0].cam_.lvcs()->get_origin(lat, lon, elev);
  
  double lat2, lon2;
  tiles[0].img_to_global(0, tiles[0].nj_, lon2, lat2);
  TEST_NEAR("tile 1 img to global", lat2, 37, 0.01);
  
  unsigned i, j;
  tiles[0].img_to_global(0, tiles[0].nj_-1, lon2, lat2);
  TEST("tile1 global to img", tiles[0].global_to_img(lon2, lat2, i, j), true);
  TEST("tile1 global to img", i, 0);
  vcl_cout << "j=" << j << ", tiles[0].nj_ = " <<  tiles[0].nj_ << '\n';
  TEST("tile1 global to img", j+1, tiles[0].nj_);
  
  TEST("tile1 global to img", tiles[0].global_to_img(lon2+5, lat2, i, j), false);
  
  volm_tile tt(37, -118, tiles[0].scale_i_, tiles[0].scale_j_, tiles[0].ni_, tiles[0].nj_);
  TEST("tt global to img", tt.global_to_img(lon2, lat2, i, j), true);
  TEST("tt global to img", i, 0);
  vcl_cout << "j=" << j << ", tt.nj_ = " <<  tt.nj_ << '\n';
  TEST("tt global to img", j+1, tt.nj_);

  //volm_tile ttt(37.622991f, 118.209999f, 'N', 'W', 1.108007f, 0.930012f, (unsigned)10000, (unsigned)10000);
  volm_tile ttt(37.622991f, 118.209999f, 'N', 'W', 1.0f, 0.9f, (unsigned)10000, (unsigned)10000);
  vcl_cout << "width is: " << ttt.calculate_width() << " meters = " << ttt.calculate_width()/1000.0 << " Km..\n";
  vcl_cout << "height is: " << ttt.calculate_height() << " meters = " << ttt.calculate_height()/1000.0 << " Km..\n";
  vcl_cout << "one pixel is: " << ttt.calculate_width()/ttt.ni_ << " meters..\n";
  ttt.write_kml("./t0.kml", 1000);

  // test the transformation as tile is in southern hemisphere
  volm_tile tile_south(34, 72, 'S', 'W', 1.0, 1.0, 3601, 3601);
  TEST("tile_south global to image", tile_south.global_to_img(-72, -34, i, j), true);
  TEST("tile_south global to image", i, 0);
  TEST("tile_south global to image", j, 3600);
  TEST("tile_south global to image", tile_south.global_to_img(-71.5, -33.5, i, j), true);
  TEST("tile_south global to image", i, 1800);
  TEST("tile_south global to image", j, 1800);
  double lat_south, lon_south;
  tile_south.img_to_global(1800, 1800, lon_south, lat_south);
  TEST_NEAR("tile_south image to global", lon_south, -71.5, 0.01);
  TEST_NEAR("tile_south image to global", lat_south, -33.5, 0.01);
  tile_south.img_to_global(3600, 3600, lon_south, lat_south);
  TEST_NEAR("tile_south image to global", lon_south, -71, 0.01);
  TEST_NEAR("tile_south image to global", lat_south, -34, 0.01);

  vsl_b_ofstream ofs("test_tile.bin");
  tiles[0].b_write(ofs);
  ofs.close();
  
  vsl_b_ifstream ifs("test_tile.bin");
  volm_tile t;
  t.b_read(ifs);
  TEST("tile 2", t.lat_, 37);
  double lat3, lon3;
  t.img_to_global(0, t.nj_, lon3, lat3);
  TEST_NEAR("tile 2 img to global lat", lat3, lat2, 0.01);
  TEST_NEAR("tile 2 img to global lon", lon3, lon2, 0.01);

  vil_image_view<float> img = vil_load("lidar_N32.7500W079.8750_S0.0625x0.0625.tif");
  TEST("Load lidar?", !img, false);
  if (img) {
    volm_tile ttest("lidar_N32.7500W079.8750_S0.0625x0.0625.tif", img.ni(), img.nj());

    lat = 32.776000;
    lon = -79.813000;
    unsigned ii, jj;
    bool inside = ttest.global_to_img(lon, lat, ii, jj);
    TEST("Inside?", inside, true);
    double val = img(ii, jj);
    vcl_cout << val;
  }
}

TESTMAIN(test_tile);
