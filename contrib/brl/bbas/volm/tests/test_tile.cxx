#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <vil/vil_image_view.h>
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
  
  vsl_b_ofstream ofs("test_tile.bin");
  tiles[0].b_write(ofs);
  ofs.close();
  
  vsl_b_ifstream ifs("test_tile.bin");
  volm_tile t;
  t.b_read(ifs);
  TEST("tile 2", t.lat_, 37);
  double lat3, lon3;
  t.img_to_global(0, t.nj_, lon3, lat3);
  TEST_NEAR("tile 2 img to global lat ", lat3, lat2, 0.01);
  TEST_NEAR("tile 2 img to global lon ", lon3, lon2, 0.01);
}


TESTMAIN(test_tile);
