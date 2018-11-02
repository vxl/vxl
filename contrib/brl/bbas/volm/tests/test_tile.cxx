#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
  std::vector<volm_tile> tiles = volm_tile::generate_p1_wr1_tiles();
  // test the lower left corner
  TEST("tile 1", tiles[0].lat_, 37);
  TEST("tile 1", tiles[0].lon_, 118);

  double lat, lon, elev;
  tiles[0].cam_.lvcs()->get_origin(lat, lon, elev);

  // test img to global for tile at North hemisphere and West direction
  // Note that the tile boundary corresponds to pixel (1,1) to (3600,3600)
  // pixels from (0,0) to (0,3600) and pixels from (0,0) to (3600,0) are defined as overlaped region
  // pixel (0,0) correspond to (lon-0.5/nj, lat+1+0.5/ni)
  double lat2, lon2;
  tiles[0].img_to_global(0, tiles[0].nj_-1, lon2, lat2);
  TEST_NEAR("tile 1 img to global", lat2,   37+tiles[0].scale_j()*0.5/tiles[0].nj(), 0.99*tiles[0].scale_j()*0.5/tiles[0].nj());
  TEST_NEAR("tile 1 img to global", lon2, -118-tiles[0].scale_i()*0.5/tiles[0].ni(), 0.99*tiles[0].scale_i()*0.5/tiles[0].ni());
  tiles[0].img_to_global(tiles[0].ni_-1, 0, lon2, lat2);
  TEST_NEAR("tile 1 img to global", lat2,   38+tiles[0].scale_j()*0.5/tiles[0].nj(), 0.99*tiles[0].scale_j()*0.5/tiles[0].nj());
  TEST_NEAR("tile 1 img to global", lon2, -117-tiles[0].scale_i()*0.5/tiles[0].ni(), 0.99*tiles[0].scale_i()*0.5/tiles[0].ni());
  unsigned i_bdry, j_bdry;
  tiles[0].global_to_img(-tiles[0].lon_, tiles[0].lat_, i_bdry, j_bdry);
  TEST("tile1 global to img(boundary)", i_bdry, 0);
  TEST("tile1 global to img(boundary)", j_bdry, 3600);

  unsigned i, j;
  TEST("tile1 global to img", tiles[0].global_to_img(lon2, lat2, i, j), true);
  std::cout << " i = " << i << ", tiles[0].ni_ = " << tiles[0].ni() << '\n';
  std::cout << " j = " << j << ", tiles[0].nj_ = " << tiles[0].nj() << '\n';
  TEST("tile1 global to img", j, 0);
  TEST("tile1 global to img", i+1, tiles[0].nj_);
  // for location out of current tile, it will return nothing
  TEST("tile1 global to img", tiles[0].global_to_img(lon2+5, lat2, i, j), false);

  volm_tile tt(38, -117, tiles[0].scale_i_, tiles[0].scale_j_, tiles[0].ni_, tiles[0].nj_);
  TEST("tt global to img", tt.global_to_img(lon2, lat2, i, j), true);
  TEST("tt global to img", i, 0);
  std::cout << " j = " << j << ", tt.nj_ = " <<  tt.nj_ << '\n';
  TEST("tt global to img", j+1, tt.nj_);

  //volm_tile ttt(37.622991f, 118.209999f, 'N', 'W', 1.108007f, 0.930012f, (unsigned)10000, (unsigned)10000);
  volm_tile ttt(37.622991f, 118.209999f, 'N', 'W', 1.0f, 0.9f, (unsigned)10000, (unsigned)10000);
  std::cout << "width is: " << ttt.calculate_width() << " meters = " << ttt.calculate_width()/1000.0 << " Km..\n";
  std::cout << "height is: " << ttt.calculate_height() << " meters = " << ttt.calculate_height()/1000.0 << " Km..\n";
  std::cout << "one pixel is: " << ttt.calculate_width()/ttt.ni_ << " meters..\n";
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

  // test tile at North hemisphere and East direction
  volm_tile tile_east(12, 77, 'N', 'E', 1.0, 1.0, 3601, 3601);
  TEST("tile_east global to image", tile_east.global_to_img(77, 13, i, j), true);
  TEST("tile_east global to image", i, 0);
  TEST("tile_east global to image", j, 0);
  TEST("tile_east global to image", tile_east.global_to_img(78, 12, i, j), true);
  TEST("tile_east global to image", i, 3600);
  TEST("tile_east global to image", j, 3600);
  double lat_east, lon_east;
  tile_east.img_to_global(1800, 1800, lon_east, lat_east);
  TEST_NEAR("tile_east global to image", lon_east, 77.5, 0.01);
  TEST_NEAR("tile_east global to image", lat_east, 12.5, 0.01);
  tile_east.img_to_global(3600,3600, lon_east, lat_east);
  TEST_NEAR("tile_east image to global", lon_east, 78, 0.01);
  TEST_NEAR("tile_east image to global", lat_east, 12, 0.01);
  tile_east.img_to_global(0,0, lon_east, lat_east);
  TEST_NEAR("tile_east image to global", lon_east, 77, 0.01);
  TEST_NEAR("tile_east image to global", lat_east, 13, 0.01);

  vsl_b_ofstream ofs("test_tile.bin");
  tiles[0].b_write(ofs);
  ofs.close();

  vsl_b_ifstream ifs("test_tile.bin");
  volm_tile t;
  t.b_read(ifs);
  TEST("tile 2", t.lat_, 37);
  double lat3, lon3;
  t.img_to_global(t.ni()-1, 0, lon3, lat3);
  TEST_NEAR("tile 2 img to global lat", lat3, lat2, 0.01);
  TEST_NEAR("tile 2 img to global lon", lon3, lon2, 0.01);

}

TESTMAIN(test_tile);
